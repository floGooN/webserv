#include "Server.hpp"
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include "CGI.hpp"
#include "UtilParsing.hpp"

// int controlAutoIndex(Server server, std::string path)
// {
//     const std::set<std::string>& locations = server.getLocationSet();
    
//     if (locations.find(path))
//         return 0;
//     return -1; 
// }

// Normalement plus besoin de check car le check est realiser avant :

// int controlAutoIndex(const Server& server, const std::string& path)
// {
//     const std::set<t_location>& locations = server.getLocationSet();
    
//     for (std::set<t_location>::const_iterator it = locations.begin(); it != locations.end(); ++it)
//     {
//         if (path.find((*it).path) == 0)
//             return 0;
//     }
//     return -1; 
// }

// std::string processAutoIndex(Server *server, std::string path) // path ici c'est le nom du dossier donc en theori uri ?
// {
//     DIR *dir;
//     std::string body;
//     std::string header;
//     std::string response;

//     if ((dir = opendir(path.c_str())) == NULL)
//         throw ErrorHandler(ERR_404, "Not Found");
//     body = buildBodyAutoIndex(dir, path);
//     header = buildHeaderAutoIndex(body);
//     response += header + body;
//     closedir(dir);
//     return response;
// }


std::string processAutoIndex(std::string path) // path ici c'est le nom du dossier donc en theori uri ?
{
    DIR *dir;
    std::string body;
    std::string header;
    std::string response;

    if ((dir = opendir(path.c_str())) == NULL)
        throw ErrorHandler(ERR_404, "Not Found");
    body = buildBodyAutoIndex(dir, path);
    header = buildHeaderAutoIndex(body);
    response += header + body;
    closedir(dir);
    return response;
}

// fct qui a pour but de creer la page a renvoyer finale
std::string buildBodyAutoIndex(DIR *dir, std::string path)
{
    struct dirent *files;
    std::string body;

  
    body = "<html>\n<head>\n<title>Telechargement Valide</title>\n</head>\n<body>\n<h1>List Files:</h1>\n";
    while ((files = readdir(dir)) != NULL)
    {
        if (std::string(files->d_name).compare(".") == 0 || std::string(files->d_name).compare("..") == 0)
            continue;
        body += "\n<a href=\"" + path + "/" + std::string(files->d_name) + "\">" + std::string(files->d_name) +  "</a>"; // ici jcrois il faut mettre d'autre balise pour link
    }
    body += "\n</body></html>";
    return body;
}

std::string buildHeaderAutoIndex(std::string body)
{
    std::string header;
    header += "Content-Type: text/html; charset=utf-8";
    header += "\nContent-Length:" + UtilParsing::intToString(body.length());
    header += "\nHTTP/1.1 200 OK\n\n"; // je mets deux \n a voir si il y a vraiment une norme car jai rien vu dessus

    return header;
}


// _____________________ PARTIE TEST ______________________

// dans cette version il manque le controle autoindex
std::string processAutoIndexTEST(std::string path)
{
    DIR *dir;
    std::string body;
    std::string header;
    std::string response;
    
    if ((dir = opendir(path.c_str())) == NULL)
        throw ErrorHandler(ERR_404, "Not Found");
    body = buildBodyAutoIndex(dir, path);
    header = buildHeaderAutoIndex(body);
    response += header + body;
    closedir(dir);
    return response;
}



// EXPLICATIONS REDIRECTIONS

/*

Il semble que les redirections commence par : rewrite  
et si c'est une redirection permanente alors on rajoute le flag : permanent
a la fin. 
exemple : 

rewrite ^/ancienne_page$ http://www.newdomain.com/nouvelle_page permanent;

les redirections en https ne fonctionne pas exactement pareil car elle sont dans le 
bloc server et vont avec un 'return' en mot clef en premier suivit du code 
exemple : 
 return 301 https://domain.fr

apres verification il semblerait que les redirections dites normale peuvent elles aussi
commencer par 'return'.

*/
