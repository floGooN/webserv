
/*	* les requetes
	*
	* ENTETES OBLIGATOIRES
	* ligne de requete  [GET / HTTP/1.1] -> toujours en premiere ligne
	* Host:		[Host: localhost:8080]
	*
	* ENTETES CONSEILLES
	* Connection:	-> keep-alive ou non
	* Accept:		-> type mime
	*
	* ENTETES IGNORES
	* Accept-language:	-> langue favorite client
	* Accept-encoding:	-> compressions supportes par client
	* Upgrade-Insecure-Requests:	-> demande redirection https
	* 
	* AUTRES ENTETES
	* User-Agent:		-> info sur client (OS-navigateur-arch microprocesseur - serveur graphique) util pour cookies (a mon avis)
	* Cache-control:	-> pour gerer cache cote client
	* sec-ch-ua, sec-ch-ua-mobile, sec-ch-ua-platform -> infos environement client
*/

/*============================================================================*/
						/*### HEADERS & STATIC FIELD ###*/
/*============================================================================*/

#include "Request.hpp"
#include "UtilParsing.hpp"

/*============================================================================*/
				/*### CONSTRUCTORS - DESTRUCTOR - OVERLOAD OP ###*/
/*============================================================================*/

Request::Request(const std::string &content) throw (ErrorHandler)
  :	totalBytessended(0) {
	updateRequest(content);
}
/*----------------------------------------------------------------------------*/

// /*	* this constructor make a "fake request" to handle an error
// 	* doesn't throwing an error
// */
// Request::Request(const Cluster &cluster) {

// }
// /*----------------------------------------------------------------------------*/

Request::Request(const Request &ref) {
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
		this->keepAlive = ref.keepAlive;
		this->totalBytessended = ref.totalBytessended;

		_args = ref._args;
		_header = ref._header;
		_body = ref._body;
	}
	return *this;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream & o, Request &ref)
{
	o	<< BOLD BRIGHT_BLUE "Request:\n"
		<< BOLD BRIGHT_BLUE "Header:\n" << ref.getHeader() << std::endl
		<< BOLD BRIGHT_BLUE "Args:\n" << ref.getArgs() << std::endl
		<< BOLD BRIGHT_BLUE "Body:\n" << ref.getbody() << std::endl
		<< BOLD BRIGHT_BLUE "body_content:\n" << ref.getbody().body;

	return o << RESET << std::endl;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

const s_header & Request::getHeader() const {
	return _header;
}
/*----------------------------------------------------------------------------*/

const s_body & Request::getbody() const {
	return _body;
}
/*----------------------------------------------------------------------------*/

const std::string &	Request::getArgs() const {
	return _args;
}
/*----------------------------------------------------------------------------*/

void Request::updateRequest(const std::string &content) throw(ErrorHandler)
{
	size_t idxSeparator = content.find(BODY_SEPARATOR);
	if (idxSeparator == content.npos)
	{
		if ( content.empty() )
			throw ErrorHandler(ERR_400, "the request is empty");
		setBody(content);
	}
	else
	{
		try
		{
			setHeader(content.substr(0, idxSeparator));
			setArgs();
			if ( _header.requestType == POST ) {
				idxSeparator += 4;
				setBody(content.substr(idxSeparator, content.length() - idxSeparator));
			}
		}
		catch(const std::exception& e) {
			std::string log = RED "in updateRequest(): " + std::string(e.what()) + RESET;
			throw ErrorHandler(ERR_500, log);
		}
	}
}
/*----------------------------------------------------------------------------*/

void Request::clearRequest()
{
	totalBytessended = 0;
	keepAlive = false;

	_args.clear();
	_header.clear();
	_body.clear();
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

/*	* search for arguments in the URI
	*
	* if no argument -> return nothing
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
#ifdef TEST
	std::cout	<< ITAL BLUE "Request::setArgs()\n"
				<< "_args: [" << _args << "]" << std::endl
				<< "uri: [" << _header.uri << "]"
				<< RESET << std::endl;
#endif
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
#ifdef TEST
	std::cout	<< ITAL BLUE "setHeader():" << std::endl
				<< "brut header:\n" << header
				<< RESET << std::endl;
#endif

	std::vector<std::string> token;
	try {
		token = UtilParsing::split(header, "\r\n");
	}
	catch (std::exception &e) {
		throw ErrorHandler(ERR_500, "in Request::setHeader(); " + std::string(e.what()));
	}

	std::vector<std::string>::iterator it = token.begin();
	
	setRequestType(*it);
	checkProtocole(*it);
	setUri(*it);
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
	}
#ifdef TEST
	std::cout	<< ITAL BLUE "Parsed header:\n"
				<< _header
				<< RESET << std::endl;
#endif
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
	*
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
	*
	* throw Bad request if port or name is empty
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
	if (_header.hostName.empty() || _header.hostPort.empty()) {
		std::string errlog = _header.hostName.empty() ? "No hostname " : "No hostport ";
		throw ErrorHandler(ERR_400, errlog + "detected in the query");
	}
}
/*----------------------------------------------------------------------------*/

/*	* set keep-alive at true if its ask in the header
*/
void Request::setKeepAlive(const std::string &line) {
	_header.keepAlive = (line.find("close") == line.npos ? true : false);
}
/*----------------------------------------------------------------------------*/

void Request::setContentLength(const std::string &line) throw(ErrorHandler)
{
	size_t idx = line.find_first_of("0123456789");
	if (idx == line.npos) {
		return ;
	}

	try {
		size_t		tokenSize = line.find_first_not_of("0123456789", idx) - idx;
		std::string lenAsStr = line.substr(idx, tokenSize);
		_body.contentLength = UtilParsing::convertBodySize(lenAsStr);
	}
	catch(const std::exception& e) {
		throw ErrorHandler(ERR_500, " in setContentLength(): " + std::string(e.what()));
	}
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
