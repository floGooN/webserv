


/*============================================================================*/
						/*### HEADERS & STATIC FIELD ###*/
/*============================================================================*/

#include "Client.hpp"
#include "Response.hpp"
#include "UtilParsing.hpp"

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
void	Response::getQuery(const Client &client)
{
	std::cout << BRIGHT_GREEN "GET QUERY" RESET << std::endl;

	if (isCGI(client.request) == true) {

		std::cout << "It's CGI\n"; // here play CGI
		throw ErrorHandler(ERR_404, "CGI"); // provisoirement
	}
	else
	{
		UtilParsing::readFile(client.request.completeUri, message);

		try {
			message.insert(0, setHeader(client.request, (message.empty() ? COD_204 : COD_200 )));
		}
		catch(const std::exception& e) {
			throw ErrorHandler(ERR_500, "in getQuery(): " + std::string(e.what()));
		}
	}
}
/*----------------------------------------------------------------------------*/

void	Response::postQuery(Client &client)
{
	std::cout	<< BRIGHT_YELLOW "POST QUERY" RESET << std::endl;

	UtilParsing::checkAccessRessource(client.request.completeUri, W_OK);

	if (isCGI(client.request) == true) {
		std::cout << BRIGHT_YELLOW "It's CGI\n" RESET; // here play CGI
		throw ErrorHandler(ERR_404, "CGI"); // provisoirement
	}
	else
	{
		if (client.request.getbody().contentType != MULTIPART)
			throw ErrorHandler(ERR_415, "The media type is not supported by the server");
		cleanBody(client.request);getsid
		uploadFile(client.request);

		client.request.completeUri = "./uploads/uploadSucces.html";
		UtilParsing::readFile(client.request.completeUri, message);
		try {
			message.insert(0, setHeader(client.request, COD_201));
		}
		catch(const std::exception& e) {
			throw ErrorHandler(ERR_500, "in getQuery(): " + std::string(e.what()));
		}		
	}
}
/*----------------------------------------------------------------------------*/

void	Response::deleteQuery(const Client &)
{
	std::cout << BRIGHT_CYAN "DELETE QUERY" << RESET << std::endl;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

void Response::uploadFile(const Request &req) throw (ErrorHandler)
{
	// extract the name of the file
	//  
	const t_body	&ref = req.getbody();
	size_t			iStart = ref.body.find_first_of("\r\n\r\n");
	size_t			endOfFile = ref.body.find_last_of("\r\n\r\n");

	std::cout << RED << ref.body << RESET;

	if (iStart == std::string::npos || endOfFile == std::string::npos)
		throw ErrorHandler(ERR_400, "No separator in the file to upload");
	
	iStart += 4;
	endOfFile -= 4;
	
	
}
/*----------------------------------------------------------------------------*/

std::string	&Response::findMimeType(const std::string &uri)
{
	try {
		return _mimeMap.at(UtilParsing::recoverExtension(uri));
	}
	catch(const std::exception& e) {
		return _mimeMap.at(".bin");
	}
}
/*----------------------------------------------------------------------------*/

std::string	Response::setHeader(const Request &req, const std::string &code) throw (ErrorHandler)
{
	std::ostringstream oss;
	oss << message.length();

	if (oss.fail())
		throw ErrorHandler(ERR_500, "In Response::setHeader()\nconversion of the length message faild");

	std::string header = \
		PROTOCOL_VERION " " + code + "\r\n" \
		"Server: Rob&Flo V0.9" + "\r\n" \
		"Content-Type: " + findMimeType(req.completeUri) + "; charset=UTF-8\r\n" \
		"Content-Length: " + oss.str() + "\r\n" \
		"Connection: " + (req.keepAlive == true ? "keep-alive" : "close") + "\r\n" \
		"\r\n";

	return header;	
}
/*----------------------------------------------------------------------------*/

/*	* unterminated function
*/
bool	Response::isCGI(const Request &req) throw (ErrorHandler)
{
	return false;
	if (UtilParsing::isDirectory(req.completeUri) == true)
		return false;

	try {
		UtilParsing::checkAccessRessource(req.completeUri, R_OK);
	}
	catch(const std::exception& e)
	{
		switch (errno)
		{
			case ENOENT:
			case ELOOP:
				throw ErrorHandler(ERR_404, e.what());
			
			case EACCES:
			case ENAMETOOLONG:
			case ENOTDIR:
				throw ErrorHandler(ERR_403, e.what());
			
			default:
				throw ErrorHandler(ERR_400, e.what());
		}
	}

	// a partir d'ici verifier l'extension du fichier et renvoyer true si elle correspond a un cgi executable
	// a partir de la il n'y a plus d'erreur a gerer sur cette fonction, juste renvoyer true ou false
	size_t idx = req.completeUri.find_last_of('.');

	if (idx == req.completeUri.npos)
		return false;
	
	return false;
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
}
/*----------------------------------------------------------------------------*/
