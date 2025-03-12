
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
#include "Cluster.hpp"
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
Request::Request(const std::string &content)
throw (ErrorHandler) : totalBytessended(0), keepAlive(false)
{
	updateRequest(content);
}
/*----------------------------------------------------------------------------*/

/*	* this constructor make a "fake request" to handle an error
	* doesn't throwing an error
*/
Request::Request(const Cluster &cluster) {

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
		this->keepAlive = ref.keepAlive;
		this->totalBytessended = ref.totalBytessended;

		_header = ref._header;
		_body = ref._body;
	}
	return *this;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream & o, Request &ref)
{
	o	<< BOLD BRIGHT_BLUE "Request:" << std::endl
		<< "_requestType: [" << ref.getHeader().requestType << "]" << std::endl
		<< "_uri: [" << ref.getHeader().uri << "]" << std::endl
		<< "_hostName: [" << ref.getHeader().hostName << "]" << std::endl
		<< "_hostPort: [" << ref.getHeader().hostPort << "]" << std::endl
		<< "keepAlive: [" << (ref.keepAlive == true ? "true" : "false") << "]" << std::endl
		<< "_contentLength: [" << ref.getbody().body.length() << "]" << std::endl
		<< "_contentType: [" << (ref.getbody().contentType.empty() ? "" : ref.getbody().contentType) << "]" << std::endl
		<< "_bound: [" << ref.getbody().bound << "]" << std::endl
		<< std::endl
		<< "body_content :\n" << ref.getbody().body << RESET;
	return o;
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

void Request::updateRequest(const std::string &content)
throw(ErrorHandler)
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
			if ( ! _header.requestType.compare("POST") ) {
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

	_header.clear();
	_body.clear();
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

void Request::setHeader(const std::string &header)
throw(ErrorHandler)
{
#ifdef TEST
	std::cout	<< ITAL BLUE "setHeader():" << std::endl
				<< "brut header:\n" << header
				<< RESET << std::endl;
#endif
	// split le header ligne par ligne
	// remplir tous les infos dans header ET body (contentLength & contentType)

}
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/




void Request::setBody(const std::string &body)
throw(ErrorHandler)
{
#ifdef TEST
	std::cout	<< ITAL CYAN "setBody():" << std::endl
				<< "brut body:\n" << body
				<< RESET << std::endl;
#endif

	this->_body.body;
	this->_body.bound;
	this->_body.contentType;


}
/*----------------------------------------------------------------------------*/

void	Request::setContentType()
throw(ErrorHandler)
{
	// this->_header.
}
/*----------------------------------------------------------------------------*/

/*	* get the delimiter for query POST type multipart/form-data (----webKit)
*/
void	Request::setBoundLimiter()
throw(ErrorHandler)
{
	
	size_t idx = _header.requestType.find("boundary=");
	if (idx == _header.requestType.npos)
		throw ErrorHandler(ERR_400, "No bound found");

	try {
		idx += 9;
		_body.bound = contentType.substr(idx, contentType.find("\r\n", idx) - idx);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		throw std::runtime_error("500 internal error in " __FILE__);
	}
}
/*----------------------------------------------------------------------------*/


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

}

/*	* extract the hostname and the host port required by the client
*/
void	Request::initHost(std::vector<std::string>::const_iterator &itToken, std::vector<std::string>::const_iterator itEnd)
{

}
/*----------------------------------------------------------------------------*/

/*	* get size of the body
*/
void	Request::initContentLength(const std::string &response)
{
}
/*----------------------------------------------------------------------------*/


size_t	Request::skipHeader(const std::string &body)
{
	size_t	idx = body.find(this->_body.bound);
	if (idx == body.npos)
		idx = body.find(BODY_SEPARATOR);
	else
		idx = body.find(BODY_SEPARATOR, idx);
	if (idx == body.npos)
		return 0;
	return idx + 4;
}
/*----------------------------------------------------------------------------*/

void	Request::setBody(const std::string &body, ssize_t)
{
}
/*----------------------------------------------------------------------------*/

