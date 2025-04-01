


/*============================================================================*/
							/*### HEADER FILES ###*/
/*============================================================================*/

#include "Server.hpp"
#include "ServerConfig.hpp"
#include "UtilParsing.hpp"

/*============================================================================*/
			/*### CONSTRUCTORS - DESTRUCTOR _ OVERLOAD OPERATORS ###*/
/*============================================================================*/

Server::Server(const ServerConfig & config, const std::string &service)
  : _params(setParams(config, service)),
	_locationSet(setLocations(config.locationConfig))
{	}
/*----------------------------------------------------------------------------*/

Server::Server(const Server &ref)
  : _params(ref._params),
	_locationSet(ref._locationSet)
{	}
/*----------------------------------------------------------------------------*/

Server::~Server() 
{	}
/*----------------------------------------------------------------------------*/

Server  & Server::operator=(const Server &) {
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
		for (std::set<t_location>::const_iterator it = ref.getLocationSet().begin(); \
			it != ref.getLocationSet().end(); it++) {
			o << *it << std::endl;
		}
	}

	return o << RESET;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream &o, const t_params &ref)
{
    o   << ITAL CYAN "params:\n"
        << "Max body size: " << ref.maxBodySize << std::endl
        << "Service: " << ref.service << std::endl
        << "Name list: ";
    
	std::set<std::string>::iterator it = ref.nameList.begin();
    for (; it != ref.nameList.end(); it++) {
        o   << *it << " ";
    }

	o	<< "\nmethodsAccept:\n";
	for (std::set<e_methods>::iterator itt = ref.methods.begin(); itt != ref.methods.end(); \
		itt++)
		o << *itt << " ";

    return o << RESET << std::endl;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream &o, const t_location &ref)
{
    o   << ITAL BRIGHT_CYAN "Location:\n"
		<< "autoindex: " << ref.autoindex << std::endl
		<< "cgipath: " << ref.cgipath << std::endl
		<< "index: " << ref.index << std::endl
		<< "path: " << ref.path << std::endl
		<< "root: " << ref.root << std::endl
		<< "methods: ";
		
		std::set<e_methods>::iterator it = ref.methods.begin();
		for (; it != ref.methods.end(); it++) {
			o << "[" << *it << "] ";
		}

	return o << RESET << std::endl;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

const t_params	&Server::getParams() const {
	return _params;
}
/*----------------------------------------------------------------------------*/

const std::set<t_location>	&Server::getLocationSet() const {
	return _locationSet;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

t_params Server::setParams(const ServerConfig &config, const std::string &service)
{
	t_params params;

	setBodySize(config.clientMaxBodySize, params);
	setService(service, params);
	setRootPath(config.rootPath, params);
	setErrorPath(config.pageErrorPath, params);
	setIndexFile(config.indexFile, params);
	setMethod(config.methodAccept, params);
	setErrorPath(config.pageErrorPath, params);
	setRootPath(config.rootPath, params);
	setNameList(config.serverName, params);
	setUploadPath(config.uploadPath, params);
	
	return params;
}
/*----------------------------------------------------------------------------*/

void	Server::setBodySize(const std::string &bodySize, t_params &params) {
	params.maxBodySize = UtilParsing::convertBodySize(bodySize);
}
/*----------------------------------------------------------------------------*/

void	Server::setService(const std::string &service, t_params &params) {
	params.service = service;
}
/*----------------------------------------------------------------------------*/

void	Server::setRootPath(const std::string &rootPath, t_params &params) {
	params.rootPath = rootPath;
}
/*----------------------------------------------------------------------------*/

void	Server::setErrorPath(const std::string &errPath, t_params &params) {
	params.errorPath = errPath;
}
/*----------------------------------------------------------------------------*/

void	Server::setIndexFile(const std::string &indexFile, t_params &params) {
	params.indexFile = indexFile;
}
/*----------------------------------------------------------------------------*/

void	Server::setUploadPath(const std::string &uploadPath, t_params &params) {
	params.uploadPath = uploadPath;
}
/*----------------------------------------------------------------------------*/

void	Server::setMethod(const std::vector<std::string> &conf, t_params &params)
{
	std::vector<std::string>::const_iterator it = conf.begin();

	params.methods.clear();
	while (it != conf.end())
	{
		if (it->compare("GET") == 0)
			params.methods.insert(GET);
		else if (it->compare("POST") == 0)
			params.methods.insert(POST);
		else if (it->compare("DELETE") == 0)
			params.methods.insert(DELETE);
		else
			throw std::invalid_argument(*it + " is not a valid method in this server (methods accepted -> GET POST DELETE)");
		
		it++;
	}
}
/*----------------------------------------------------------------------------*/

void	Server::setNameList(const std::vector<std::string> &names, t_params &params) {
	UtilParsing::convertVectorToSet(params.nameList, names);
}
/*----------------------------------------------------------------------------*/

std::set<t_location> Server::setLocations(const std::vector<LocationConfig> &config)
{
	t_location				newLoc;
	std::set<t_location>	result;
	std::vector<LocationConfig>::const_iterator it = config.begin();

	while (it != config.end())
	{
		newLoc.clear();
		setAutoindex(it->autoindex, newLoc);
		setCgiPath(it->cgipath, newLoc);
		setIndex(it->index, newLoc);
		setMethods(it->methods, newLoc);
		setPath(it->path, newLoc);
		setRoot(it->root, newLoc);
		setRedir(it->redirect, newLoc);
		result.insert(newLoc);
		it++;
	}

	return result;
}
/*----------------------------------------------------------------------------*/

void	Server::setAutoindex(const std::string &conf, t_location &loc)
{
	if (conf.empty() || conf.compare("off") == 0)
		loc.autoindex = false;
	else if (conf.compare("on") == 0)
		loc.autoindex = true;
	else
		throw std::invalid_argument("[" + conf + "] is not a valid parameter: (autoindex -> on/off)");
}
/*----------------------------------------------------------------------------*/

void	Server::setCgiPath(const std::string &conf, t_location &loc) {
	loc.cgipath = conf;
}
/*----------------------------------------------------------------------------*/

void	Server::setIndex(const std::string &conf, t_location &loc) {
	loc.index = conf;
}
/*----------------------------------------------------------------------------*/

void	Server::setMethods(const std::vector<std::string> &conf, t_location &loc)
{
	std::vector<std::string>::const_iterator it = conf.begin();

	while (it != conf.end())
	{
		if (it->compare("GET") == 0)
			loc.methods.insert(GET);
		else if (it->compare("POST") == 0)
			loc.methods.insert(POST);
		else if (it->compare("DELETE") == 0)
			loc.methods.insert(DELETE);
		else
			throw std::invalid_argument(*it + " is not a valid method in this server (methods -> GET POST DELETE)");
		it++;
	}
}

/*----------------------------------------------------------------------------*/
void	Server::setRedir(const std::vector<std::string> &conf, t_location &loc)
{
	std::vector<std::string>::const_iterator it = conf.begin();
	while (it != conf.end())
	{
		loc.redirect.push_back(*it);
		it++;
	}
}

/*----------------------------------------------------------------------------*/

void	Server::setPath(const std::string &conf, t_location &loc) {
	loc.path = conf;
}
/*----------------------------------------------------------------------------*/

void	Server::setRoot(const std::string &conf, t_location &loc) {
	loc.root = conf;
}
/*----------------------------------------------------------------------------*/
