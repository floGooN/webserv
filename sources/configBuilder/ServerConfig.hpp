


#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "webserv.hpp"
# include "LocationConfig.hpp"

class ServerConfig 
{
public:
	ServerConfig() {};
	ServerConfig(const ServerConfig &);

	ServerConfig & operator=(const ServerConfig &ref);
	
	std::vector<std::string>	_serverName;
	std::vector<std::string>	_listenPort;
	std::vector<LocationConfig>	_locationConfig;
	std::string _errorPath;
	
	std::string	_clientMaxBodySize;
	std::string	_uploadPath;
	int			_serverIndex;

	void displayValueServer();
	void displayAllLocations();
	void controlDefaultServerConf();
	void checkSemiColonServer();
};
std::ostream & operator<<(std::ostream & o, const ServerConfig &);
#endif
