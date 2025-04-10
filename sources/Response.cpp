/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fberthou <fberthou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 05:11:42 by fberthou          #+#    #+#             */
/*   Updated: 2025/04/09 13:56:15 by fberthou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
						/*### HEADERS & STATIC FIELD ###*/
/*============================================================================*/

#include "Client.hpp"
#include "Response.hpp"
#include "Utils.hpp"
#include <sys/stat.h>
#include "CGI.hpp"

/*============================================================================*/
				/*### CONSTRUCTORS - DESTRUCTOR - OVERLOAD OP ###*/
/*============================================================================*/

Response::Response()
{
	clearResponse();
	initMimeMap();
}
/*----------------------------------------------------------------------------*/

Response::Response(const Response &ref) {
	initMimeMap();
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
		message = ref.message;
		totalBytesSended = ref.totalBytesSended;
	}
	return *this;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream &o, const Response &ref)
{
	o	<< "Response:\n"
		<< BOLD "Total bytes sended: " << ref.totalBytesSended << std::endl
		<< "File + Header: " << ref.message;

	return o;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

void	Response::getQuery(Client &client)
{
	std::stringstream ss;
	ss << "Request received GET : [" << client.request.getHeader().uri << "] from client [" << client.fdClient << "]" << std::endl;
	Utils::printLog(INFO, ss.str());

	if (isRedirect(client) == true)
	{
		client.response.message = setHeaderRedirect(client);
		return ;
	}
	else if (isCGI(client) == true) 
		message = processCGI(client);
	else if (isRepository(client) == true)
		message = processAutoIndex(client);
	else
		Utils::readFile(client.request.completeUri, message);

	try {
		message.insert(0, setHeader(client, (message.empty() ? COD_204 : COD_200 )));
	}
	catch(const std::exception& e) {
		throw ErrorHandler(ERR_500, "in getQuery(): " + std::string(e.what(), '\n'));
	}
}
/*----------------------------------------------------------------------------*/

void	Response::postQuery(Client &client)
{
	std::stringstream ss;
	ss << "Request received POST : [" << client.request.getHeader().uri << "] from client [" << client.fdClient << "]" << std::endl;
	Utils::printLog(INFO, ss.str());
	
	Utils::checkAccessRessource(client.request.completeUri, W_OK);
	if (isCGI(client) == true)
		message = processCGI(client);
	else
	{
		if (client.request.getbody().contentType != MULTIPART)
			throw ErrorHandler(ERR_415, "The media type is not supported by the server\n");
		uploadFile(client);
		client.request.completeUri = "./uploads/uploadSucces.html";
		Utils::readFile(client.request.completeUri, message);
	}
	try {
		message.insert(0, setHeader(client, COD_201));
	}
	catch(const std::exception& e) {
		throw ErrorHandler(ERR_500, "in getQuery(): " + std::string(e.what(), '\n'));
	}	
}
/*----------------------------------------------------------------------------*/

void	Response::deleteQuery(const Client &client)
{
	std::stringstream ss;
	ss << "Request received DELETE : [" << client.request.getHeader().uri << "] from client [" << client.fdClient << "]" << std::endl;
	Utils::printLog(INFO, ss.str());

	char		path[100];
	if (realpath(client.request.completeUri.c_str(), path) == NULL) {
		if (errno == ENOENT)
			throw ErrorHandler(ERR_404, "File to delete not found\n");
		throw ErrorHandler(ERR_400, "realpath() in DELETE, invalid path\n");
	}
	if (access(path, F_OK) == -1)
		throw ErrorHandler(ERR_404, "Not found in delete()\n");
	else if (access(path, W_OK) == -1)
		throw ErrorHandler(ERR_403, "Acces forbidden\n");
    else
    {
        struct stat path_stat;
        stat(path, &path_stat);
        if (S_ISREG(path_stat.st_mode))
        {
            if (remove(path) != 0)
				throw ErrorHandler(ERR_500, "remove() in deleteQuery()\n");
        }
        else
			throw ErrorHandler(ERR_403, "Forbidden removing\n");
    }
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

std::string Response::extractFilename(const std::string &bodyHeader) throw (ErrorHandler)
{
	size_t i = bodyHeader.find("filename=");
	if (i == std::string::npos)
		throw ErrorHandler(ERR_400, "The format of the request is wrong (missing filename)\n");
	i += 9;
	std::vector<std::string> res = Utils::split(bodyHeader.substr(i, bodyHeader.find_first_of("\r\n") - i), "\"");
	for (std::vector<std::string>::iterator it = res.begin(); it != res.end(); it++)
	{
		if (it->compare("\""))
			return *it;
	}	
	return "";
}
/*----------------------------------------------------------------------------*/

void Response::uploadFile(const Client &client) throw (ErrorHandler)
{
	const t_body	&ref = client.request.getbody();
	size_t			iStart = ref.body.find("\r\n\r\n");
	
	if (iStart== std::string::npos)
		throw ErrorHandler(ERR_400, "No separator in the file to upload\n");
		
	std::string bodyHeader = ref.body.substr(0, iStart);
		
	size_t	endOfFile = ref.body.find(ref.bound, iStart);

	if (endOfFile == std::string::npos)
		throw ErrorHandler(ERR_400, "No EOF delimiter in the file to upload\n");

	endOfFile = ref.body.find_last_of('\n', endOfFile);

	iStart += 4;
	endOfFile -= 1;
	
	std::string filename = extractFilename(bodyHeader);
	if (filename.empty() == true)
		throw ErrorHandler(ERR_400, "no file name is specified\n");
	
	filename.insert(0, client.request.completeUri);

	std::ofstream ss(filename.c_str(), std::ios::binary);
	if (! ss)
		throw ErrorHandler(ERR_500, "in uploadFile()\n");

	ss.write(ref.body.c_str() + iStart, endOfFile - iStart);
	
	std::stringstream str;
	str << "File uploaded: " << filename << std::endl;
	Utils::printLog(INFO, str.str()); 
}
/*----------------------------------------------------------------------------*/

std::string	&Response::findMimeType(const std::string &uri)
{
	try {
		return _mimeMap.at(Utils::recoverExtension(uri));
	}
	catch(const std::exception& e) 
	{
		return _mimeMap.at(".html");
	}
}
/*----------------------------------------------------------------------------*/

std::string	Response::setHeader(const Client &client, const std::string &code) throw (ErrorHandler)
{
	std::ostringstream oss;
	oss << message.length();

	if (oss.fail())
		throw ErrorHandler(ERR_500, "In Response::setHeader()\nconversion of the length message faild\n");

	std::string header = \
		PROTOCOL_VERION " " + code + HTTP_SEPARATOR \
		"Server: Rob&Flo V1" + HTTP_SEPARATOR \
		"Content-Type: " + findMimeType(client.request.completeUri) + "; charset=UTF-8\r\n" \
		"Content-Length: " + oss.str() + HTTP_SEPARATOR \
		"Connection: " + (client.request.keepAlive == true ? "keep-alive" : "close") + HTTP_SEPARATOR;
	setCookies(header, client);
	return header;	
}
/*----------------------------------------------------------------------------*/

std::string	Response::setHeaderRedirect(const Client &client) throw (ErrorHandler)
{
	std::string res;
	const t_location *current = Utils::findLocation(client.clientServer->getLocationSet(), client.request.getHeader().uri);
	if (!current)
		throw ErrorHandler(ERR_444, "\n");
	if (current->redirect[0] == "302")
	{
		res = PROTOCOL_VERION " " + current->redirect[0]+ " Found" + HTTP_SEPARATOR \
		"Server: Rob&Flo V1" + HTTP_SEPARATOR \
		"Content-Type: " + "text/html" + "; charset=UTF-8" HTTP_SEPARATOR \
		"Content-Length: 0" HTTP_SEPARATOR \
		"Connection: close" HTTP_SEPARATOR \
		"Location: " + current->redirect[1] + HTTP_SEPARATOR;
	}
	else
	{
		res = PROTOCOL_VERION " " + current->redirect[0]+ " Moved Permanently" + HTTP_SEPARATOR \
		"Server: Rob&Flo V1" + HTTP_SEPARATOR \
		"Content-Type: " + "text/html" + "; charset=UTF-8" HTTP_SEPARATOR \
		"Content-Length: 0" + HTTP_SEPARATOR \
		"Connection: close" + HTTP_SEPARATOR \
		"Location: " + current->redirect[1] + HTTP_SEPARATOR;
	}
	setCookies(res, client);
	return res;
}
/*----------------------------------------------------------------------------*/

void	Response::setCookies(std::string &header, const Client &client)
{
	std::stringstream ss;
	ss	<< "Set-Cookie: user_id=" << client.fdClient
		<< "; Path=" << client.request.getHeader().uri
		<< "; HttpOnly" HTTP_SEPARATOR
		<< HTTP_SEPARATOR;
	header += ss.str();
}
/*----------------------------------------------------------------------------*/

bool	Response::isRedirect(const Client & client)
{
	const t_location *current = Utils::findLocation(client.clientServer->getLocationSet(), client.request.getHeader().uri);

	if (current && current->redirect.size() != 0)
		return true;
	return false;
}
/*----------------------------------------------------------------------------*/

bool	Response::isCGI(Client client) throw (ErrorHandler)
{
	if (checkExtensionCGI(client.request.getHeader().uri) == true)
	{
		const t_location *current = Utils::findLocation(client.clientServer->getLocationSet(), client.request.getHeader().uri);
		if (current == NULL)
			throw ErrorHandler(ERR_404, "\n");
		std::string path = current->root + client.request.getHeader().uri;
		if (Utils::isDirectory(path) == true)
			return false;
		if (access(path.c_str(), X_OK) != 0)
			throw ErrorHandler(ERR_403, "\n");
		return true;
	}
	return false;
}
/*----------------------------------------------------------------------------*/

bool Response::isRepository(Client client)
{
	if (client.request.getHeader().uri == "/")
		return false;
	const t_location *current = Utils::findLocation(client.clientServer->getLocationSet(), client.request.getHeader().uri);
	if (current == NULL)
		return false;
	std::string path = current->root + client.request.getHeader().uri;
	return Utils::isDirectory(path);
}
/*----------------------------------------------------------------------------*/

void	Response::clearResponse()
{
	message.clear();
	totalBytesSended = 0;
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
	_mimeMap.insert(std::make_pair(".urlencoded", "application/x-www-form-urlencoded"));
}
/*----------------------------------------------------------------------------*/
