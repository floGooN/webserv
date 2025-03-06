#include "Client.hpp"
#include <cstring>
#include "CGI.hpp"
#include "UtilParsing.hpp"
#include <unistd.h>
#include <sys/wait.h>

class ErrorCGI : virtual public std::exception 
{
    public:
        ErrorCGI(const std::string &message, int code) : _message(message), _code(code){}
        virtual const char *	what() const throw() 
        {
            (void) _code;
             return _message.c_str();
        }
        virtual ~ErrorCGI() throw() {};

    private:
        std::string _message;
        int         _code;
};


// ici voir si il faut renvoyer une page error dans le catch
void processCGI(const std::string &path, Server server, Request req)
{
    std::string response;
    try 
    {
        if (UtilParsing::fileExits(path) != 0)
            throw ErrorCGI("Not found", 404);
        if (access(path.c_str(), X_OK) != 0)
            throw ErrorCGI("Not found", 404);
        if (checkExtensionCGI(path, server) != 0)
            throw ErrorCGI("Bad Gateway", 502);
        if (moveToDirectoryScript(extractDirectory(path)) != 0)
            throw ErrorCGI("Internal server error", 500);
        response = executeCGI(path, server, req); // ici plus tard renvoyer directement dans la value response
        if (response.empty())
            throw ErrorCGI("Bad Gateway", 502);
    }
    catch (const ErrorCGI& e)
    {
        std::cerr << e.what() << std::endl;
    }
        
}
// fonction pour check le type d'extension du script demander pour verifier si le server la supporte
// pareil la recuperer dans le parsing le nom par server  le nom du script cgi pour verifier son extension
bool checkExtensionCGI(const std::string &path, Server server)
{
   std::string cgi_path = "support.pl";

    if (UtilParsing::recoverExtension(path) == UtilParsing::recoverExtension(cgi_path))
        return true;
    return false;
}

// fonction pour extract le nom du dossier
std::string extractDirectory(const std::string &path)
{
    std::string directory;

    size_t posEndDirectory = path.find_last_of("/");
    directory = path.substr(0, posEndDirectory);

    return directory;
}

// fonction qui permet d'aller dans le bon dossier avec execution du processus cgi
bool moveToDirectoryScript(const std::string &directory)
{
    if (chdir(directory.c_str()) != 0)
        return false;
    return true;
}

// ici recuperer les values de l'env avec les valeurs de la request/server
std::string _method,  _params, _contentType, _http, _httpReferer, _remoteAddr, _remotePort, _scriptName, _pathInfo;
char** initEnv(Request req, Server server)
{
      std::string environnement[] = {
        "REQUEST_METHOD=" + req.gettype(),
        "QUERY_STRING=" + (req.gettype().compare("GET") == 0) ? ParseUri(req.geturi())  : req.getbody(),
        "CONTENT_TYPE=" + req.getcontenttype(),
        "HTTP_HOST=" + req.gethostname(),
        "SCRIPT_NAME=" + server.getService(),
        "PATH_INFO=" + req.geturi(), // Ici pas sur car il faut tout le path donc a voir 
    };
    int  environSize = sizeof(environnement) / sizeof(environnement[0]);
    char** environ = new char*[environSize + 1]; 

    for (int i = 0; i < environSize; i++) 
    {
        environ[i] = new char[environnement[i].size() + 1];
        strcpy(environ[i], environnement[i].c_str());
    }
    environ[environSize] = NULL;

    return environ;
}


// route pour effectuer le pipe + fork neccessaire pour l'execution 
// si erreur on renvoie juste un string vide car le controle se fait plus haut
std::string playCGI(const std::string path, char** env)
{
    std::string output;
    int pipfd[2];

    if (pipe(pipfd) == -1)
        return "";
    pid_t pid = fork();
    if (pid < 0)
    {
        close(pipfd[0]);
        close(pipfd[1]);
        return "";
    }
    else if (pid == 0) 
        childProcess(path, env, pipfd); // ici des exit avant mais aucune utiliter
    else 
        output = parentProcess(pid, pipfd);

    return output;
}

// process du child du fork avec la recuperation du fichier a ouvrir etc 
// a voir si ici il est possible d'avoir une autre facon juste le nom du fichier au lieu de le decouper ici
// pour eviter les probleme ici
int childProcess(const std::string path, char**env, int *pipfd)
{
    const char *filename;
    close(pipfd[0]);
    dup2(pipfd[1], STDOUT_FILENO); 
    close(pipfd[1]);
    std::string::size_type start = path.find_last_of("/");
    if (start != std::string::npos)
        filename = path.substr(start + 1).c_str();
    else
        filename = path.c_str();
    const char *args[] = {"/usr/bin/perl", filename, NULL};
    if (execve(args[0],(char* const*)args , env) == -1)
        return 1;
    return 0;
}
// le process parent qui va ecrire dans output le retour du script qui dans l'idee 
// s'occupe de creer un body html
std::string parentProcess(pid_t pid, int *pipfd)
{
    std::string output;
    char buffer[128];
    ssize_t bytesRead;
    close(pipfd[1]);
    while ((bytesRead = read(pipfd[0], buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytesRead] = '\0';
        output.append(buffer);
    }
    close(pipfd[0]);
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        return output;
    else 
        return "";
}

std::string executeCGI(const std::string &path, Server server, Request req)
{
    char **env;
    std::string body;
    env = initEnv(req, server);
    body = playCGI(path, env);
    if (env)
        freeEnv(env);
    return body;
}

void freeEnv(char** tab)
{
    int i = 0;
    while (tab[i] != NULL)
    {
        delete[] tab[i];
        i++;
    }
    delete[] tab;
}
std::string ParseUri(std::string uri)
{
    std::string::size_type start = uri.find('?');
    if (start == std::string::npos)
        return "";
    return UtilParsing::convertHexaToString(uri.substr(start + 1));
}
