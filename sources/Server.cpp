


/*============================================================================*/
							/*### HEADER FILES ###*/
/*============================================================================*/
#include "webserv.hpp"
#include "Server.hpp"
#include "UtilParsing.hpp"

/*============================================================================*/
			/*### CONSTRUCTORS - DESTRUCTOR _ OVERLOAD OPERATORS ###*/
/*============================================================================*/

Server::Server(ServerConfig & config, const std::string &service)
{
	_config = config;
	_service = service;
	UtilParsing::convertVectorToSet(_nameList, _config.serverName);
	setLocation();
	_clientList.clear();
	try {
		_maxBodySize = UtilParsing::convertBodySize(config.clientMaxBodySize);
	}
	catch(const std::exception& e) {
		std::cout	<< "Invalid format on max body size: " << config.clientMaxBodySize << std::endl
					<< "Max body size will be set by default (1 MB)"
					<< std::endl;
		_maxBodySize = 1000000;
	}
	setErrorPath();
}
/*----------------------------------------------------------------------------*/

Server::Server(const Server &ref)
{
	_config = ref._config;
	_service = ref._service;
	UtilParsing::deepCopieSet(_nameList, ref._nameList);
	UtilParsing::deepCopieSet(_location, ref._location);
	_clientList = ref._clientList;
	_maxBodySize = ref._maxBodySize;
}
/*----------------------------------------------------------------------------*/

Server::~Server() 
{	}
/*----------------------------------------------------------------------------*/

Server  & Server::operator=(const Server &ref)
{
	_service = ref._service;
	UtilParsing::deepCopieSet(_nameList, ref._nameList);
	UtilParsing::deepCopieSet(_location, ref._location);

	return *this;
}
/*----------------------------------------------------------------------------*/

bool	Server::operator<(const Server &ref) const {
	return _service < ref._service;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream & o, const Server &ref)
{
	o	<< BOLD "SERVER:" RESET << std::endl
		<< "_nameList: ";
	for (std::set<std::string>::const_iterator it = ref.getNameList().begin();
		it != ref.getNameList().end(); it++)
			o	<< "[" << *it << "] ";

	o	<< "\n_service: " << ref.getService() ;

	return o << RESET;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

size_t Server::getMaxBodySize() const {
	return _maxBodySize;
}
/*----------------------------------------------------------------------------*/

const std::set<std::string>	& Server::getNameList() const {
	return const_cast<std::set<std::string> &>(_nameList);
}
/*----------------------------------------------------------------------------*/

const ServerConfig	&Server::getConfig() const {
	return _config;
}
/*----------------------------------------------------------------------------*/

const std::string & Server::getService() const {
	return _service;
}
/*----------------------------------------------------------------------------*/

const std::set<LocationConfig>	& Server::getLocation() const {
	return _location;
}
/*----------------------------------------------------------------------------*/

std::map<int, Client>	&Server::getClientList() const {
	return const_cast<std::map<int, Client>& >(_clientList);
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/
void	Server::setLocation()
{
	for (std::vector<LocationConfig>::const_iterator it = _config.locationConfig.begin();
		it != _config.locationConfig.end(); it++) {
			try {
				_location.insert(*it);
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
			
		}
}
/*----------------------------------------------------------------------------*/

void	Server::setErrorPath()
{
	try {
		_errorPathList.insert(std::make_pair(0, "_config._errorPath"));
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << "\n" __FILE__ " at : " << __LINE__;
	}
}
/*----------------------------------------------------------------------------*/
