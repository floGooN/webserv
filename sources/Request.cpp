


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
	* http version
	* method type
	* path to ressource
	* Host error
*/
Request::Request(const std::string &response)
: _keepAlive(response.find("keep-alive") == response.npos ? false : true)
{
	size_t	idxBodySeparator = response.find(BODY_SEPARATOR);
	
	std::vector<std::string>					tokenHeader;
	std::vector<std::string>::const_iterator	itToken;

	tokenHeader = UtilParsing::split(response.substr(0, idxBodySeparator + 1), "\r\n"); // extract header and split it
	itToken = tokenHeader.begin();

	initContentLength(response);
	initRequestLine(*itToken);
	initHost(++itToken, tokenHeader.end());
	setBody(response.substr(idxBodySeparator + 4));
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
		_uri = ref._uri;
		_hostName = ref._hostName;
		_hostPort = ref._hostPort;
		_requestType = ref._requestType;
		_body = ref._body;
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

size_t	Request::getcontentlength()	const {
	return _contentLength;
}
/*----------------------------------------------------------------------------*/

void	Request::setBody(const std::string &body) {
	_body.empty() ? _body = body : _body.append(body);
}
/*----------------------------------------------------------------------------*/

void	Request::clearRequest()
{
	this->_body.clear();
	this->_contentLength = 0;
	// this->_hostName.clear();
	// this->_hostPort.clear();
	this->_keepAlive = false;
	this->_requestType.clear();
	this->_uri.clear();
}
/*----------------------------------------------------------------------------*/


/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

/*	* check validity of the request line (METHOD PATH_TO_RESSOURCE PROTOCOL_VERSION)
	* throw exception with the correct error code
*/
void	Request::initRequestLine(const std::string &requestLine)
{
	_requestType = requestLine.substr(0, requestLine.find_first_of(" "));
	//throw exception with error 415 -> Unsupported Media Type : Format de requête non supporté pour une méthode et une ressource données.
	if (_requestType.empty() == true) {
		std::cerr << RED "throw exception with error 415 in initRequestLine()" RESET << std::endl;
		throw std::exception();
	}

	//throw exception with error 400 -> Bad request : protocol non supporte
	if (requestLine.find(PROTOCOL_VERION) == std::string::npos) {
		std::cerr << RED "throw exception with error 400 in initRequestLine() : " YELLOW << requestLine << RESET << std::endl;
		throw std::exception();
	}

	//error maybe impossible
	size_t	idx = requestLine.find_first_of("/");
	if (idx == std::string::npos) {
		std::cerr << RED "\'/\' DOESN'T FIND IN requestLine in function initRequestLine()" RESET << std::endl;
		throw std::exception();
	}
	_uri = requestLine.substr(idx, requestLine.find_first_of(' ', idx) - idx);
}

/*	* extract the hostname and the host port required by the client
*/
void	Request::initHost(std::vector<std::string>::const_iterator &itToken, std::vector<std::string>::const_iterator itEnd)
{
	while (itToken != itEnd) {
		if (itToken->find("Host") != itToken->npos)
			break;
		itToken++;
	}
	if (itToken == itEnd) {
		std::cerr << RED "NO HOST in initHost()" << std::endl; // manage error
		return;
	}
	
	_hostName.clear();
	_hostPort.clear();
	size_t idxSpace = itToken->find_last_of(" ");
	size_t idxSemicolon = itToken->find_last_of(":");
	_hostName = itToken->substr(idxSpace + 1, idxSemicolon - idxSpace - 1);
	_hostPort = itToken->substr(idxSemicolon + 1, itToken->length() - idxSemicolon);
}
/*----------------------------------------------------------------------------*/

void	Request::initContentLength(const std::string &response)
{
	size_t idx = response.find("Content-Length");
	
	if (idx == response.npos) {
		_contentLength = 0;
		return ;
	}
	idx = response.find_first_of(' ', idx) + 1;
	_contentLength = UtilParsing::convertBodySize(response.substr(idx, response.length() - \
												response.find_first_of(' ', idx)));
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### EXCEPTIONS ###*/
/*============================================================================*/


/*----------------------------------------------------------------------------*/
