




/*============================================================================*/
						/*### HEADERS & STATIC FIELD ###*/
/*============================================================================*/

#include "Client.hpp"
#include "Server.hpp"
#include "UtilParsing.hpp"
#include "ErrorHandler.hpp"

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

void	Client::buildResponse()
{
	// verifier que le requete ne pointe pas sur un directory
	// si c'est le cas, faire pointer le requete sur l'index de la location ou sur l'index serveur ou renvoyer bad request
	std::cout	<< "Client::buildResponse()\n" << std::endl
				<< "CompleteURI: " << response.completeUri << std::endl;
	if (UtilParsing::isDirectory(response.completeUri) == true)
	{
		const LocationConfig *current = UtilParsing::findLocationConfig(clientServer->getLocation(), request.geturi());
		if ( current && ! current->index.empty()) {
			response.completeUri.append(current->index);
		}
		else if ( ! clientServer->getConfig().indexFile.empty()) {
			response.completeUri.append(clientServer->getConfig().indexFile);
		}
		else {
			//si l'autoindex est autorise envoyer l'arborescence ?
			throw ErrorHandler(*this, ERR_404, ("[" + request.geturi() + "] request didn't succeed"));
		}
	}

	std::cout	<< YELLOW "completeUri: [" << response.completeUri << "]" RESET << std::endl;

	UtilParsing::checkAccessRessource(response.completeUri.c_str(), R_OK);

	// if (request.gettype().compare("GET") == 0)
	// 	getRequest(request);
	// else if (request.gettype().compare("POST") == 0)
	// 	postRequest(request);
	// else if (request.gettype().compare("DELETE") == 0)
	// 	deleteRequest(request);
	// else
	// 	throw std::runtime_error("Error 405 method not allowed in " __FILE__);

	response.finalMessage = UtilParsing::readFile(response.completeUri);

	// set la page une fois qu'on est sur le l'avoir
	buildHeader();
	
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/
/*
	HTTP/1.1 404 Not Found
	Date: Tue, 04 Mar 2025 12:34:56 GMT
	Server: MyMinimalWebServer/1.0
	Content-Type: text/html; charset=UTF-8
	Content-Length: 123
	Connection: close
*/

/*
	1️⃣ Réponse 200 - OK (Page HTML servie avec succès)
	👉 Cette réponse est envoyée lorsque la requête a été traitée avec succès et que le contenu demandé est retourné.

	2️⃣ Réponse 201 - Created (Fichier créé sur le serveur)
	👉 Utilisé lorsqu'une ressource a été créée avec succès (ex: upload d'un fichier via POST).

	3️⃣ Réponse 204 - No Content (Aucune donnée retournée)
	👉 Utilisé quand une action a réussi mais qu'il n'y a rien à retourner (ex: suppression d'un fichier avec DELETE).

	4️⃣ Réponse 301 - Moved Permanently (Redirection permanente)
	👉 Utilisé lorsqu'une ressource a été déplacée de façon permanente vers une autre URL.
*/
void	Client::buildHeader()
{
	std::string length = UtilParsing::intToString(static_cast<int>( response.finalMessage.length() ));
	std::string final =	PROTOCOL_VERION " 200 OK\r\n" \
						"Date: TODAY\r\n" \
						"Server: MyMinimalWebServer/0.5\r\n" \
						"Content-Type: text/html; charset=UTF-8\r\n" \
						"Content-Length: " + length + "\r\n" \
						"Connection: close\r\n\r\n";
	response.finalMessage.insert(0, final);
	std::cout	<< BRIGHT_YELLOW "Client::buildHeader()\n"
				<< response.finalMessage << RESET <<std::endl;

}
/*----------------------------------------------------------------------------*/

/*	* build the complete uri and return the location associated with the path requested by client
*/
const LocationConfig *	Client::buildCompleteUri()
{
	std::string				RootPart;
	const LocationConfig	*result = UtilParsing::findLocationConfig(clientServer->getLocation(), request.geturi());
	
	if (result && ! result->root.empty())
		RootPart = result->root;
	else
		RootPart = clientServer->getConfig().rootPath;

	try {
		response.completeUri = RootPart + request.geturi();
		return result;
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
	// concatener le root et le new uri
	// retourne un pointeur sur une location qui match ou null
	std::cout	<< BRIGHT_RED "checkRequestValidity()\n"
				<< "brut uri: " << request.geturi() << std::endl;
				// << this->request << std::endl;
	const LocationConfig *currentLocation = buildCompleteUri();
	
	// doit verifier que la methode est acceptee dans le scop (si location est null recherche dans les methods server)
	checkMethodAuthor(currentLocation);

	std::cout	<< "complete uriPath: [" << response.completeUri << "]\n";

	// doit verifier si le paths pointe sur une donee accessible
	UtilParsing::checkAccessRessource(response.completeUri.c_str(), R_OK);

// 	* security check :
// 		-> uri doesn't include ".." ()
// 		-> only allow Chars (RFC 3986 section 2.2)
// 		-> don't care about uri longest

}
/*----------------------------------------------------------------------------*/

void	Client::clearData()
{
	request.clearRequest();
	response.clearResponse();
	clientServer = NULL;
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
