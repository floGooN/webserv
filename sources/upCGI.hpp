#ifndef UPCGI_HPP
# define UPCGI_HPP

#include "Client.hpp"
#include <cstring>
#include <stdlib.h>

std::string processCGI(const Client &client);
std::string extractDirectory(const std::string &path);
bool moveToDirectoryScript(const std::string &directory);
std::string playCgi(const std::string &path, Request req, char **env);
void childProcessCgi(char**env, int *pipe_in, int *pipe_out, Request req);
void childProcessCgiPy(char**env, int *pipe_in, int *pipe_out, Request req);
std::string parentProcessCgi(Request req, pid_t pid, int *pipe_in, int *pipe_out);
std::string createBody(int *pipe_out);
std::string ParseUri(std::string uri);
void freeEnv(char** tab);

//------------------PARTIE TEST EN DUR-------------------------



#endif
