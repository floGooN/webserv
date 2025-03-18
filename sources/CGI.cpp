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


void processCGI(const std::string &path, Server server, Request req)
{
    std::string response;
    try
    {
        if (UtilParsing::fileExits(path) != 0)
            throw ;
        if (access(path.c_str(), X_OK) != 0)
            throw ;
        if (checkExtensionCGI(path, server) != 0)
            throw ;
        if (moveToDirectoryScript(extractDirectory(path)) != 0)
            throw ;
        response = executeCGI(path, server, req); // ici plus tard renvoyer directement dans la value response
        if (response.empty())
            throw ;
    }
    catch ()
    {
         
    }
}
// fonction pour check le type d'extension du script demander pour verifier si le server la supporte
// pareil la recuperer dans le parsing le nom par server  le nom du script cgi pour verifier son extension
bool checkExtensionCGI(const std::string &path, Server server)
{
    (void)server;
   std::string cgi_path = "support.pl";
   std::string cgi_path_other = "support.py";

    if (UtilParsing::recoverExtension(path) == UtilParsing::recoverExtension(cgi_path))
        return true;
    else if (UtilParsing::recoverExtension(path) == UtilParsing::recoverExtension(cgi_path_other))
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


std::string _method,  _params, _contentType, _http, _httpReferer, _remoteAddr, _remotePort, _scriptName, _pathInfo;
char** initEnv(Request req, Server server)
{
      std::string environnement[] = 
      {
        "REQUEST_METHOD=" + req.getHeader().requestType,
        "QUERY_STRING=" + ((req.getHeader().requestType.compare("GET") == 0) ? ParseUri(req.getHeader().uri)  : " "), // si c'est une get je mets rien apres a voir si on met une valeur ou pas
        "CONTENT_TYPE=" + req.getbody().contentType, // content-type request
        "HTTP_HOST=" + req.getHeader().hostName,
        "SCRIPT_NAME=" + server.getParams().service, // ici le nom du script je pense pas que ce soit bon
        "PATH_INFO=" + req.getHeader().uri, // tout url 
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



std::string playCgi(const std::string &path, Request req, char **env) 
{
    int pipe_in[2];
    int pipe_out[2];
    pid_t pid;
    std::string newBody;

    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) 
        return "Status: 500\r\n\r\n";
    pid = fork();
    if (pid == -1) 
    {
        closePipe(pipe_in, pipe_out);
        return "Status: 500\r\n\r\n";
    }
    if (pid == 0)
    {
        if (UtilParsing::recoverExtension(path) == ".pl")
            childProcessCgi(env, pipe_in, pipe_out);
        else
            childProcessCgiPy(env, pipe_in, pipe_out);
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

std::string executeCGI(const std::string &path, Server server, Request req)
{
    char **env;
    std::string body;
    if (controlContentBodyReq(req) == -1)
        throw ;
    env = initEnv(req, server);
    body = playCgi(path, req, env);
    if (env)
        freeEnv(env);
    return body;
}

int controlContentBodyReq(Request req)
{
    if (req.getHeader().requestType == POST) // a verifier mais je crois que c'est bon
        if (req.getbody().body.empty())
            return -1;
    return 0;
}

// rajouter le nom du script a appele de facon modulable
void childProcessCgi(char**env, int *pipe_in, int *pipe_out)
{
    close(pipe_in[1]);
    close(pipe_out[0]);
    dup2(pipe_in[0], STDIN_FILENO);
    close(pipe_in[0]);
    dup2(pipe_out[1], STDOUT_FILENO);
    close(pipe_out[1]);
    const char *args[] = {"/usr/bin/perl", "./cgi-bin/script.pl", NULL}; // utiliser uri ou une var script name sans doute la meme qui est utiliser pour voir l'extension plus haut
    execve(args[0], (char *const *)args, env);
    _exit(1); // ici gestion d'erreur 
}


// rajouter le nom du script a appele de facon modulable
void childProcessCgiPy(char**env, int *pipe_in, int *pipe_out)
{
    close(pipe_in[1]);
    close(pipe_out[0]);
    dup2(pipe_in[0], STDIN_FILENO);
    close(pipe_in[0]);
    dup2(pipe_out[1], STDOUT_FILENO);
    close(pipe_out[1]);
    const char *args[] = {"/usr/bin/python3", "./cgi-bin/script.py", NULL}; // utiliser uri ou une var script name sans doute la meme qui est utiliser pour voir l'extension plus haut
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
