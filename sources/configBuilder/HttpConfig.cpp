

/*============================================================================*/
							/*### HEADER FILES ###*/
/*============================================================================*/

#include "HttpConfig.hpp"
#include "UtilParsing.hpp"


/*============================================================================*/
			/*### CONSTRUCTORS - DESTRUCTOR _ OVERLOAD OPERATORS ###*/
/*============================================================================*/

HttpConfig::HttpConfig()
{
	default_type.clear();
	keepalive_timeout.clear();
	worker_connexion.clear();
	serversConfig.clear();
}
/*----------------------------------------------------------------------------*/

HttpConfig::HttpConfig(const HttpConfig & ref) {
	*this = ref;
}
/*----------------------------------------------------------------------------*/

HttpConfig & HttpConfig::operator=(const HttpConfig & ref)
{
	if (this != &ref)
	{
		default_type = ref.default_type;
		keepalive_timeout = ref.keepalive_timeout;
		worker_connexion = ref.worker_connexion;
		serversConfig = ref.serversConfig;
	}
	return *this;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream & o, const HttpConfig &ref)
{
	o   << ITAL CYAN "HttpConfig :" << std::endl
		<< "default_type: " << ref.default_type << std::endl
		<< "keepalive_timeout: " << ref.keepalive_timeout << std::endl
		<< "worker_connexion: " << ref.worker_connexion << std::endl
		<< "serversConfig:" << std::endl;
	if (ref.serversConfig.empty() == false) {
		for (std::vector<ServerConfig>::const_iterator it = ref.serversConfig.begin();
			it != ref.serversConfig.end(); it++)
			o << *it << std::endl;
	}
	else
		o	<< "no server config" << std::endl;
	return o << RESET;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
							/*### METHODS ###*/
/*============================================================================*/

void HttpConfig::controlDefaultHttpConf()
{
	if (default_type.empty())
		default_type = DFLT_MIME_TYPE;
	if (keepalive_timeout.empty())
		keepalive_timeout = DFLT_TIMEOUT;
	if (worker_connexion.empty())
		worker_connexion = DFLT_WORKCONNEX;

	std::vector<ServerConfig>::iterator it = serversConfig.begin();
	
	if (it == serversConfig.end())
		throw std::runtime_error("No server config");
	while (it != serversConfig.end())
	{
		it->controlDefaultServerConf();
		if (it->locationConfig.empty() == false) {
			std::vector<LocationConfig>::iterator itLoc = it->locationConfig.begin();
			while (itLoc != it->locationConfig.end()) {
				itLoc->controlDefaultLocationConf();
				itLoc++;
			}
		}
		it++;
	}
}
/*----------------------------------------------------------------------------*/

void HttpConfig::checkSemiColonAllValues()
{
	if (default_type.find(";") != std::string::npos)
		default_type = UtilParsing::trimSemicolon(default_type);
	if (keepalive_timeout.find(";") != std::string::npos)
		keepalive_timeout = UtilParsing::trimSemicolon(keepalive_timeout);
	if (worker_connexion.find(";") != std::string::npos)
		worker_connexion = UtilParsing::trimSemicolon(worker_connexion);
	for (size_t i = 0; i <= serversConfig.size() - 1; i++)
	{
		serversConfig[i].checkSemiColonServer();
		for (std::vector<LocationConfig>::iterator y = serversConfig[i].locationConfig.begin(); y != serversConfig[i].locationConfig.end(); y++)
			y->checkSemiColonLocation();
	}
}
/*----------------------------------------------------------------------------*/
