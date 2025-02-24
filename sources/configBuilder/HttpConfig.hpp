#ifndef HTTPCONFIG_HPP
# define HTTPCONFIG_HPP


#include "webserv.hpp"
#include "ServerConfig.hpp"

class HttpConfig 
{
	public:
		HttpConfig() {
			_default_type.clear();
			_keepalive_timeout.clear();
			_worker_connexion.clear();
			_include.clear();
			_serversConfig.clear();
		}
		HttpConfig(const HttpConfig &);
		HttpConfig & operator=(const HttpConfig & ref);

		std::string					_default_type;		//octet-stream
		std::string					_keepalive_timeout;	//in secondes
		std::string					_worker_connexion;	//nb total client
		std::vector<std::string>	_include;			//files includes in custer config?
		
		std::vector<ServerConfig>	_serversConfig;		//toutes les config extraits du parsing

		// std::set<std::string>		_serviceList;

		void	displayDefaultHttp();
		void	displayServers();
		void	controlDefaultHttpConf();
		void	checkSemiColonAllValues();

		std::string	getDefaultType();
		std::string	getKeepalive();
		std::string	getWorkerConnexion();
		std::vector<std::string> getIncludes();
};

std::ostream & operator<<(std::ostream & o, const HttpConfig &);

#endif
