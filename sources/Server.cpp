


/*============================================================================*/
							/*### HEADER FILES ###*/
/*============================================================================*/

#include "Server.hpp"
#include "ServerConfig.hpp"
#include "UtilParsing.hpp"

/*============================================================================*/
			/*### CONSTRUCTORS - DESTRUCTOR _ OVERLOAD OPERATORS ###*/
/*============================================================================*/

Server::Server(ServerConfig & config, const std::string &service)
  : _params(setParams(config, service)),
	_locationSet(setLocation(config.locationConfig)),
	_errorPathList(setErrorPath())
{	}
/*----------------------------------------------------------------------------*/

Server::Server(const Server &ref)
  : _params(ref._params),
	_locationSet(ref._locationSet),
	_errorPathList(ref._errorPathList)
{	}
/*----------------------------------------------------------------------------*/

Server::~Server() 
{	}
/*----------------------------------------------------------------------------*/

Server  & Server::operator=(const Server &ref)
{
	if (this != &ref) {
		_errorPathList = ref._errorPathList;
	}

	return *this;
}
/*----------------------------------------------------------------------------*/

bool	Server::operator<(const Server &ref) const {
	return this->_params.service < ref._params.service;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream & o, const Server &ref)
{
	o	<< BOLD "SERVER:\n"
		<< BOLD "Params:\n" << ref.getParams()
		<< BOLD "Locations:\n";

	if (ref.getLocationSet().empty())
		o << "no specific path\n";
	else
	{
		std::set<s_location>::iterator it = ref.getLocationSet().begin();
		for (; it != ref.getLocationSet().end(); it++) {
			o << *it << std::endl;
		}
	}

	return o << RESET;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

const s_params	Server::getParams() const {
	return _params;
}
/*----------------------------------------------------------------------------*/

const std::set<s_location>	Server::getLocationSet() const {
	return _locationSet;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/
s_params Server::setParams(const ServerConfig &config, const std::string &service)
{
	this->_params.maxBodySize;
	this->_params.nameList;
	this->_params.service;

	config.clientMaxBodySize;
	config.indexFile;
	config.listenPort;
	config.methodAccept;
	config.pageErrorPath;
	config.rootPath;
	config.serverName;
	config.uploadPath;

}
/*----------------------------------------------------------------------------*/

std::set<s_location> Server::setLocations(const std::vector<LocationConfig> &config)
{
	setAutoindex();
	setCgiPath();
	setIndex();
	setMethods();
	setUrl();
	setRootPath();

	config.begin()->autoindex;
	config.begin()->cgipath;
	config.begin()->index;
	config.begin()->methods;
	config.begin()->path;
	config.begin()->root;
}
/*----------------------------------------------------------------------------*/

std::set<std::pair<int, std::string> >	Server::setErrorPath()
{
	try {
		_errorPathList.insert(std::make_pair(0, "_config._errorPath"));
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << "\n" __FILE__ " at : " << __LINE__;
	}
	return _errorPathList;
}
/*----------------------------------------------------------------------------*/
