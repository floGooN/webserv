#ifndef HTTPCONFIG_HPP
# define HTTPCONFIG_HPP


#include "webserv.hpp"
#include "ServerConfig.hpp"

class HttpConfig 
{
	public:
		HttpConfig();
		HttpConfig(const HttpConfig &);
		HttpConfig & operator=(const HttpConfig &);

		std::string					default_type;
		std::string					keepalive_timeout;
		std::string					worker_connexion;
		std::vector<ServerConfig>	serversConfig;

		void	controlDefaultHttpConf();
		void	checkSemiColonAllValues();
};

std::ostream & operator<<(std::ostream & o, const HttpConfig &);

#endif
