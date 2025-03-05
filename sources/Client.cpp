




/*============================================================================*/
						/*### HEADERS & STATIC FIELD ###*/
/*============================================================================*/

#include "Client.hpp"
#include "Server.hpp"
#include <cstring>
std::map<std::string, std::string> Client::_mimeMap;

/*============================================================================*/
				/*### CONSTRUCTORS - DESTRUCTOR - OVERLOAD OP ###*/
/*============================================================================*/

Client::Client(const Request &req)
{
	request = req;
	clientServer = NULL;
	initMimeMap();
}
/*----------------------------------------------------------------------------*/

Client::Client(const Client &ref) {
	*this = ref;
}
/*----------------------------------------------------------------------------*/

Client::~Client()
{	}
/*----------------------------------------------------------------------------*/

Client &Client::operator=(const Client &ref)
{
	if (this != &ref)
	{
		request = Request(ref.request);
		clientServer = ref.clientServer;
	}
	return *this;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream &o, const Client &ref)
{
	o   << "CLIENT:" << std::endl
		<< *ref.clientServer << std::endl;

	return o;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

/*	* extract path in URI ommitted arguments
*/
std::string	Client::extractPath(const std::string &uri) const
{
	try {
		size_t idx = uri.find_first_of("?");
		if (idx == uri.npos)
			return uri.substr(0, uri.size());
		else
			return uri.substr(0, idx);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		throw std::runtime_error("500 Internal error in " __FILE__);
	}
}
/*----------------------------------------------------------------------------*/

/*	* build the complete uri and return the location associated with the path requested by client
*/
const LocationConfig *	Client::buildCompleteUri(std::string &QueryPart) const
{
	std::set<LocationConfig>::iterator itLocation = clientServer->getLocation().begin();
	while (itLocation != clientServer->getLocation().end())
	{
		if (itLocation->path.compare(QueryPart) == 0)
			break ;
		itLocation++;
	}

	std::string	RootPart;
	if (itLocation != clientServer->getLocation().end() && ! itLocation->root.empty())
		RootPart = itLocation->root;
	else
		RootPart = clientServer->getConfig().rootPath;

	try {
		QueryPart = RootPart + QueryPart;
		if (itLocation == clientServer->getLocation().end())
			return NULL;
		return &(*itLocation) ;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		throw std::runtime_error("internal error buildCompleteUri() in " __FILE__);
	}
}
/*----------------------------------------------------------------------------*/

void	Client::checkMethodAuthor(const LocationConfig *current) const
{
	std::vector<std::string>::const_iterator itEnd;
	std::vector<std::string>::const_iterator itStart;

	if (current != NULL && ! current->methods.empty()) {
		itStart = current->methods.begin();
		itEnd = current->methods.end();
	}
	else {
		itStart = clientServer->getConfig().methodAccept.begin();
		itEnd = clientServer->getConfig().methodAccept.end();
	}

	while (itStart != itEnd)
	{
		if (itStart->compare(request.gettype()) == 0)
			break;
		itStart++;
	}
	if (itStart == itEnd)
		throw std::runtime_error("Error 405 Method Not Allowed checkMethodAuthor() in " __FILE__);
}
/*----------------------------------------------------------------------------*/

/*	*
	* 
*/
void	Client::checkRequestValidity()
{
	// ne pas parser les arguments dans l'uri
	//
	// doit verifier si les paths sont valides
	
	// extrait uniquement le chemin de la requete :
	std::string	URI_Query = extractPath(request.geturi());
	
	// concatener le root et le new uri
	// retourne un pointeur sur une location qui match ou null
	const LocationConfig *currentLocation = buildCompleteUri(URI_Query);
	
	// doit verifier que la methode est acceptee dans le scop (location - serveur)
	checkMethodAuthor(currentLocation);


	std::cout	<< "checkRequestValidity()\n"
				<< this->request << std::endl
				<< "newURI: [" << URI_Query << "]" << std::endl;
}
/*----------------------------------------------------------------------------*/

void	Client::clearData()
{
	request.clearRequest();
	response.clearResponse();
}
/*----------------------------------------------------------------------------*/

void Client::initMimeMap()
{
	if (!_mimeMap.empty())
		return;

	_mimeMap.insert(std::make_pair(".aac", "audio/acc"));
	_mimeMap.insert(std::make_pair(".abw", "application/x-abiword"));
	_mimeMap.insert(std::make_pair(".apng", "image/apng"));
	_mimeMap.insert(std::make_pair(".arc", "application/x-freearc"));
	_mimeMap.insert(std::make_pair(".avif", "image/avif"));
	_mimeMap.insert(std::make_pair(".avi", "video/x-msvideo"));
	_mimeMap.insert(std::make_pair(".csh", "application/x-csh"));
	_mimeMap.insert(std::make_pair(".css", "text/css"));
	_mimeMap.insert(std::make_pair(".csv", "text/csv"));
	_mimeMap.insert(std::make_pair(".gif", "image/gif"));
	_mimeMap.insert(std::make_pair(".html", "text/html"));
	_mimeMap.insert(std::make_pair(".htm", "text/html"));
	_mimeMap.insert(std::make_pair(".ico", "image/vnd.microsoft.icon"));
	_mimeMap.insert(std::make_pair(".jpeg", "image/jpeg"));
	_mimeMap.insert(std::make_pair(".jpg", "image/jpeg"));
	_mimeMap.insert(std::make_pair(".js", "text/javascript"));
	_mimeMap.insert(std::make_pair(".json", "application/json"));
	_mimeMap.insert(std::make_pair(".jsonld", "application/ld+json"));
	_mimeMap.insert(std::make_pair(".mjs", "text/javascript"));
	_mimeMap.insert(std::make_pair(".mp3", "audio/mpeg"));
	_mimeMap.insert(std::make_pair(".mp4", "video/mp4"));
	_mimeMap.insert(std::make_pair(".png", "image/png"));
	_mimeMap.insert(std::make_pair(".pdf", "application/pdf"));
	_mimeMap.insert(std::make_pair(".php", "application/x-httpd-php"));
	_mimeMap.insert(std::make_pair(".sh", "application/x-sh"));
	_mimeMap.insert(std::make_pair(".svg", "image/svg+xml"));
	_mimeMap.insert(std::make_pair(".tar", "application/x-tar"));
	_mimeMap.insert(std::make_pair(".txt", "text/plain"));
    _mimeMap.insert(std::make_pair(".webp", "image/webp"));
    _mimeMap.insert(std::make_pair(".xhtml", "application/xhtml+xml"));
    _mimeMap.insert(std::make_pair(".xml", "application/xml"));
    _mimeMap.insert(std::make_pair(".zip", "application/zip"));
    _mimeMap.insert(std::make_pair(".xul", "application/vnd.mozilla.xul+xml"));
    _mimeMap.insert(std::make_pair(".3gp", "video/3gpp"));
    _mimeMap.insert(std::make_pair(".7z", "application/x-7z-compressed"));
    _mimeMap.insert(std::make_pair(".bin", "application/octet-stream"));
}
/*----------------------------------------------------------------------------*/
