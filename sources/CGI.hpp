#ifndef CGI_HPP
# define CGI_HPP

#include "Client.hpp"
#include <cstring>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

bool checkExtensionCGI(const std::string &path);
std::string extractDirectory(Client client ,const std::string &path);
bool moveToDirectoryScript(const std::string &directory);
// char** initEnv(Request req, Server server);
char** initEnv(Request req);
std::string playCgi(const std::string &path, Request req, char **env);
void childProcessCgi(char**env, int *pipe_in, int *pipe_out, Request req);
void childProcessCgiPy(char**env, int *pipe_in, int *pipe_out, Request req);
void closePipe(int *pipe_in, int *pipe_out);
std::string parentProcessCgi(Request req, pid_t pid, int *pipe_in, int *pipe_out);
std::string ParseUri(std::string uri);
void freeEnv(char** tab);
int controlContentBodyReq(Request req);
std::string buildScriptName(Request req);
std::string processAutoIndex(std::string path);
std::string buildBodyAutoIndex(DIR *dir, std::string path);
std::string buildHeaderAutoIndex(std::string body);
std::string executeCGI(const std::string &path, Server &server, Request req);
std::string processCGI(const Client &client);





std::string parentProcessCgi(Request req, pid_t pid, int *pipe_in, int *pipe_out);
std::string createBody(int *pipe_out);
std::string ParseUri(std::string uri);



//------------------PARTIE TEST EN DUR-------------------------

// void processCGITEST(const std::string &path);
// bool checkExtensionCGITEST(const std::string &path);
// std::string executeCGITEST(const std::string &path);
// char** initEnvTEST();

#endif
