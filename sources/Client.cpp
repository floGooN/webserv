




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


/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

/*	* build the complete uri and return the location associated with the path requested by client
*/
const t_location * Client::buildCompleteUri()
{
	std::string			RootPart;
	const t_location	*result = UtilParsing::findLocation(clientServer->getLocationSet(), request.getHeader().uri);
	
	if (result && !result->root.empty() )
		RootPart = result->root;
	else
		RootPart = clientServer->getParams().rootPath;

	response.completeUri = RootPart + request.getHeader().uri;
	return result;
}
/*----------------------------------------------------------------------------*/

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
		ErrorHandler(ERR_405, "Method " + request.getHeader().requestType + std::string(" not allowedin this service"))
}
/*----------------------------------------------------------------------------*/

/*	*
	* 
*/
void	Client::checkRequestValidity() throw (ErrorHandler)
{
	const t_location *currentLocation = buildCompleteUri();
	
#ifdef TEST
	std::cout	<< "complete uriPath: [" << response.completeUri << "]\n";
#endif

	try {
		checkAutorisation(currentLocation);
	}
	catch(const ErrorHandler& e) {
		throw ErrorHandler(ERR_403, e.what());
	}
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

