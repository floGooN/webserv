#ifndef CGI_HPP
# define CGI_HPP

#include "Client.hpp"
#include <cstring>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

bool checkExtensionCGI(const std::string &path);

bool moveToDirectoryScript(const std::string &directory);

char** initEnv(Request &req);
std::string playCgi(const std::string &path, const Request &req, char **env);
void childProcessCgi(char**env, int *pipe_in, int *pipe_out, const Request &req);
void childProcessCgiPy(char**env, int *pipe_in, int *pipe_out, const Request &req);
void closePipe(int *pipe_in, int *pipe_out);
std::string parentProcessCgi(const Request &req, pid_t pid, int *pipe_in, int *pipe_out);
std::string ParseUri(std::string uri);
void freeEnv(char** tab);
int controlContentBodyReq(const Request &req);
std::string buildScriptName(const Request &req);
std::string processAutoIndex(std::string path);
std::string buildBodyAutoIndex(DIR *dir, std::string path);
std::string buildHeaderAutoIndex(std::string body);
std::string processCGI(const Client &client);
std::string executeCGI(const Client &client);
std::string parentProcessCgi(const Request &req, pid_t pid, int *pipe_in, int *pipe_out);
std::string createBody(int *pipe_out);
std::string ParseUri(std::string uri);

#endif
