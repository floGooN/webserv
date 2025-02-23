


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
	_service = _service;
	UtilParsing::convertVectorToSet(_nameList, _config._serverName);
	if (_nameList.empty() == true)
		_nameList.insert("localhost");
	setLocationPath();
	_clientList.clear();
	try {
		_maxBodySize = UtilParsing::convertBodySize(config._clientMaxBodySize);
	}
	catch(const std::exception& e) {
		std::cout	<< "Invalid format on max body size: " << config._clientMaxBodySize << std::endl
					<< "Max body size is set by default (1 MB)"
					<< std::endl;
		_maxBodySize = 1000000;
	}	
}
/*----------------------------------------------------------------------------*/

Server::Server(const Server &ref)
{
	_config = ref._config;
	_service = ref._service;
	UtilParsing::deepCopieSet(_nameList, ref._nameList);
	UtilParsing::deepCopieSet(_locationPath, ref._locationPath);
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
	UtilParsing::deepCopieSet(_locationPath, ref._locationPath);

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
	
	o	<< "\n_locationPath: ";
	for (std::set<std::string>::const_iterator it = ref.getLocationPath().begin();
		it != ref.getLocationPath().end(); it++)
			o	<< "[" << *it << "] ";

	// o	<< "\n_clientList: ";
	// for (std::set<Client>::const_iterator it = ref.getClientList().begin();
	// 	it != ref.getClientList().end(); it++)
	// 		o	<< *it ;
	return o << RESET << std::endl;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

const size_t Server::getBodySize() const {
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

const std::set<std::string>	& Server::getLocationPath() const {
	return const_cast<std::set<std::string>&>(_locationPath);
}
/*----------------------------------------------------------------------------*/

std::map<int, Client>	&Server::getClientList() const {
	return const_cast<std::map<int, Client>& >(_clientList);
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/
void	Server::setLocationPath()
{
	for (std::vector<LocationConfig>::const_iterator it = _config._locationConfig.begin();
		it != _config._locationConfig.end(); it++) {
			_locationPath.insert(it->_path);
		}
}
/*----------------------------------------------------------------------------*/


/*============================================================================*/
							/*### EXCEPTIONS ###*/
/*============================================================================*/
