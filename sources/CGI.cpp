#include "Client.hpp"
#include <cstring>
#include "CGI.hpp"
#include "Utils.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include "Server.hpp"
#include "Client.hpp"
#include "Request.hpp"
#include "RequestStructure.hpp"
#include <cstring>
#include "ErrorHandler.hpp"


std::string processCGI(const Client &client)
{
    std::string res;
    try
    {
        const t_location *current = Utils::findLocation(client.clientServer->getLocationSet(), client.request.getHeader().uri);
        if (!current)
            throw ErrorHandler(ERR_404, "Not Found\n");
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        if (moveToDirectoryScript(current->root) != true)
            throw ErrorHandler(ERR_500, "Internal server error\n");
        res = executeCGI(client);
        if (res.empty())
            throw ErrorHandler(ERR_502, "Bad Gateway\n");
        if (moveToDirectoryScript(std::string(cwd)) != true)
            throw ErrorHandler(ERR_500);
    }
    catch (const ErrorHandler& e)
    {
        std::cerr << e.errorNumber << ":" << e.errorNumber << std::endl;
    }
    return res;
}
/*----------------------------------------------------------------------------*/

bool checkExtensionCGI(const std::string &path)
{
   std::string cgi_path = "support.pl";
   std::string cgi_path_other = "support.py";

    if (Utils::recoverExtension(path) == Utils::recoverExtension(cgi_path))
        return true;
    else if (Utils::recoverExtension(path) == Utils::recoverExtension(cgi_path_other))
        return true;
    return false;
}
/*----------------------------------------------------------------------------*/

bool moveToDirectoryScript(const std::string &directory)
{
    if (chdir(directory.c_str()) != 0)
        return false;
    return true;
}
/*----------------------------------------------------------------------------*/

char** initEnv(const Request &req)
{
    std::string environnement[] = 
      {
        std::string("REQUEST_METHOD=") + Utils::emethodsTypeToString(req.getHeader().requestType),
        std::string("QUERY_STRING=") + ((req.getHeader().requestType == GET) ? ParseUri(req.getArgs())  : " "),
        std::string("CONTENT_TYPE=") + Utils::econtentTypeToString(req.getbody().contentType),
        std::string("CONTENT_LENGTH=") + ((req.getbody().contentLength == (size_t)0) ? std::string("100") : Utils::intToString(req.getbody().contentLength)),
        std::string("HTTP_HOST=") + req.getHeader().hostName,
        buildScriptName(req),
        std::string("PATH_INFO=") + buildPathInfo(req.getHeader().uri),
    };
    int  environSize = sizeof(environnement) / sizeof(environnement[0]);
    char** envCGI = new char*[environSize + 1]; 
    if (envCGI == NULL)
        return NULL;
    for (int i = 0; i < environSize; i++) 
    {
        envCGI[i] = new char[environnement[i].size() + 1];
        if (!envCGI)
        {
            freeEnv(envCGI);
            return NULL;
        }
        strcpy(envCGI[i], environnement[i].c_str());
    }
    envCGI[environSize] = NULL;

    return envCGI;
}

/*----------------------------------------------------------------------------*/

std::string buildPathInfo(const std::string &path)
{
    std::string cgi_path = "support.pl";
    std::string cgi_path_other = "support.py";

    if (Utils::recoverExtension(path) == Utils::recoverExtension(cgi_path))
        return DFLT_PATH_PERL;
    return DFT_PATH_PY; 
}

/*----------------------------------------------------------------------------*/


std::string buildScriptName(const Request &req)
{
    std::string::size_type start = req.getHeader().uri.find_last_of('/');
    std::string::size_type end = req.getHeader().uri.find('?');
    if (end == std::string::npos)
    {
        if (start == std::string::npos)
            return req.getHeader().uri;
        else
        {
            std::string result = req.getHeader().uri.substr(start + 1);
            return result;
        }
    }
    else
    {
        std::string result = req.getHeader().uri.substr(start + 1, end);
        return result;
    }
}

/*----------------------------------------------------------------------------*/


std::string playCgi(const Request &req, char **env) 
{
    int pipe_in[2];
    int pipe_out[2];
    pid_t pid;
    std::string newBody;

    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) 
        return "";
    pid = fork();
    if (pid == -1) 
    {
        closePipe(pipe_in, pipe_out);
        return "";
    }
    if (pid == 0)
        childProcessCgi(env, pipe_in, pipe_out, req);
    else 
        return parentProcessCgi(req, pid, pipe_in, pipe_out);

    return "";
}

/*----------------------------------------------------------------------------*/

void closePipe(int *pipe_in, int *pipe_out)
{
    close(pipe_in[0]); 
    close(pipe_in[1]);
    close(pipe_out[0]); 
    close(pipe_out[1]);
}

/*----------------------------------------------------------------------------*/

std::string executeCGI(const Client &client)
{
    char **env;
    std::string body("");
    if (controlContentBodyReq(client.request) == -1)
        return body;
    env = initEnv(client.request);
    if (!env)
        return body;
    body = playCgi(client.request, env);
    if (env)
        freeEnv(env);
    return body;
}

/*----------------------------------------------------------------------------*/

int controlContentBodyReq(const Request &req)
{
    if (req.getHeader().requestType == POST)
        if (req.getbody().body.empty())
            return -1;
    return 0;
}

/*----------------------------------------------------------------------------*/

void childProcessCgi(char**env, int *pipe_in, int *pipe_out, const Request &req)
{
    close(pipe_in[1]);
    close(pipe_out[0]);
    dup2(pipe_in[0], STDIN_FILENO);
    close(pipe_in[0]);
    dup2(pipe_out[1], STDOUT_FILENO);
    close(pipe_out[1]);
    std::string script = buildScriptName(req);
    std::string road = buildPathInfo(script);
    const char *args[] = {road.c_str(), script.c_str(), NULL};
    execve(args[0], (char *const *)args, env);
    if (env)
        freeEnv(env);
    _exit(1);
}

/*----------------------------------------------------------------------------*/


std::string parentProcessCgi(const Request &req, pid_t pid, int *pipe_in, int *pipe_out)
{
    std::string newBody;

    close(pipe_in[0]);
    close(pipe_out[1]);
    ssize_t result = write(pipe_in[1], req.getbody().body.c_str(), req.getbody().contentLength);
    if (result < 0)
        return newBody;
    close(pipe_in[1]);
    newBody = createBody(pipe_out);
    close(pipe_out[0]);
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        return "";
    return newBody;
}

/*----------------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------------*/

std::string ParseUri(std::string uri)
{
    std::string::size_type start = uri.find('?');
    if (start == std::string::npos)
        return "";
    return Utils::convertHexaToString(uri.substr(start + 1));
}

/*----------------------------------------------------------------------------*/

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
