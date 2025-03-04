#ifndef CGI_HPP
# define CGI_HPP

#include "Client.hpp"
#include <cstring>
#include <stdlib.h>

void processCGI(const std::string &path, Server server, Request req);
bool checkExtensionCGI(const std::string &path, Server server);
std::string extractDirectory(const std::string &path);
bool moveToDirectoryScript(const std::string &directory);
char** initEnv(Request req, Server server);
std::string playCGI(const std::string path, char** env);
int childProcess(const std::string path, char**env, int *pipfd);
std::string parentProcess(pid_t pid, int *pipfd);
std::string executeCGI(const std::string &path, Server server, Request req);
void freeEnv(char** tab);

//------------------PARTIE TEST EN DUR-------------------------

void processCGITEST(const std::string &path);
bool checkExtensionCGITEST(const std::string &path);
std::string executeCGITEST(const std::string &path);
char** initEnvTEST();

#endif
