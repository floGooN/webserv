#include "CGI.hpp"
#include "UtilParsing.hpp"


std::string processRedirect(const Client &client) throw (ErrorHandler)
{
    std::string res;
    const t_location *current = UtilParsing::findLocation(client.clientServer->getLocationSet(), client.request.getHeader().uri);
    if (current == NULL)
        throw ErrorHandler(ERR_404);
    if (controlValuesRedirect(current->redirect) != true)
        throw ErrorHandler(ERR_404); // ici voir l'erreur a mettre
    res = buildBodyRedirect(current->redirect);
    return res;
}


bool controlValuesRedirect(const std::vector<std::string> &vec)
{
    if (vec.size() != 2)
        return false;
    if (*vec.begin() != std::string("301") || *vec.begin() != std::string("302"))
        return false;
    return true;
}


std::string buildBodyRedirect(const std::vector<std::string> &vec)
{
    std::string body;

    body = "<html>\n<head>\n<style>body {display: flex;justify-content: center;flex-direction: column;align-items: center;height: 100vh;font-family: 'Poppins', sans-serif;font-weight: 300;font-size: 15px;line-height: 1.7;color: #c4c3ca;background-color: #1f2029;overflow-x: hidden;margin: 0;} a { font-size: 30px;}</style><title>List</title>\n</head>\n<body>\n<h1>Redirection" + vec[0] + "</h1>\n";
    body += "\n<a href=\"" + vec[1] + "> Redirection </a>";
    body += "\n</body></html>";
    return body;
}
