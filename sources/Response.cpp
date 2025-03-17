










/*============================================================================*/
						/*### HEADERS & STATIC FIELD ###*/
/*============================================================================*/

#include "Response.hpp"
#include "Client.hpp"
#include "UtilParsing.hpp"

/*============================================================================*/
				/*### CONSTRUCTORS - DESTRUCTOR - OVERLOAD OP ###*/
/*============================================================================*/

Response::Response()
{
	completeUri.clear();
	finalMessage.clear();
}
/*----------------------------------------------------------------------------*/

Response::Response(const Response &ref) {
	*this = ref;
}
/*----------------------------------------------------------------------------*/

Response::~Response()
{	}
/*----------------------------------------------------------------------------*/

Response &Response::operator=(const Response &ref)
{
	if (this != &ref)
	{
		completeUri = ref.completeUri;
		finalMessage = ref.finalMessage;
	}
	return *this;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream &o, const Response &)
{
	// o	<< ""
	return o;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

/*----------------------------------------------------------------------------*/

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
void	Response::buildHeader()
{
	std::string length = UtilParsing::intToString(static_cast<int>( response.finalMessage.length() ));
	std::string final =	PROTOCOL_VERION " 200 OK\r\n" \
						"Date: TODAY\r\n" \
						"Server: MyMinimalWebServer/0.5\r\n" \
						"Content-Type: text/html; charset=UTF-8\r\n" \
						"Content-Length: " + length + "\r\n" \
						"Connection: keep-alive\r\n\r\n";
	response.finalMessage.insert(0, final);
	// std::cout	<< BRIGHT_YELLOW "Client::buildHeader()\n"
	// 			<< response.finalMessage << RESET <<std::endl;

}
/*----------------------------------------------------------------------------*/

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

void	Response::getRequest(const Request &)
{
	std::cout << BRIGHT_GREEN "GET QUERY" << RESET << std::endl;
}
/*----------------------------------------------------------------------------*/

void	Response::postRequest(const Request &request)
{
	std::cout	<< BRIGHT_YELLOW "POST QUERY" << std::endl
				<< "bodySize: " << request.getbody().size() << std::endl
				<< "request:\n" RESET << const_cast<Request&>(request) << std::endl;
}
/*----------------------------------------------------------------------------*/

void	Response::deleteRequest(const Request &)
{
	std::cout << BRIGHT_CYAN "DELETE QUERY" << RESET << std::endl;
}
/*----------------------------------------------------------------------------*/

void	Response::clearResponse()
{
	completeUri.clear();
	finalMessage.clear();
}
/*----------------------------------------------------------------------------*/

void Response::initMimeMap()
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
