


#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "webserv.hpp"
# include "LocationConfig.hpp"

class ServerConfig 
{
public:
	ServerConfig();
	ServerConfig(const ServerConfig &);
	ServerConfig & operator=(const ServerConfig &ref);
	
	std::string					rootPath;
	std::string					clientMaxBodySize;
	std::string					uploadPath;
	std::string					indexFile;
	std::string					pageErrorPath;
	std::vector<std::string>	listenPort;
	std::vector<std::string>	serverName;
	std::vector<std::string>	methodAccept;
	std::vector<LocationConfig>	locationConfig;
	
	void controlDefaultServerConf();
	void checkSemiColonServer();
};

std::ostream & operator<<(std::ostream & o, const ServerConfig &);

#endif
