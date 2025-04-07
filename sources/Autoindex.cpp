#include "Server.hpp"
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include "CGI.hpp"
#include "Utils.hpp"


std::string processAutoIndex(const Client &client)
{
    DIR *dir;
    std::string body;
    std::string header;

    const t_location *current = Utils::findLocation(client.clientServer->getLocationSet(), client.request.getHeader().uri);
	std::string path = current->root + client.request.getHeader().uri;
    if ((dir = opendir(path.c_str())) == NULL)
        throw ErrorHandler(ERR_403, ("[" + client.request.getHeader().uri + "] is forbidden"));
    body = buildBodyAutoIndex(dir, path);
    closedir(dir);
    return body;
}

std::string buildBodyAutoIndex(DIR *dir, std::string path)
{
    struct dirent *files;
    std::string body;

    body = "<html>\n<head>\n<style>body {display: flex;justify-content: center;flex-direction: column;align-items: center;height: 100vh;font-family: 'Poppins', sans-serif;font-weight: 300;font-size: 15px;line-height: 1.7;color: #c4c3ca;background-color: #1f2029;overflow-x: hidden;margin: 0;} a { font-size: 30px;}</style><title>List</title>\n</head>\n<body>\n<h1>List Files:</h1>\n";
    while ((files = readdir(dir)) != NULL)
    {
        if (std::string(files->d_name).compare(".") == 0 || std::string(files->d_name).compare("..") == 0)
            continue;
        body += "\n<a href=\"" + path + "/" + std::string(files->d_name) + "\">" + std::string(files->d_name) +  "</a>"; // ici jcrois il faut mettre d'autre balise pour link
    }
    body += "\n</body></html>";
    return body;
}

