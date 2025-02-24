
#include "ServerConfig.hpp"
#include "UtilParsing.hpp"

ServerConfig::ServerConfig(const ServerConfig & ref) {
	*this = ref;
}

ServerConfig & ServerConfig::operator=(const ServerConfig &ref)
{
	if (this != &ref) {
		_listenPort = ref._listenPort;
		rootPath = ref.rootPath;
		_clientMaxBodySize = ref._clientMaxBodySize;
		_uploadPath = ref._uploadPath;
		indexFile = ref.indexFile;
		pageErrorPath = ref.pageErrorPath;
		_serverName = ref._serverName;
		methodAccept = ref.methodAccept;
		_locationConfig = ref._locationConfig;
	}
	return *this;
}

std::ostream & operator<<(std::ostream & o, const ServerConfig &ref)
{
	o   << ITAL YELLOW "ServerConfig:" << std::endl
	
		<< "_listenPort: ";
	for (std::vector<std::string>::const_iterator it = ref._listenPort.begin();
		it != ref._listenPort.end(); it++)
		o   << *it << " ";	
	
	o	<< std::endl
		<< "rootPath: " << ref.rootPath << std::endl
		<< "_clientMaxBodySize: " << ref._clientMaxBodySize << std::endl
		<< "_uploadPath: " << ref._uploadPath << std::endl
		<< "indexFile: " << ref.indexFile << std::endl
		<< "methodsAccept: ";
	for (std::vector<std::string>::const_iterator it = ref.methodAccept.begin();
		it != ref.methodAccept.end(); it++)
		o	<< *it << " ";

	o	<< "pageErrorPath: " << (ref.pageErrorPath.empty() ? "empty" : ref.pageErrorPath) << std::endl
		<< "_serverName: ";
	if (ref._serverName.empty() == false){
		for (std::vector<std::string>::const_iterator it = ref._serverName.begin();
			it != ref._serverName.end(); it++)
			o   << *it << " ";
	}

	o	<< "_locationConfig:" << std::endl;
	if (ref._locationConfig.empty() == false) {
		for (std::vector<LocationConfig>::const_iterator it = ref._locationConfig.begin();
			it != ref._locationConfig.end(); it++)
			o << *it << std::endl;
	}
	else
		o	<< "No location specified" << std::endl;
	return o << RESET;
}

void	ServerConfig::displayValueServer()
{
    std::cout   << BOLD BRIGHT_CYAN "SERVERCONFIG:\n" RESET
                << BRIGHT_CYAN "_serverName:		" RESET;
                UtilParsing::displayVector(_serverName);
    std::cout   << BRIGHT_CYAN "_uploadPath:		" RESET CYAN << _uploadPath << std::endl
                << BRIGHT_CYAN "_clientMaxBodySize:	" RESET CYAN <<  _clientMaxBodySize << std::endl
                << BRIGHT_CYAN "_listenPort:		" RESET;
                UtilParsing::displayVector(_listenPort);
                displayAllLocations();
    std::cout << std::endl;
}

void	ServerConfig::displayAllLocations()
{
	for(std::vector<LocationConfig>::iterator it = _locationConfig.begin(); it != _locationConfig.end(); it++)
		it->displayLocation();
}

void	ServerConfig::controlDefaultServerConf()
{
	if (_listenPort.empty())
		throw std::invalid_argument("'listen' must not be empty. Put the keyword (in quotes) followed by its value(s) separated by a space.");
	if (rootPath.empty())
		throw std::invalid_argument("'root' must not be empty. Put the keyword (in quotes) followed by its value(s) separated by a space.");
	if (_clientMaxBodySize.empty())
		throw std::invalid_argument("'client_max_body_size' must not be empty. Put the keyword (in quotes) followed by its value(s) separated by a space.");
	if (_uploadPath.empty())
		throw std::invalid_argument("'upload_path' must not be empty. Put the keyword (in quotes) followed by its value(s) separated by a space.");
	if (indexFile.empty())
		throw std::invalid_argument("'index' must not be empty. Put the keyword (in quotes) followed by its value(s) separated by a space.");
	if (pageErrorPath.empty())
		pageErrorPath = PATH_ERRPAGE;
	if (methodAccept.empty())
		methodAccept = UtilParsing::split(DFLT_METHOD, " ");
	if (_serverName.empty())
		_serverName.push_back(DFLT_HOSTNAME);
}

void	ServerConfig::checkSemiColonServer()
{
    if (_clientMaxBodySize.find(";") != std::string::npos)
        _clientMaxBodySize = UtilParsing::trimSemicolon(_clientMaxBodySize);
    if (_uploadPath.find(";") != std::string::npos)
        _uploadPath =  UtilParsing::trimSemicolon(_uploadPath);
    for (std::vector<std::string>::iterator it = _listenPort.begin(); it != _listenPort.end(); it++)
    {
        if (it->find(";") != std::string::npos)
            *it = UtilParsing::trimSemicolon(*it);
    }
    for (std::vector<std::string>::iterator i = _serverName.begin(); i != _serverName.end(); i++)
    {
        if (i->find(";") != std::string::npos)
            *i = UtilParsing::trimSemicolon(*i);
    }
}



