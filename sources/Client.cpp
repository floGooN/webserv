




/*============================================================================*/
						/*### HEADERS & STATIC FIELD ###*/
/*============================================================================*/

#include "Client.hpp"
#include "Server.hpp"
#include "UtilParsing.hpp"
#include "ErrGenerator.hpp"

#include <cstring>

/*============================================================================*/
				/*### CONSTRUCTORS - DESTRUCTOR - OVERLOAD OP ###*/
/*============================================================================*/

Client::Client()
{	}
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
		clientServer = ref.clientServer;
		request = ref.request;
		response = ref.response;
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
void	Client::checkRequestValidity() throw (ErrorHandler)
{
	const t_location *currentLocation = buildCompleteUri();

	try {
		checkAutorisation(currentLocation);
	}
	catch(const ErrorHandler& e) {
		throw ErrorHandler(e.errorNumber, e.errorLog);
	}
	UtilParsing::checkAccessRessource(response.completeUri.c_str(), R_OK);

// 	* security check :
// 		-> uri doesn't include ".." ()
// 		-> only allow Chars (RFC 3986 section 2.2)
// 		-> don't care about uri longest

}
/*----------------------------------------------------------------------------*/

void	Client::buildResponse()
{
	// verifier que le requete ne pointe pas sur un directory
	// si c'est le cas, faire pointer le requete sur l'index de la location ou sur l'index serveur ou renvoyer bad request
	std::cout	<< "Client::buildResponse()\n" << std::endl;

	switch (request.getHeader().requestType)
	{
		case GET:
			response.getRequest(request);
			break;
		case POST:
			response.postRequest(request);
			break;
		case DELETE:
			response.deleteRequest(request);
			break;
		default:
			throw ErrorHandler(ERR_405, "");
	}

	if (UtilParsing::isDirectory(response.completeUri) == true)
	{
		const t_location *current = UtilParsing::findLocation(clientServer->getLocationSet() , request.getHeader().uri);
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

	response.finalMessage = UtilParsing::readFile(response.completeUri);

	// set la page une fois qu'on est sur le l'avoir
	buildHeader();
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

/*	* build the complete uri and return the location associated with the path requested by client
*/
const t_location * Client::buildCompleteUri()
{
	std::string			RootPart;
	const t_location	*result = UtilParsing::findLocation(clientServer->getLocationSet(), request.getHeader().uri);
	
	if (result && ! result->root.empty() )
		RootPart = result->root;
	else
		RootPart = clientServer->getParams().rootPath;

	request.completeUri = RootPart + request.getHeader().uri;

	return result;
}
/*----------------------------------------------------------------------------*/

/*	* check if the method is allowed in the required location
*/
void Client::checkAutorisation(const t_location *current) const throw (ErrorHandler)
{
	std::set<e_methods>::const_iterator itEnd;
	std::set<e_methods>::const_iterator itStart;

	if (current != NULL && ! current->methods.empty()) {
		itStart = current->methods.begin() ;
		itEnd = current->methods.end();
	}
	else {
		itStart = clientServer->getParams().methods.begin();
		itEnd = clientServer->getParams().methods.end();
	}

	while (itStart != itEnd)
	{
		if (*itStart == request.getHeader().requestType)
			break;
		itStart++;
	}
	if (itStart == itEnd)
		ErrorHandler(ERR_405, "Method " + request.getHeader().requestType + \
									std::string(" not allowed in this service"));
}
/*----------------------------------------------------------------------------*/




void	Client::clearData()
{
	request.clearRequest();
	response.clearResponse();
	clientServer = NULL;
}
/*----------------------------------------------------------------------------*/
