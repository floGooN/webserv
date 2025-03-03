
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
#include <sstream>

/*============================================================================*/
				/*### CONSTRUCTORS - DESTRUCTOR - OVERLOAD OP ###*/
/*============================================================================*/

/*	* if an error is detected throw an exception with the correct error code
	* set keep_alive
	* get content length
	* get content type
	* get uri and method
	* get host
	* get body (if detected)
*/
Request::Request(const std::string &response)
  : totalBytesReceived(0), totalBytessended(0),
	_keepAlive(response.find("keep-alive") == response.npos ? false : true)
{
	size_t	idxBodySeparator = response.find(BODY_SEPARATOR);
	if (!response.size() || idxBodySeparator == response.npos)
		throw std::runtime_error("400 bad request empty request\n");

	initContentLength(response);
	initContentType(response);
	
	std::vector<std::string>					tokenHeader;
	std::vector<std::string>::const_iterator	itToken;
	try {
		tokenHeader = UtilParsing::split(response.substr(0, idxBodySeparator), "\r\n"); // split header line by line
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		throw std::runtime_error("error 500 in constructor request\n");
	}
	
	itToken = tokenHeader.begin();
	if (itToken != tokenHeader.end())
	{
		initRequestLine(*itToken);
		initHost(++itToken, tokenHeader.end());
	}
	else
	throw std::runtime_error("surprising 400 bad request\n");
	setBody(response);
}
/*----------------------------------------------------------------------------*/

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
		_keepAlive = ref._keepAlive;
		_contentLength = ref._contentLength;
		_contentType = ref._contentType;
		_uri = ref._uri;
		_hostName = ref._hostName;
		_hostPort = ref._hostPort;
		_requestType = ref._requestType;
		_body = ref._body;
		totalBytesReceived = ref.totalBytesReceived;
		totalBytessended = ref.totalBytessended;
		_bound = ref._bound;
		_metaData = ref._metaData;
	}
	return *this;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream & o, Request &ref)
{
	o	<< BOLD BRIGHT_BLUE "Request:" << std::endl
		<< "_requestType: [" << ref.gettype() << "]" << std::endl
		<< "_uri: [" << ref.geturi() << "]" << std::endl
		<< "_hostName: [" << ref.gethostname() << "]" << std::endl
		<< "_hostPort: [" << ref.gethostport() << "]" << std::endl
		<< "_keepAlive: [" << (ref.getkeepalive() == true ? "true" : "false") << "]" << std::endl
		<< "_contentLength: [" << ref.getcontentlength() << "]" << std::endl
		<< "_contentType: [" << (ref.getcontenttype().empty() ? "" : ref.getcontenttype()) << "]" << std::endl
		<< "_bound: [" << ref.getbound() << "]" << std::endl
		<< std::endl
		<< "BODY :\n" << ref.getbody() << RESET;
	return o;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

bool Request::getkeepalive() const {
	return _keepAlive;
}
/*----------------------------------------------------------------------------*/

const std::string&	Request::geturi() const {
	return _uri;
}
/*----------------------------------------------------------------------------*/

const std::string&	Request::gethostname() const {
	return _hostName;
}
/*----------------------------------------------------------------------------*/

const std::string&	Request::gethostport() const {
	return _hostPort;
}
/*----------------------------------------------------------------------------*/

const std::string&	Request::gettype() const {
	return _requestType;
}
/*----------------------------------------------------------------------------*/

std::string&	Request::getbody() {
	return _body;
}
/*----------------------------------------------------------------------------*/

const std::string&	Request::getbound() const {
	return _bound;
}
/*----------------------------------------------------------------------------*/

size_t	Request::getcontentlength()	const {
	return _contentLength;
}
/*----------------------------------------------------------------------------*/

const std::string	&Request::getcontenttype()	const {
	return _contentType;
}
/*----------------------------------------------------------------------------*/

const std::string	&Request::getMetaData()	const {
	return _metaData;
}
/*----------------------------------------------------------------------------*/

void	Request::clearRequest()
{
	totalBytesReceived = 0;
	totalBytessended = 0;

	_keepAlive = false;
	_contentLength = 0;
	_uri.clear();
	_hostName.clear();
	_hostPort.clear();
	_requestType.clear();
	_body.clear();
	_contentType.clear();
	_bound.clear();	
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

/*	* check validity of the request line (METHOD PATH_TO_RESSOURCE PROTOCOL_VERSION)
	*
	* extract the method (GET POST DELETE)
	* check the HTTP protocole version HTTP/1.1
	* extract the uri
	*
	* throw exception with the correct error code
*/
void	Request::initRequestLine(const std::string &requestLine)
{
	try {
		_requestType = requestLine.substr(0, requestLine.find_first_of(" "));
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		throw std::runtime_error("throw exception with error 500 internal error in initRequestLine()\n");
	}

	if (_requestType.empty() == true || requestLine.find(PROTOCOL_VERION) == std::string::npos)
		throw std::runtime_error("throw 400 bad req in initRequestLine() unsupported protocol version\n");

	size_t	idx = requestLine.find_first_of("/");
	if (idx == std::string::npos)
		throw std::runtime_error("function initRequestLine() throw 400 bad req");
	
	try {
		_uri = requestLine.substr(idx, requestLine.find_first_of(' ', idx) - idx);
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		throw std::runtime_error("throw 500 internal error in initRequestLine()");
	}
}

/*	* extract the hostname and the host port required by the client
*/
void	Request::initHost(std::vector<std::string>::const_iterator &itToken, std::vector<std::string>::const_iterator itEnd)
{
	while (itToken != itEnd)
	{
		if (itToken->find("Host") != itToken->npos)
			break;
		itToken++;
	}
	if (itToken == itEnd)
		throw std::runtime_error("400 Bad request no host specified\n");
	
	try {
		size_t idxSpace = itToken->find_last_of(" ");
		size_t idxSemicolon = itToken->find_last_of(":");

		_hostName = itToken->substr(idxSpace + 1, idxSemicolon - idxSpace - 1);
		_hostPort = itToken->substr(idxSemicolon + 1, itToken->length() - idxSemicolon);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("500 internal error\n");
	}
}
/*----------------------------------------------------------------------------*/

void	Request::initContentLength(const std::string &response)
{
	size_t idx = response.find("Content-Length");
	if (idx == response.npos) {
		_contentLength = 0;
		return ;
	}
	idx = response.find_first_of("0123456789", idx);
	if (idx != response.npos && !response[idx]) {
		_contentLength = 0;
		return ;
	}
	try {
		_contentLength = UtilParsing::convertBodySize(response.substr(idx, response.length() - \
													response.find_first_of("0123456789", idx)));
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		throw std::runtime_error("error 500 initContentLength() request constructor\n");
	}
}
/*----------------------------------------------------------------------------*/

/*	* 3 types value possible (help for the parsing of arguments)
	*
	* application/x-www-form-urlencoded
		-> Default setting. All characters are encoded before sent
			.spaces are converted to "+" symbols
			.special characters are converted to ASCII HEX values (ex: %20 == ASCII ' ' 32)
	* multipart/form-data
		-> necessary if the user will upload a file through the form
			. body is detached from url
	* text/plain
		-> Sending data without any encoding at all. Not recommended
	
	for each of them, data is in a specific format in the request
*/
void	Request::initContentType(const std::string &response)
{
	size_t	idx = response.find("Content-Type");
	_contentType.clear();
	_metaData.clear();
	_bound.clear();
	if (idx == response.npos)
		return;
	if ((idx = response.find_first_of(' ', idx)) == response.npos) {
		_contentType = DFLT_CONTENT_TYPE;
		return;
	}

	try {
		idx++;
		_contentType = response.substr(idx, response.find_first_of("\r\n", idx) - idx);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		throw std::runtime_error("500 internal error in initContentType in " __FILE__);
	}
}
/*----------------------------------------------------------------------------*/

size_t	Request::skipMetaData(const std::string &body)
{
	try
	{
		size_t	idx = _bound.empty() ? 0 : body.find(_bound);

		idx == 0 ? idx = body.find("\r\n\r\n") : idx = body.find("\r\n\r\n", idx);
		if (idx == body.npos)
			throw std::runtime_error("Error 400 bad request format in skipMetaData() in " __FILE__);
		return idx + 4;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		throw std::runtime_error("internal error 500 in " __FILE__);
	}
	return body.npos;
}
/*----------------------------------------------------------------------------*/

void	Request::setBody(const std::string &body)
{
	if (this->_requestType.compare("GET") == 0)
		return;

	if (_contentType.find("multipart/form-data") != _contentType.npos)
		extractBound(body);
	size_t idx = skipMetaData(body);
	if (idx == body.npos)
		throw std::runtime_error("400 bad request in " __FILE__ " bound doesn't found");
	
	_body = body.substr(idx, body.size() - idx);
}
/*----------------------------------------------------------------------------*/

void	Request::extractBound(const std::string &contentType)
{
	size_t idx = contentType.find("boundary=", contentType.find("multipart/form-data"));
	if (idx == contentType.npos)
		throw std::runtime_error("400 bad request in " __FILE__ " no body separator found");
	try {
		idx += 9;
		_bound = contentType.substr(idx, contentType.find("\r\n", idx) - idx);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		throw std::runtime_error("500 internal error in " __FILE__);
	}
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### EXCEPTIONS ###*/
/*============================================================================*/


/*----------------------------------------------------------------------------*/
