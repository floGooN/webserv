


#include "HttpConfig.hpp"
#include "UtilParsing.hpp"

HttpConfig::HttpConfig(const HttpConfig & ref) {
	*this = ref;
}

HttpConfig & HttpConfig::operator=(const HttpConfig & ref)
{
	if (this != &ref)
	{
		_default_type = ref._default_type;
		_keepalive_timeout = ref._keepalive_timeout;
		_worker_connexion = ref._worker_connexion;
		_include = ref._include;
		_serversConfig = ref._serversConfig;
	}
	return *this;
}

std::ostream & operator<<(std::ostream & o, const HttpConfig &ref)
{
	o   << ITAL CYAN "HttpConfig :" << std::endl
		<< "_default_type: " << ref._default_type << std::endl
		<< "_keepalive_timeout: " << ref._keepalive_timeout << std::endl
		<< "_worker_connexion: " << ref._worker_connexion << std::endl
		<< "_includes: ";
	if (ref._include.empty() == false) {
		for (std::vector<std::string>::const_iterator it = ref._include.begin();
			it != ref._include.end(); it++)
			o   << *it << " ";
	}
	else
		o	<< "no include file";
	o   << std::endl 
		<< "_serversConfig:" << std::endl;
	if (ref._serversConfig.empty() == false) {
		for (std::vector<ServerConfig>::const_iterator it = ref._serversConfig.begin();
			it != ref._serversConfig.end(); it++)
			o << *it << std::endl;
	}
	else
		o	<< "no server config" << std::endl;
	return o << RESET;
}

std::vector<std::string> HttpConfig::getIncludes() {
	return (_include);
}

std::string HttpConfig::getDefaultType() {
	return (_default_type);
}

std::string HttpConfig::getKeepalive() {
	return (_keepalive_timeout);
}

std::string HttpConfig::getWorkerConnexion() {
	return (_worker_connexion);
}

void HttpConfig::displayDefaultHttp()
{
	for (std::vector<std::string>::iterator it = _include.begin(); it != _include.end(); it++)
		std::cout << *it << " " << std::endl;
	std::cout <<  getDefaultType() << std::endl;
	std::cout <<  getKeepalive() << std::endl;
	std::cout << getWorkerConnexion() << std::endl; 
}

void HttpConfig::displayServers() {
	for(std::vector<ServerConfig>::iterator it = _serversConfig.begin(); it != _serversConfig.end(); it++)
		it->displayValueServer();
}

void HttpConfig::controlDefaultHttpConf()
{
	if (_default_type.empty())
		_default_type = DFLT_TYPE;
	if (_keepalive_timeout.empty())
		_keepalive_timeout = "65";
	if (_worker_connexion.empty())
		_worker_connexion = "1024";
	for (size_t i = 0; i <= _serversConfig.size() - 1; i++)
	{
		_serversConfig[i].controlDefaultServerConf();
		for (std::vector<LocationConfig>::iterator it = _serversConfig[i]._locationConfig.begin(); it != _serversConfig[i]._locationConfig.end(); it++)
			it->controlDefaultLocationConf();
	}
}

void HttpConfig::checkSemiColonAllValues()
{
	if (_default_type.find(";") != std::string::npos)
		_default_type = UtilParsing::trimSemicolon(_default_type);
	if (_keepalive_timeout.find(";") != std::string::npos)
		_keepalive_timeout = UtilParsing::trimSemicolon(_keepalive_timeout);
	if (_worker_connexion.find(";") != std::string::npos)
		_worker_connexion = UtilParsing::trimSemicolon(_worker_connexion);
	for (std::vector<std::string>::iterator it = _include.begin(); it != _include.end(); it++)
	{
		if (it->find(";") != std::string::npos)
			*it = UtilParsing::trimSemicolon(*it);
	}
	for (size_t i = 0; i <= _serversConfig.size() - 1; i++)
	{
		_serversConfig[i].checkSemiColonServer();
		for (std::vector<LocationConfig>::iterator y = _serversConfig[i]._locationConfig.begin(); y != _serversConfig[i]._locationConfig.end(); y++)
			y->checkSemiColonLocation();
	}
}
