#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include "HttpConfig.hpp"
#include "webserv.hpp"
#include "UtilParsing.hpp"
#include <sstream>
#include <fstream>
#include <cstring>
#include <unistd.h>


class ConfigParser 
{
public:
    HttpConfig parse(const std::string& filepath);
    void controlStructFile(std::ifstream& file);

private:
    void parseHttpBlock(std::ifstream& file, HttpConfig& httpConfig);
    void parseServerBlock(std::ifstream& file, ServerConfig& serverConfig);
    void parseLocationBlock(std::ifstream& file, LocationConfig& locationConfig);  
};


#endif
