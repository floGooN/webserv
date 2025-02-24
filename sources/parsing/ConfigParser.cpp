


#include "ConfigParser.hpp"

HttpConfig ConfigParser::parse(const std::string& filepath) 
{
    HttpConfig httpConfig;
    std::ifstream file(filepath.c_str());
    std::ifstream filetest(filepath.c_str());
    if (!file.is_open()) 
        throw std::runtime_error("Could not open config file");
    controlStructFile(filetest);
    std::string line;
    while (std::getline(file, line)) 
    {
        if (line.empty() || line[0] == '#')
			continue;
        if (line.find("http") != std::string::npos) 
            parseHttpBlock(file, httpConfig);
    }
    httpConfig.controlDefaultHttpConf();
    if (httpConfig._serversConfig.empty() == false)
    {
        std::vector<ServerConfig>::iterator itServerConf = httpConfig._serversConfig.begin();
        while (itServerConf != httpConfig._serversConfig.end())
        {
            itServerConf->controlDefaultServerConf();
            if (itServerConf->_locationConfig.empty() == false)
            {
                std::vector<LocationConfig>::iterator itLoconfig = itServerConf->_locationConfig.begin();
                while (itLoconfig != itServerConf->_locationConfig.end())
                {
                    itLoconfig->controlDefaultLocationConf();
                    itLoconfig++;
                }
            }
            itServerConf++;
        }
    }
    return httpConfig;
}

void ConfigParser::parseHttpBlock(std::ifstream& file, HttpConfig& httpConfig) 
{
    std::string line;
    while (std::getline(file, line)) 
    {
        if (line.find("include") != std::string::npos)
            httpConfig._include = (UtilParsing::splitSpecialDeleteKey(line, std::string(" ")));
        if (line.find("default_type") != std::string::npos)
            httpConfig._default_type = UtilParsing::recoverValue(line, "default_type");
        if (line.find("keepalive_timeout") != std::string::npos)
            httpConfig._keepalive_timeout = UtilParsing::recoverValue(line, "keepalive_timeout");
        if (line.find("worker_connexion") != std::string::npos)
            httpConfig._worker_connexion = UtilParsing::recoverValue(line, "worker_connexion");
        if (line.find("server") != std::string::npos) 
        {
            ServerConfig serverConfig;
            parseServerBlock(file, serverConfig);
            httpConfig._serversConfig.push_back(serverConfig);
        }
    }
}

void ConfigParser::parseServerBlock(std::ifstream& file, ServerConfig& serverConfig) 
{
    std::string line;
    while (std::getline(file, line)) 
    {
        if (line.find("listen") != std::string::npos) 
            serverConfig._listenPort = (UtilParsing::splitSpecialDeleteKey(UtilParsing::trim(line), std::string(" ")));
        else if (line.find("root") != std::string::npos)
            serverConfig.rootPath = UtilParsing::recoverValue(line, "root");
        else if (line.find("client_max_body_size") != std::string::npos) 
            serverConfig._clientMaxBodySize = UtilParsing::recoverValue(line, "client_max_body_size");
        else if (line.find("upload_path") != std::string::npos) 
            serverConfig._uploadPath = UtilParsing::recoverValue(line, "upload_path");
        else if (line.find("index") != std::string::npos)
            serverConfig.indexFile = UtilParsing::recoverValue(line, "index");
        else if (line.find("methods_accept") != std::string::npos)
            serverConfig.methodAccept = UtilParsing::splitSpecialDeleteKey(line, std::string(" "));
        else if (line.find("error_path") != std::string::npos) 
            serverConfig.pageErrorPath = UtilParsing::recoverValue(line, "error_path");
        else if (line.find("server_name") != std::string::npos)
            serverConfig._serverName = (UtilParsing::splitSpecialDeleteKey(line, std::string(" ")));
        else if (line.find("location") != std::string::npos) 
        {
            LocationConfig locationConfig;
            locationConfig._path = UtilParsing::recoverValue(line, "location");
            parseLocationBlock(file, locationConfig);
            serverConfig._locationConfig.push_back(locationConfig);
        }
        else if (line.find("}") != std::string::npos)
        {
            if (serverConfig.pageErrorPath.empty())
                serverConfig.pageErrorPath = std::string("none");
            break;
        }
    }
}

void ConfigParser::parseLocationBlock(std::ifstream& file, LocationConfig& locationConfig) 
{
    std::string line;
    while (std::getline(file, line)) 
    {
        if (line.find("autoindex") != std::string::npos)
            locationConfig.autoindex = UtilParsing::recoverValue(line, "autoindex");
        else if (line.find("root") != std::string::npos) 
            locationConfig._root = UtilParsing::recoverValue(line, "root");
        else if (line.find("index") != std::string::npos) 
            locationConfig._index = UtilParsing::recoverValue(line, "index");
        else if (line.find("cgi_path") != std::string::npos)
            locationConfig._cgipath = UtilParsing::recoverValue(line, "cgi_path");
        else if (line.find("methods_accept") != std::string::npos)
            locationConfig._methods = UtilParsing::splitSpecialDeleteKey(line, std::string(" "));
        else if (line.find("}") != std::string::npos)
        {

            if (locationConfig._cgipath.empty())
                locationConfig._cgipath = std::string("none");
            break;
        }  
    }
}

void ConfigParser::controlStructFile(std::ifstream& file)
{
    std::string line;
    size_t nb_part = 0;
    size_t nb_bracket = 0;
    while (std::getline(file, line))  
    {
        if (line.find("http") != std::string::npos || (line.find("server") != std::string::npos && line.find("server_name") == std::string::npos)
                || line.find("location") != std::string::npos)
            nb_part++;
        if (line.find("{") != std::string::npos || line.find("}") != std::string::npos)
            nb_bracket++;
    }
    if (nb_bracket != (nb_part * 2))
    {
        std::cout << nb_bracket << std::endl;
        std::cout << nb_part << std::endl;
        throw std::invalid_argument("Whether it is 'http', 'server' or 'location' each part must be enclosed by brackets.");
    }
}
