/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fberthou <fberthou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 05:12:52 by fberthou          #+#    #+#             */
/*   Updated: 2025/04/09 13:25:06 by fberthou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
						/*### HEADERS & STATIC FIELD ###*/
/*============================================================================*/

#include "Client.hpp"
#include "Server.hpp"
#include "Utils.hpp"

#include <ctime>

/*============================================================================*/
				/*### CONSTRUCTORS - DESTRUCTOR - OVERLOAD OP ###*/
/*============================================================================*/

Client::Client(const int sockfd) : fdClient(sockfd)
{
	clientServer = NULL;
	time = std::time(NULL);
	totalBytesReceived = 0;
}
/*----------------------------------------------------------------------------*/

Client::Client(const Client &ref) : fdClient(ref.fdClient) {
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
		time = ref.time;
		totalBytesReceived = ref.totalBytesReceived;
	}
	return *this;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream &o, const Client &ref)
{
	o   << "CLIENT:" << std::endl
		<< *ref.clientServer << std::endl
		<< ref.request
		<< ref.response
		<< "Total Bytes Received" << ref.totalBytesReceived;

	return o << std::endl;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

void	Client::checkRequestValidity() throw (ErrorHandler)
{
	const t_location *currentLocation = NULL;
	
	if (request.getHeader().requestType == DELETE)
	{
		request.completeUri = (request.getHeader().uri[0] == '/' ? "./uploads" : "./uploads/") + request.getHeader().uri;
		checkAutorisation(Utils::findLocation(clientServer->getLocationSet(), "/uploads"));
		return;
	}
	else
	{
		currentLocation = buildCompleteUri();
		if (currentLocation && currentLocation->redirect.size() != 0)
			return;
	}

	checkAutorisation(currentLocation);
	
	try {
		Utils::checkAccessRessource(request.completeUri, R_OK);
	}
	catch(const std::exception& e) {
		switch (errno)
		{
			case ENOENT:
			case ELOOP:
				throw ErrorHandler(ERR_404, e.what() + std::string("\n"));
			
			case EACCES:
			case ENAMETOOLONG:
			case ENOTDIR:
				throw ErrorHandler(ERR_403, e.what() + std::string("\n"));
			
			default:
				throw ErrorHandler(ERR_400, e.what() + std::string("\n"));
		}
	}
	checkUriContent();
	validTheUriPath();
}
/*----------------------------------------------------------------------------*/

void	Client::buildResponse() throw (ErrorHandler)
{
	if ( ! response.message.empty() )
		return;

	switch (request.getHeader().requestType)
	{
		case GET:
			response.getQuery(*this);
			break;
		case POST:
			response.postQuery(*this);
			break;
		case DELETE:
			response.deleteQuery(*this);
			break;
		default:
			throw ErrorHandler(ERR_400, "Unknow the request type\n");
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
	if (request.getHeader().uri.find("upload") != std::string::npos || \
		Utils::isDirectory(request.completeUri) == false)
		return false;

	const t_location *current = Utils::findLocation(clientServer->getLocationSet(), request.getHeader().uri);

	if (current != NULL && !current->index.empty()) {
		request.completeUri.append(current->index);
		return false;
	}
	else if (current != NULL && current->autoindex == true )
		return true;
	else
		throw ErrorHandler(ERR_403, ("[" + request.getHeader().uri + "] is forbidden\n"));
}
/*----------------------------------------------------------------------------*/

/*	* build the complete uri and return the location associated with the path requested by client
*/
const t_location * Client::buildCompleteUri()
{
	std::string			rootPart;
	std::string			uriPart;
	const t_location	*result = Utils::findLocation(clientServer->getLocationSet(), request.getHeader().uri);
	
	if (result && result->redirect.size() != 0)
		return result;
	else if (result && ! result->root.empty() )
		rootPart = result->root;
	else
		rootPart = clientServer->getParams().rootPath;

	if (request.getHeader().uri.find("upload") != std::string::npos)
		uriPart = "";
	else if (result && ! result->index.empty())
		uriPart = result->index;
	else
		uriPart = request.getHeader().uri;

	if (*uriPart.begin() == '/' && *rootPart.end() == '/')
		rootPart.erase(rootPart.end());
	else if (*uriPart.begin() != '/' && *rootPart.end() != '/')
		rootPart += "/";

	request.completeUri = rootPart + uriPart;

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

	bool found = false;
	while (itStart != itEnd)
	{
		if (*itStart == request.getHeader().requestType) {
			found = true;
			break;
		}
		itStart++;
	}

	if (!found) {
		std::stringstream ss;
		throw ErrorHandler(ERR_405, "Method not allowed in the service\n");
	}
}
/*----------------------------------------------------------------------------*/

/*	* check the characters contained in the uri
		-> only allow Chars define in the RFC 3986 section 2.2
*/
void Client::checkUriContent() const throw (ErrorHandler)
{
	if (request.getHeader().uri.find_first_not_of(HTTP_ALLOW_CHARS) != request.getHeader().uri.npos)
		throw ErrorHandler(ERR_400, "an invalid charater is detected in the uri\n");
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
		token = Utils::split(request.getHeader().uri, "/");
		it = token.begin(); 
	}
	catch(const std::exception& e) {
		throw ErrorHandler(ERR_500, std::string(e.what(), '\n'));
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
			throw ErrorHandler(ERR_403, request.getHeader().uri + std::string(" is an invalid uri\n"));
		it++;
	}
}
/*----------------------------------------------------------------------------*/

void	Client::clearData()
{
	request.clearRequest();
	response.clearResponse();
	totalBytesReceived = 0;
}
/*----------------------------------------------------------------------------*/
