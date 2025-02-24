


#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "webserv.hpp"
# include "LocationConfig.hpp"

class ServerConfig 
{
public:
	ServerConfig() {
		_listenPort.clear();
		rootPath.clear();
		_clientMaxBodySize.clear();
		_uploadPath.clear();
		methodAccept.clear();
		pageErrorPath.clear();
		_serverName.clear();
		_locationConfig.clear();
	}
	ServerConfig(const ServerConfig &);
	ServerConfig & operator=(const ServerConfig &ref);
	
	std::vector<std::string>	_listenPort;
	std::string					rootPath;
	std::string					_clientMaxBodySize;
	std::string					_uploadPath;
	std::vector<std::string>	methodAccept;
	
	std::string					pageErrorPath;
	std::vector<std::string>	_serverName;

	std::vector<LocationConfig>	_locationConfig;

	void displayValueServer();
	void displayAllLocations();
	void controlDefaultServerConf();
	void checkSemiColonServer();
};

std::ostream & operator<<(std::ostream & o, const ServerConfig &);

#endif
