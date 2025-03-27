#include "Client.hpp"
#include <cstring>
#include "CGI.hpp"
#include "UtilParsing.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include "Server.hpp"
#include "Client.hpp"
#include "Request.hpp"
#include "RequestStructure.hpp"
#include <cstring>
#include "CGI.hpp"
#include "ErrorHandler.hpp"


std::string processCGI(const Client &client)
{
    std::string res;
    try
    {

        const t_location *current = UtilParsing::findLocation(client.clientServer->getLocationSet(), client.request.getHeader().uri);
        if (moveToDirectoryScript(current->root) != true)
            throw ErrorHandler(ERR_500, "Internal server error");
        res = executeCGI(client.request.getHeader().uri, *client.clientServer, client.request); // ici dans ce body header + body
        std::cout << "valeur de res :" << res << std::endl;
        if (res.empty())
            throw ErrorHandler(ERR_502, "Bad Gateway");
    }
    catch (const ErrorHandler& e)
    {
        std::cerr << e.errorNumber << ":" << e.errorNumber << std::endl;
    }
    return res;
}

bool checkExtensionCGI(const std::string &path)
{
   std::string cgi_path = "support.pl";
   std::string cgi_path_other = "support.py";

    if (UtilParsing::recoverExtension(path) == UtilParsing::recoverExtension(cgi_path))
        return true;
    else if (UtilParsing::recoverExtension(path) == UtilParsing::recoverExtension(cgi_path_other))
        return true;
    return false;
}

// fonction pour extract le nom du dossier
// std::string extractDirectory(Client client ,const std::string &path)
// {
//     (void) client;
//     std::string directory;
//     directory = path;

//     // size_t posEndDirectory = path.find_last_of("/");
//     // directory = client.clientServer->getParams().rootPath;
//     // directory += path.substr(0, posEndDirectory);
//     // std::cout << "valeur de directory : " << directory << std::endl;
//     return directory;
// }

// fonction qui permet d'aller dans le bon dossier avec execution du processus cgi
bool moveToDirectoryScript(const std::string &directory)
{

    if (chdir(directory.c_str()) != 0)
    {
        return false;
    }
    return true;
}


// foction pour creer env necessaire dans les script CGI et plus particulieerement les GET
char** initEnv(Request req)
{
      std::string environnement[] = 
      {
        std::string("REQUEST_METHOD=") + UtilParsing::emethodsTypeToString(req.getHeader().requestType),
        std::string("QUERY_STRING=") + ((req.getHeader().requestType == GET) ? ParseUri(req.getHeader().uri)  : " "), // si c'est une get je mets rien apres a voir si on met une valeur ou pas
        std::string("CONTENT_TYPE=") + UtilParsing::econtentTypeToString(req.getbody().contentType), // content-type request
        std::string("HTTP_HOST=") + req.getHeader().hostName,
        buildScriptName(req),
        std::string("PATH_INFO=") + req.getHeader().uri, // tout url 
    };
    int  environSize = sizeof(environnement) / sizeof(environnement[0]);
    char** envCGI = new char*[environSize + 1]; 
    for (int i = 0; i < environSize; i++) 
    {
        envCGI[i] = new char[environnement[i].size() + 1];
        strcpy(envCGI[i], environnement[i].c_str());
    }
    envCGI[environSize] = NULL;

    return envCGI;
}
// fonction pour creer la valeur sript name de l'env du script
// std::string buildScriptName(Server &server)
// {
//     std::string scriptName = "SCRIPT_NAME=";
//     for (std::set<std::string>::iterator it = server.getParams().nameList.begin(); it != server.getParams().nameList.end(); ++it) 
//     {
//     if (it != server.getParams().nameList.begin()) 
//     {
//         scriptName += ",";
//     }
//     scriptName += *it;
//     }
//     return scriptName;
// }

// la je dois check si c'est une post ou get car dans uri il va y avoir une difference
// ensuite eliminer tout ce qui est devant le nom
// et supprimer aussi les arguments a voir car je crois avoir deja fais une fct comme ca
std::string buildScriptName(Request req)
{
    std::string::size_type end = req.getHeader().uri.find('?');
    if (end == std::string::npos)
        return req.getHeader().uri;
    else
    {
        std::string result = req.getHeader().uri.substr(0, end);
        // std::cout << result << std::endl;
        return result;
    }
}


std::string playCgi(const std::string &path, Request req, char **env) 
{
    int pipe_in[2];
    int pipe_out[2];
    pid_t pid;
    std::string newBody;

    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) 
        return "Status: 500 pipein :\r\n\r\n";
    pid = fork();
    if (pid == -1) 
    {
        closePipe(pipe_in, pipe_out);
        return "Status: 500\r\n\r\n";
    }
    if (pid == 0)
    {
        if (UtilParsing::recoverExtension(path) == ".pl")
            childProcessCgi(env, pipe_in, pipe_out, req);
        else
            childProcessCgiPy(env, pipe_in, pipe_out, req);
    }
    else 
        return parentProcessCgi(req, pid, pipe_in, pipe_out);

    return "Error";
}


void closePipe(int *pipe_in, int *pipe_out)
{
    close(pipe_in[0]); 
    close(pipe_in[1]);
    close(pipe_out[0]); 
    close(pipe_out[1]);
}

std::string executeCGI(const std::string &path, Server &server, Request req)
{
    (void) server;
    char **env;
    std::string body;
    if (controlContentBodyReq(req) == -1)
        throw ;
    env = initEnv(req);
    body = playCgi(path, req, env);
    // std::cout << "ici le play est jouer" << std::endl;
    if (env)
        freeEnv(env);
    return body;
}

int controlContentBodyReq(Request req)
{
    // std::cout << req.getbody().body << std::endl;

    if (req.getHeader().requestType == POST) // a verifier mais je crois que c'est bon
        if (req.getbody().body.empty())
            return -1;
    return 0;
}

// rajouter le nom du script a appele de facon modulable
void childProcessCgi(char**env, int *pipe_in, int *pipe_out, Request req)
{
    (void) req;
    close(pipe_in[1]);
    close(pipe_out[0]);
    dup2(pipe_in[0], STDIN_FILENO);
    close(pipe_in[0]);
    dup2(pipe_out[1], STDOUT_FILENO);
    close(pipe_out[1]);
    std::string lechemin = "script.pl";
    // char cwd[256];
    // getcwd(cwd, 256);
    // std::cout << "Répertoire courant : " << cwd << std::endl;
    const char *args[] = {"/usr/bin/perl", lechemin.c_str(), NULL}; // utiliser uri ou une var script name sans doute la meme qui est utiliser pour voir l'extension plus haut
    execve(args[0], (char *const *)args, env);
    _exit(1); // ici gestion d'erreur 
}


// rajouter le nom du script a appele de facon modulable
void childProcessCgiPy(char**env, int *pipe_in, int *pipe_out, Request req)
{
    (void) req;
    close(pipe_in[1]);
    close(pipe_out[0]);
    dup2(pipe_in[0], STDIN_FILENO);
    close(pipe_in[0]);
    dup2(pipe_out[1], STDOUT_FILENO);
    close(pipe_out[1]);
    std::string lechemin = "script.py";
    const char *args[] = {"/usr/bin/python3", lechemin.c_str(), NULL}; // utiliser uri ou une var script name sans doute la meme qui est utiliser pour voir l'extension plus haut
    execve(args[0], (char *const *)args, env);
    _exit(1); // ici gestion d'erreur 
}



std::string parentProcessCgi(Request req, pid_t pid, int *pipe_in, int *pipe_out)
{
    std::string newBody;

    close(pipe_in[0]);
    close(pipe_out[1]);
    write(pipe_in[1], req.getbody().body.c_str(), req.getbody().contentLength); // avant check si le body envoyer a un content sinon renvoyer error 204 No content
    close(pipe_in[1]);
    newBody = createBody(pipe_out);
    close(pipe_out[0]);
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        return "Status: 500\r\n\r\n";
    return newBody;
}

std::string createBody(int *pipe_out)
{
    std::string newBody;
    char buffer[4096];
    ssize_t bytes_read;   
    while ((bytes_read = read(pipe_out[0], buffer, sizeof(buffer) - 1)) > 0) 
    {
        buffer[bytes_read] = '\0';
        newBody.append(buffer);
    }
    return newBody;
}

std::string ParseUri(std::string uri) // ici donner args
{
    std::string::size_type start = uri.find('?');
    if (start == std::string::npos)
        return "";
    return UtilParsing::convertHexaToString(uri.substr(start + 1));
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
