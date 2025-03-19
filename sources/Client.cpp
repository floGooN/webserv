




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
		<< *ref.clientServer << std::endl
		<< ref.request << std::endl
		<< ref.response;

	return o;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/
void	Client::checkRequestValidity() throw (ErrorHandler)
{
	const t_location *currentLocation = buildCompleteUri();

	checkAutorisation(currentLocation);
	
	try {
		UtilParsing::checkAccessRessource(request.completeUri, R_OK);
	}
	catch(const std::exception& e) {
		throw ErrorHandler(ERR_403, e.what());
	}

	checkUriContent();

	validTheUriPath();
}
/*----------------------------------------------------------------------------*/

/*
*/
void	Client::buildResponse() throw (ErrorHandler)
{
	// verifier si la reponse est deja construite

	if (isAutoindex() == true) {
		std::cout << "It's Autoindex\n"; // here play autoindex generator
		throw ErrorHandler(ERR_404, "Autoindex"); // provisoirement
	}
	else
	{
		switch (request.getHeader().requestType)
		{
			case GET:
				response.getQuery(request);
				break;
			case POST:
				response.postQuery(request);
				break;
			case DELETE:
				response.deleteQuery(request);
				break;
			default:
				throw ErrorHandler(ERR_400, "Unknow the request type");
		}
	}
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

/*	* Returns true if auto-indexing is enabled for the requested directory.
	* Generates an ERR_403 error if auto-indexing is not allowed.
*/
bool	Client::isAutoindex() throw (ErrorHandler)
{
	if (UtilParsing::isDirectory(request.completeUri) == false)
		return false;

	const t_location *current = UtilParsing::findLocation(clientServer->getLocationSet(), request.getHeader().uri);

	if (current != NULL && !current->index.empty()) {
		request.completeUri.append(current->index);
		return false;
	}
	else if (current != NULL && current->autoindex == true )
		return true;
	else
		throw ErrorHandler(ERR_403, ("[" + request.getHeader().uri + "] is forbidden"));
}
/*----------------------------------------------------------------------------*/

/*	* build the complete uri and return the location associated with the path requested by client
*/
const t_location * Client::buildCompleteUri()
{
	std::string			rootPart;
	const t_location	*result = UtilParsing::findLocation(clientServer->getLocationSet(), request.getHeader().uri);
	
	if (result && ! result->root.empty() )
		rootPart = result->root;
	else
		rootPart = clientServer->getParams().rootPath;

	if (*request.getHeader().uri.begin() == '/' && *rootPart.end() == '/')
		rootPart.erase(rootPart.end());
	else if (*request.getHeader().uri.begin() != '/' && *rootPart.end() != '/')
		rootPart += "/";

	request.completeUri = rootPart + request.getHeader().uri;

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

/*	* check the characters contained in the uri
		-> only allow Chars define in the RFC 3986 section 2.2
*/
void Client::checkUriContent() const throw (ErrorHandler)
{
	if (request.getHeader().uri.find_first_not_of(HTTP_ALLOW_CHARS) != request.getHeader().uri.npos)
		throw ErrorHandler(ERR_400, "an invalid charater is detected in the uri");
}
/*----------------------------------------------------------------------------*/

/*	* check if the URI does not go outside the authorized path
	*
	* some URI exemple:	
		/cgi-bin/form/script.sh			-> ok		(client is in /form)
		/cgi-bin/form/../script.sh		-> ok 		(client is in /cgi-bin)

		/cgi-bin/form/../../script.sh	-> not ok	(client is in the root directory)
		/../							-> not ok	(client is a directory above root directory)
*/
void Client::validTheUriPath() const throw (ErrorHandler)
{
	std::vector<std::string>					token;
	std::vector<std::string>::const_iterator	it;

	try {
		token = UtilParsing::split(request.getHeader().uri, "/");
		it = token.begin(); 
	}
	catch(const std::exception& e) {
		throw ErrorHandler(ERR_500, e.what());
	}

	while (it != token.end() && it->compare("/") == 0)
		it++;

	int	dirCounter = 0;
	while (it != token.end())
	{
		if (it->compare("..") == 0)
			dirCounter--;
		else if (it->compare(".") == 0)
			;
		else if (it->compare("/") != 0)
			dirCounter++;
		if (dirCounter <= 0)
			throw ErrorHandler(ERR_403, request.getHeader().uri + std::string(" is an invalid uri"));
		it++;
	}
}
/*----------------------------------------------------------------------------*/

void	Client::clearData()
{
	request.clearRequest();
	response.clearResponse();
	clientServer = NULL;
}
/*----------------------------------------------------------------------------*/
