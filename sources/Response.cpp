










/*============================================================================*/
						/*### HEADERS & STATIC FIELD ###*/
/*============================================================================*/

#include "Response.hpp"

/*============================================================================*/
				/*### CONSTRUCTORS - DESTRUCTOR - OVERLOAD OP ###*/
/*============================================================================*/

Response::Response()
{	}
/*----------------------------------------------------------------------------*/

Response::Response(const Response &ref) {
	*this = ref;
}
/*----------------------------------------------------------------------------*/

Response::~Response()
{	}
/*----------------------------------------------------------------------------*/

Response &Response::operator=(const Response &) {
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

void	Response::buildResponse(const Request &request)
{
	if (request.gettype().compare("GET") == 0)
		getRequest(request);
	else if (request.gettype().compare("POST") == 0)
		postRequest(request);
	else if (request.gettype().compare("DELETE") == 0)
		deleteRequest(request);
	else
		throw std::runtime_error("Error 405 method not allowed in " __FILE__);
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
	
}
/*----------------------------------------------------------------------------*/

// /*  * build final path
// 	* security check :
// 		-> uri doesn't include ".." ()
// 		-> only allow Chars (RFC 3986 section 2.2)
// 		-> don't care about uri longest
// */
// void	Client::buildUri(const std::string &uri)
// {
// #ifdef TEST
// 	std::cout << "in builduir() URI: [" << uri << "]" << std::endl;
// #endif
// 	if (uri.find("..") != uri.npos)
// 		throw std::runtime_error("400 bad request \'.. detected\'\n");
// 	if (uri.find_first_not_of(HTTP_ALLOW_CHARS) != uri.npos) {
// 		std::cout << "idx: " << uri.find_first_not_of(HTTP_ALLOW_CHARS) << "cha: [" << uri[uri.find_first_not_of(HTTP_ALLOW_CHARS)] << "]" << std::endl;
// 		throw std::runtime_error("400 bad request \'invalid char\'\n");
// 	}
	
// 	std::set<LocationConfig>::iterator itLocation = clientServer->getLocation().begin();

// 	while (itLocation != clientServer->getLocation().end())
// 	{
// 		if (itLocation->path.compare(uri) == 0)
// 		{
// 			std::cout	<< *itLocation
// 						<< "it : [" << itLocation->path << "]" << std::endl;
// 			break;
// 		}
// 		itLocation++;
// 	}

// }
// /*----------------------------------------------------------------------------*/


// /*  * Formating header and body for client
// 	* throw error page
// 	* manage herself system exception
// */
// void	Client::responseFormating()
// {
// 	// std::cout	<< "IN formatResponse():" << std::endl
// 	// << *this << std::endl
// 	// << this->request << std::endl;

// 	// soccuper des POST query
// 		// TOUT LES PARAMS SONT DANS UN BODY
// 	if (request.gettype().compare("POST") == 0)
// 	{
// 		handlePostRequest();
// 	}
// 	else if (request.gettype().compare("GET") == 0)
// 	{
// 		std::cout << BRIGHT_GREEN "GET QUERY" << RESET << std::endl;
// 		return;
// 	}
// 	else if (request.gettype().compare("DELETE") == 0)
// 	{
// 		std::cout << BRIGHT_CYAN "DELETE QUERY" << RESET << std::endl;
// 	}
// 	else
// 		throw std::runtime_error("error XXX Request not supported");


// 	// comment se formatte la reponse client ?

// 	// extraire chemin + nom fichier
// 	// differencier POST GET DELETE 
// 	// if POST	-> extraire le content type
// 	//				.application/x-www-form-urlencoded
// 	//				.multipart/form-data	
// 	//				.text/plain	
// 	//			-> extraire arguments du body

// 	// if GET	-> extraire les arguments de la query string
// 	// 

// 	// concatener le chemins correctement suivant l'url et le serveur qui recoit la requete
// 	// verifier que toutes les conditions soient bonnes
// 		// dossier et fichiers existants ?
// 		// droits des requetes (POST ETC)
// 		// est ce qu'il y a des cgi
	
// 	// buildUri(this->request.geturi());

// 	// formatter header http
// 	// dans un try catch pour formatter le header une fois le serveur trouve

// 	// gestion requet favico
// 	// toujours matter a la racine du site courrant si il en a un 
// 	// sinon renvoyer le favico par defaut


// 	// if (request.geturi().find("favicon") != std::string::npos)
// 	// 	response = "";
// }
// /*----------------------------------------------------------------------------*/
