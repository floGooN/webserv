/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fberthou <fberthou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 06:24:44 by fberthou          #+#    #+#             */
/*   Updated: 2025/04/07 07:13:46 by fberthou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
						/*### HEADERS & STATIC FIELD ###*/
/*============================================================================*/

#include "Request.hpp"
#include "Utils.hpp"

/*============================================================================*/
				/*### CONSTRUCTORS - DESTRUCTOR - OVERLOAD OP ###*/
/*============================================================================*/

Request::Request() {
	keepAlive = false;
	clearRequest();
}

Request::Request(const std::string &content) throw (ErrorHandler)
{
	keepAlive = false;
	size_t delimiter = content.find("\r\n\r\n");
	if (delimiter == std::string::npos)
		throw ErrorHandler(ERR_400, "Bad request in Request constructor");

	try
	{
		setHeader(content.substr(0, delimiter));
		delimiter += 4;
		setBody(content.substr(delimiter, content.length() - delimiter));
	}
	catch(const std::exception& e) {
		throw ErrorHandler(ERR_500, "In Request(): " + std::string(e.what()));
	}
}
/*----------------------------------------------------------------------------*/

Request::Request(const Request &ref) {
	keepAlive = false;
	*this = ref;
}
/*----------------------------------------------------------------------------*/

Request::~Request()
{	}
/*----------------------------------------------------------------------------*/

Request & Request::operator=(const Request &ref)
{
	if (this != &ref)
	{
		keepAlive = ref.keepAlive;
		completeUri = ref.completeUri;

		_body = ref._body;
		_args = ref._args;
		_header = ref._header;
	}
	return *this;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream & o, const Request &ref)
{
	o	<< BOLD "Request:\n"
		<< BOLD "KeepAlive: " RESET << ref.keepAlive << std::endl
		<< BOLD "Header:\n" << ref.getHeader()
		<< BOLD "Args:\n" << ref.getArgs() << std::endl
		<< BOLD "Body:\n" << ref.getbody();

	return o << RESET << std::endl;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream &o, const t_header &ref)
{
    o   << ITAL BLUE "t_header:\n"
        << "URI: " << ref.uri << std::endl
        << "Request type: " << ref.requestType << std::endl
        << "HostPort: " << ref.hostPort << std::endl
        << "HostName: " << ref.hostName << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = ref.otherFields.begin(); \
		it != ref.otherFields.end(); it++)
		o << it->first << ": " << it->second << std::endl;

    return o << RESET;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream &o, const t_body &ref)
{
    o   << ITAL BRIGHT_BLUE "t_header:\n"
        << "Content type: " << ref.contentType << std::endl
        << "Content length: " << ref.contentLength << std::endl
        << "Boundary: " << ref.bound;

    return o << RESET << std::endl;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

const t_header & Request::getHeader() const {
	return _header;
}
/*----------------------------------------------------------------------------*/

const t_body & Request::getbody() const {
	return _body;
}
/*----------------------------------------------------------------------------*/

const std::string &	Request::getArgs() const {
	return _args;
}
/*----------------------------------------------------------------------------*/

void Request::updateRequest(const std::string &content) throw(ErrorHandler) {
	setBody(content);
}
/*----------------------------------------------------------------------------*/

void Request::clearRequest()
{
	keepAlive = false;
	completeUri.clear();

	_body.clear();
	_args.clear();
	_header.clear();
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

/*	* search for arguments in the URI
	*
	* if no argument -> do nothing
	* else -> separate arguments and path from uri
*/
void Request::setArgs() throw(ErrorHandler)
{
	size_t idxSep = _header.uri.find_first_of('?');

	if (idxSep == _header.uri.npos)
		return ;
	try {
		_args = _header.uri.substr(idxSep, _header.uri.length() - idxSep);
		_header.uri = _header.uri.substr(0, idxSep);
	}
	catch(const std::exception& e) {
		throw ErrorHandler(ERR_500, "In setArgs() : " + std::string(e.what()));
	}
}
/*----------------------------------------------------------------------------*/

/*	* extract all information from the header of the query
	*
	* type of query (GET - POST - DELETE)
	* check the http version (1.1)
	* get the entire uri (with arguments)
	* get hostPort and hostName
	* set keep-alive
	* set content length (for POST request)
	* set content-type (for POST request)
*/
void Request::setHeader(const std::string &header) throw(ErrorHandler)
{
	std::vector<std::string> token;

	try {
		token = Utils::split(header, "\r\n");
	}
	catch (std::exception &e) {
		throw ErrorHandler(ERR_500, "in Request::setHeader(); " + std::string(e.what()));
	}

	std::vector<std::string>::iterator it = token.begin();
	if (it->find(PROTOCOL_VERION) != it->npos)
	{
		setRequestType(*it);
		checkProtocole(*it);
		setUri(*it);
		setArgs();
	}
	while (++it != token.end())
	{
		if (it->find("Host: ") != it->npos)
			setHost(*it);
		else if (it->find("Connection: ") != it->npos)
			setKeepAlive(*it);
		else if (_header.requestType == POST && it->find("Content-Length: ") != it->npos)
			setContentLength(*it);
		else if (_header.requestType == POST && it->find("Content-Type") != it->npos)
			setContentType(*it);
		else
			addHeaderField(*it);
	}
}
/*----------------------------------------------------------------------------*/

void Request::setRequestType(const std::string &line) throw(ErrorHandler)
{
	size_t idx = line.find_first_of(' ');
	if (idx == line.npos)
		throw ErrorHandler(ERR_400, "Invalid format header");
	
	std::string type("");
	try {
		type = line.substr(0, idx);
	}
	catch(const std::exception& e) {
		throw ErrorHandler(ERR_500, "in Request::setRequestType(): " + std::string(e.what()));
	}

	if ( type.compare("GET") == 0 )
		_header.requestType = GET;
	else if ( type.compare("POST") == 0 )
		_header.requestType = POST;
	else if ( type.compare("DELETE") == 0 )
		_header.requestType = DELETE;
	else
		throw ErrorHandler(ERR_405, "Request type " + type + " doesn't handle by the server");
}
/*----------------------------------------------------------------------------*/

/*	* Check if the protocole version is correct, throw ErrorHandler if not
*/
void Request::checkProtocole(const std::string &line) throw(ErrorHandler) {
	if ( line.find(PROTOCOL_VERION) == line.npos )
		throw ErrorHandler(ERR_505, "Http version not supported");
}
/*----------------------------------------------------------------------------*/

/*	* extract the entire uri (path + args)
*/
void Request::setUri(const std::string &line) throw(ErrorHandler)
{
	size_t idx = line.find_first_of(' ');
	if (idx == line.npos)
		throw ErrorHandler(ERR_400, "Invalid format header");

	idx++;
	try {
		size_t uriSize = line.find_first_of(' ', idx) - idx;
		_header.uri = line.substr(idx, uriSize);
	}
	catch(const std::exception& e) {
		throw ErrorHandler(ERR_500, "in Request::setRequestType(): " + std::string(e.what()));
	}
}
/*----------------------------------------------------------------------------*/

/*	* extract the host port and host name from the header of the query
*/
void Request::setHost(const std::string &line) throw(ErrorHandler)
{
	try {
		size_t idxSpace = line.find_last_of(" ");
		size_t idxSemicolon = line.find_last_of(":");

		_header.hostName = line.substr(idxSpace + 1, idxSemicolon - idxSpace - 1);
		_header.hostPort = line.substr(idxSemicolon + 1, line.length() - idxSemicolon);
	}
	catch (std::exception &e) {
		throw ErrorHandler(ERR_500, "In setHost(): " + std::string(e.what()));
	}
	if (_header.hostName.empty() || _header.hostPort.empty())
	{
		std::string errlog("");
		if ( ! _header.hostName.empty() )
			errlog = "No hostname\n";
		if ( ! _header.hostPort.empty() )
			errlog += "No hostport\n";
		throw ErrorHandler(ERR_400, errlog + "Invalid request");
	}
}
/*----------------------------------------------------------------------------*/

/*	* set keep-alive at true if its ask in the header
*/
void Request::setKeepAlive(const std::string &line) {
	keepAlive = (line.find("Connection: keep-alive") == line.npos ? false : true);
}
/*----------------------------------------------------------------------------*/

void Request::setContentLength(const std::string &line) throw(ErrorHandler)
{
	size_t idx = line.find_first_of("0123456789");
	if (idx == line.npos) {
		return ;
	}

	size_t		tokenSize = line.find_first_not_of("0123456789", idx) - idx;
	std::string lenAsStr("");
	
	try {
		lenAsStr = line.substr(idx, tokenSize);
	}
	catch(const std::exception& e) {
		throw ErrorHandler(ERR_500, " in setContentLength(): " + std::string(e.what()));
	}

    size_t				result;
	std::istringstream	iss(lenAsStr);
    iss >> result;

    if (iss.fail())
		throw ErrorHandler(ERR_500, "In Response::setHeader()\nconversion of the length message faild");

	_body.contentLength = result;
}
/*----------------------------------------------------------------------------*/

void Request::addHeaderField(const std::string &line)
{
	size_t i = line.find_first_of(' ');

	if (i == line.npos)
		return;

	_header.otherFields[line.substr(0, i)] = line.substr((i + 1), line.length() - (i + 1));
}
/*----------------------------------------------------------------------------*/

void Request::setContentType(const std::string &line) throw(ErrorHandler)
{
	if (line.find("application/x-www-form-urlencoded") != line.npos)
		_body.contentType = APP;
	else if (line.find("multipart/form-data") != line.npos)
		_body.contentType = MULTIPART;
	else if (line.find("text/plain"))
		_body.contentType = TXT;
	else
		throw ErrorHandler(ERR_400, "Unkwown body content type");
	
	if (_body.contentType == MULTIPART)
		setBoundLimiter(line);
}
/*----------------------------------------------------------------------------*/

void Request::setBody(const std::string &body) throw(ErrorHandler)
{
	if (body.empty())
		return;
	try {
		_body.body.empty() ? _body.body = body : _body.body.append(body);
	}
	catch (const std::exception &e) {
		throw ErrorHandler(ERR_500, "In setBody(): " + std::string(e.what()));
	}
}
/*----------------------------------------------------------------------------*/

/*	* get the delimiter for query POST type multipart/form-data (----webKit)
*/
void	Request::setBoundLimiter(const std::string &line) throw(ErrorHandler)
{
	size_t idx = line.find("boundary=") ;
	if (idx == line.npos)
		throw ErrorHandler(ERR_400, "No bound found");

	try {
		idx += 9;
		_body.bound = line.substr(idx, line.find("\r\n", idx) - idx);
	}
	catch(const std::exception& e) {
		throw ErrorHandler(ERR_500, "In setBoundLimiter(): " + std::string(e.what()));
	}
}
/*----------------------------------------------------------------------------*/
