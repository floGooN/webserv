#ifndef CGI_HPP
# define CGI_HPP

#include "Client.hpp"
#include <cstring>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

/*============================================================================*/
						/*### CGI FONCTIONS ###*/
/*============================================================================*/
bool checkExtensionCGI(const std::string &path);
bool moveToDirectoryScript(const std::string &directory);
std::string playCgi(const Request &req, char **env);
void childProcessCgi(char**env, int *pipe_in, int *pipe_out, const Request &req);
void closePipe(int *pipe_in, int *pipe_out);
std::string parentProcessCgi(const Request &req, pid_t pid, int *pipe_in, int *pipe_out);
std::string processCGI(const Client &client) throw (ErrorHandler);
std::string executeCGI(const Client &client);
std::string parentProcessCgi(const Request &req, pid_t pid, int *pipe_in, int *pipe_out);
std::string createBody(int *pipe_out);


/*============================================================================*/
/*### AUTOINDEX FONCTIONS ###*/
/*============================================================================*/
std::string buildBodyAutoIndex(DIR *dir, std::string path);
std::string processAutoIndex(const Client &client);

/*============================================================================*/
/*### UTILS FONCTIONS ###*/
/*============================================================================*/
std::string ParseUri(std::string uri);
void freeEnv(char** tab);
int controlContentBodyReq(const Request &req);
std::string ParseUri(std::string uri);
std::string buildPathInfo(const std::string &path);
std::string buildScriptName(const Request &req);
char** initEnv(Request &req);

#endif
