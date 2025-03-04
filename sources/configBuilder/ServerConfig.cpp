



#include "ServerConfig.hpp"
#include "UtilParsing.hpp"

ServerConfig::ServerConfig()
{
	listenPort.clear();
	rootPath.clear();
	clientMaxBodySize.clear();
	uploadPath.clear();
	methodAccept.clear();
	pageErrorPath.clear();
	serverName.clear();
	locationConfig.clear();
}

ServerConfig::ServerConfig(const ServerConfig & ref) {
	*this = ref;
}

ServerConfig & ServerConfig::operator=(const ServerConfig &ref)
{
	if (this != &ref) {
		rootPath = ref.rootPath;
		clientMaxBodySize = ref.clientMaxBodySize;
		uploadPath = ref.uploadPath;
		indexFile = ref.indexFile;
		pageErrorPath = ref.pageErrorPath;
		listenPort = ref.listenPort;
		serverName = ref.serverName;
		methodAccept = ref.methodAccept;
		locationConfig = ref.locationConfig;
	}
	return *this;
}

std::ostream & operator<<(std::ostream & o, const ServerConfig &ref)
{
	o   << ITAL YELLOW "ServerConfig:" << std::endl

		<< "listenPort: ";
	for (std::vector<std::string>::const_iterator it = ref.listenPort.begin();
		it != ref.listenPort.end(); it++)
		o   << *it << " ";	

	o	<< std::endl
		<< "rootPath: " << ref.rootPath << std::endl
		<< "clientMaxBodySize: " << ref.clientMaxBodySize << std::endl
		<< "uploadPath: " << ref.uploadPath << std::endl
		<< "indexFile: " << ref.indexFile << std::endl
		<< "methodsAccept: ";
	for (std::vector<std::string>::const_iterator it = ref.methodAccept.begin();
		it != ref.methodAccept.end(); it++)
		o	<< *it << " ";

	o	<< "\npageErrorPath: " << ref.pageErrorPath << std::endl
		<< "serverName: ";
	if (ref.serverName.empty() == false){
		for (std::vector<std::string>::const_iterator it = ref.serverName.begin();
			it != ref.serverName.end(); it++)
			o   << *it << " ";
	}

	o	<< "\nlocationConfig:" << std::endl;
	if (ref.locationConfig.empty() == false) {
		for (std::vector<LocationConfig>::const_iterator it = ref.locationConfig.begin();
			it != ref.locationConfig.end(); it++)
			o << *it << std::endl;
	}
	else
		o	<< "No location specified" << std::endl;
	return o << RESET;
}

void	ServerConfig::controlDefaultServerConf()
{
	if (listenPort.empty())
		throw std::invalid_argument("'listen' must not be empty. Put the keyword (in quotes) followed by its value(s) separated by a space.");
	if (rootPath.empty())
		throw std::invalid_argument("'root' must not be empty. Put the keyword (in quotes) followed by its value(s) separated by a space.");
	if (clientMaxBodySize.empty())
		throw std::invalid_argument("'client_max_body_size' must not be empty. Put the keyword (in quotes) followed by its value(s) separated by a space.");
	if (uploadPath.empty())
		throw std::invalid_argument("'upload_path' must not be empty. Put the keyword (in quotes) followed by its value(s) separated by a space.");
	if (indexFile.empty())
		indexFile.clear();
	if (pageErrorPath.empty())
		pageErrorPath = PATH_ERRPAGE;
	if (methodAccept.empty())
		methodAccept = UtilParsing::split(DFLT_METHOD, " ");
	if (serverName.empty())
		serverName.push_back(DFLT_HOSTNAME);
}

void	ServerConfig::checkSemiColonServer()
{
    if (clientMaxBodySize.find(";") != std::string::npos)
        clientMaxBodySize = UtilParsing::trimSemicolon(clientMaxBodySize);
    if (uploadPath.find(";") != std::string::npos)
        uploadPath =  UtilParsing::trimSemicolon(uploadPath);
    for (std::vector<std::string>::iterator it = listenPort.begin(); it != listenPort.end(); it++)
    {
        if (it->find(";") != std::string::npos)
            *it = UtilParsing::trimSemicolon(*it);
    }
    for (std::vector<std::string>::iterator i = serverName.begin(); i != serverName.end(); i++)
    {
        if (i->find(";") != std::string::npos)
            *i = UtilParsing::trimSemicolon(*i);
    }
}
