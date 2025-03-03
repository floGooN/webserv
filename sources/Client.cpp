




/*============================================================================*/
						/*### HEADERS & STATIC FIELD ###*/
/*============================================================================*/

#include "Client.hpp"
#include <cstring>
std::map<std::string, std::string> Client::_mimeMap;

/*============================================================================*/
				/*### CONSTRUCTORS - DESTRUCTOR - OVERLOAD OP ###*/
/*============================================================================*/

Client::Client(const Request &req)
{
	request = req;
	clientServer = NULL;
	memset(&response, 0, sizeof(response));
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
		request = ref.request;
		clientServer = ref.clientServer;
		response = ref.response;
	}
	return *this;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream &o, const Client &ref)
{
	o   << "CLIENT:" << std::endl
		<< *ref.clientServer << std::endl;
		// << "response : " << ref.response;
	return o;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

void	Client::handlePostRequest()
{
	std::cout	<< BRIGHT_YELLOW "IN POST QUERY\nrequest:" << std::endl
				<< request << RESET << std::endl;

	
}
/*----------------------------------------------------------------------------*/

/*  * Formating header and body for client
	* throw error page
	* manage herself system exception
*/
void	Client::responseFormating()
{
	// std::cout	<< "IN formatResponse():" << std::endl
	// << *this << std::endl
	// << this->request << std::endl;

	// soccuper des POST query
		// TOUT LES PARAMS SONT DANS UN BODY
	if (request.gettype().compare("POST") == 0)
	{
		handlePostRequest();
	}
	else if (request.gettype().compare("GET") == 0)
	{
		std::cout << BRIGHT_GREEN "GET QUERY" << RESET << std::endl;
		return;
	}
	else if (request.gettype().compare("DELETE") == 0)
	{
		std::cout << BRIGHT_CYAN "DELETE QUERY" << RESET << std::endl;
	}
	else
		throw std::runtime_error("error XXX Request not supported");


	// comment se formatte la reponse client ?

	// extraire chemin + nom fichier
	// differencier POST GET DELETE 
	// if POST	-> extraire le content type
	//				.application/x-www-form-urlencoded
	//				.multipart/form-data	
	//				.text/plain	
	//			-> extraire arguments du body

	// if GET	-> extraire les arguments de la query string
	// 

	// concatener le chemins correctement suivant l'url et le serveur qui recoit la requete
	// verifier que toutes les conditions soient bonnes
		// dossier et fichiers existants ?
		// droits des requetes (POST ETC)
		// est ce qu'il y a des cgi
	
	// buildUri(this->request.geturi());

	// formatter header http
	// dans un try catch pour formatter le header une fois le serveur trouve

	// gestion requet favico
	// toujours matter a la racine du site courrant si il en a un 
	// sinon renvoyer le favico par defaut


	// if (request.geturi().find("favicon") != std::string::npos)
	// 	response = "";
}
/*----------------------------------------------------------------------------*/

/*	* doit mettre a jour suivant le fichier du serveur
*/
void Client::initMimeMap() {
	if (_mimeMap.empty() == false)
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

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

/*  * build final path
	* security check :
		-> uri doesn't include ".." ()
		-> only allow Chars (RFC 3986 section 2.2)
		-> don't care about uri longest
*/
void	Client::buildUri(const std::string &uri)
{
#ifdef TEST
	std::cout << "in builduir() URI: [" << uri << "]" << std::endl;
#endif
	if (uri.find("..") != uri.npos)
		throw std::runtime_error("400 bad request \'.. detected\'\n");
	if (uri.find_first_not_of(HTTP_ALLOW_CHARS) != uri.npos) {
		std::cout << "idx: " << uri.find_first_not_of(HTTP_ALLOW_CHARS) << "cha: [" << uri[uri.find_first_not_of(HTTP_ALLOW_CHARS)] << "]" << std::endl;
		throw std::runtime_error("400 bad request \'invalid char\'\n");
	}
	
	std::set<LocationConfig>::iterator itLocation = clientServer->getLocation().begin();

	while (itLocation != clientServer->getLocation().end())
	{
		if (itLocation->path.compare(uri) == 0)
		{
			std::cout	<< *itLocation
						<< "it : [" << itLocation->path << "]" << std::endl;
			break;
		}
		itLocation++;
	}

}
/*----------------------------------------------------------------------------*/


// /*============================================================================*/
// 							/*### EXCEPTIONS ###*/
// /*============================================================================*/

// /*----------------------------------------------------------------------------*/
