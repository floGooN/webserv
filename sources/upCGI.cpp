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
#include "upCGI.hpp"
#include "ErrorHandler.hpp"



std::string processCGI(const Client &client)
{
    std::string res;
    try
    {
        // if (UtilParsing::fileExits(path) != 0)
        //     throw ErrorHandler(ERR_404, "Not Found");
        // if (access(path.c_str(), X_OK) != 0)
        //     throw ErrorHandler(ERR_404, "Not Found");
        if (checkExtensionCGI(client.request.completeUri, client.clientServer) != 0)
            throw ErrorHandler(ERR_502, "Bad Gateway"); 
        if (moveToDirectoryScript(extractDirectory(client.request.completeUri)) != 0)
            throw ErrorHandler(ERR_500, "Internal server error");
        res = executeCGI(client.request.completeUri, client.clientServer, client.request); // ici dans ce body header + body
        if (res.empty())
            throw ErrorHandler(ERR_502, "Bad Gateway");
    }
    catch (const ErrorHandler& e)
    {
        std::cerr << e.errorNumber << ":" << e.errorNumber << std::endl;
    }
    return res;
}