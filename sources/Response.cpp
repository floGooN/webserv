


/*============================================================================*/
						/*### HEADERS & STATIC FIELD ###*/
/*============================================================================*/

#include "Client.hpp"
#include "Response.hpp"
#include "UtilParsing.hpp"
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
	std::cout << BRIGHT_GREEN "GET QUERY" RESET << std::endl;

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
		UtilParsing::readFile(client.request.completeUri, message);

	try {
		message.insert(0, setHeader(client.request, (message.empty() ? COD_204 : COD_200 )));
	}
	catch(const std::exception& e) {
		throw ErrorHandler(ERR_500, "in getQuery(): " + std::string(e.what()));
	}
	
}
/*----------------------------------------------------------------------------*/

void	Response::postQuery(Client &client)
{
	std::cout	<< BRIGHT_YELLOW "POST QUERY" RESET << std::endl;

	UtilParsing::checkAccessRessource(client.request.completeUri, W_OK);

	if (isCGI(client) == true) 
		message = processCGI(client);
	else
	{
		if (client.request.getbody().contentType != MULTIPART)
			throw ErrorHandler(ERR_415, "The media type is not supported by the server");
		uploadFile(client);
		client.request.completeUri = "./uploads/uploadSucces.html";
		UtilParsing::readFile(client.request.completeUri, message);
	}
	try {
		message.insert(0, setHeader(client.request, COD_201));
	}
	catch(const std::exception& e) {
		throw ErrorHandler(ERR_500, "in getQuery(): " + std::string(e.what()));
	}	
}
/*----------------------------------------------------------------------------*/

void	Response::deleteQuery(const Client &client)
{
	std::cout << BRIGHT_CYAN "DELETE QUERY" << RESET << std::endl;

	char		path[100];

    std::cout	<< BRIGHT_YELLOW << client.request.completeUri << RESET
				<< std::endl;

	if (realpath(client.request.completeUri.c_str(), path) == NULL) {
		if (errno == ENOENT)
			throw ErrorHandler(ERR_404, "File to delete not found");
		throw ErrorHandler(ERR_400, "realpath() in DELETE, invalid path");
	}
    std::cout	<< BRIGHT_RED << path << RESET 
				<< std::endl;
	if (access(path, F_OK) == -1)
		throw ErrorHandler(ERR_404, "Not found in delete()");
	else if (access(path, W_OK) == -1)
		throw ErrorHandler(ERR_403, "Acces forbidden");
    else
    {
        struct stat path_stat;
        stat(path, &path_stat);
        if (S_ISREG(path_stat.st_mode))
        {
            if (remove(path) == 0)
				throw ErrorHandler(COD_204, "No content in DELETE");
            else
				throw ErrorHandler(ERR_500, "remove() in deleteQuery()");
        }
        else
			throw ErrorHandler(ERR_403, "Forbidden removing");
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
		throw ErrorHandler(ERR_400, "The format of the request is wrong (missing filename)");
	i += 9;
	std::vector<std::string> res = UtilParsing::split(bodyHeader.substr(i, bodyHeader.find_first_of("\r\n") - i), "\"");
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
		throw ErrorHandler(ERR_400, "No separator in the file to upload");
		
	std::string bodyHeader = ref.body.substr(0, iStart);
		
	size_t	endOfFile = ref.body.find(ref.bound, iStart);

	if (endOfFile == std::string::npos)
		throw ErrorHandler(ERR_400, "No EOF delimiter in the file to upload");

	endOfFile = ref.body.find_last_of('\n', endOfFile);

	iStart += 4;
	endOfFile -= 1;
	
	std::string filename = extractFilename(bodyHeader);
	if (filename.empty() == true)
		throw ErrorHandler(ERR_400, "no file name is specified");
	
	filename.insert(0, client.request.completeUri);

	std::ofstream ss(filename.c_str(), std::ios::binary);
	if (! ss)
		throw ErrorHandler(ERR_500, "in uploadFile()");

	ss.write(ref.body.c_str() + iStart, endOfFile - iStart);
}
/*----------------------------------------------------------------------------*/

std::string	&Response::findMimeType(const std::string &uri)
{
	try {
		return _mimeMap.at(UtilParsing::recoverExtension(uri));
	}
	catch(const std::exception& e) 
	{
		return _mimeMap.at(".html");
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

std::string	Response::setHeaderRedirect(const Client &client) throw (ErrorHandler)
{
	const t_location *current = UtilParsing::findLocation(client.clientServer->getLocationSet(), client.request.getHeader().uri);
	   	if (!current)
	   		throw ErrorHandler(ERR_444);
	std::string res = PROTOCOL_VERION " " + current->redirect[0]+ " Found" + "\r\n" \
	"Server: Rob&Flo V0.9" + "\r\n" \
	"Content-Type: " + "text/html" + "; charset=UTF-8\r\n" \
	"Content-Length: 0\r\n" \
	"Connection: close\r\n" \
	"Location: " + current->redirect[1] + "\r\n" \
	"\r\n";
	std::cout << YELLOW << current->redirect[1] << RESET << std::endl;
	return res;
}
// /*----------------------------------------------------------------------------*/

// prendre les redirect pour verifier .
bool	Response::isRedirect(const Client & client)
{
	const t_location *current = UtilParsing::findLocation(client.clientServer->getLocationSet(), client.request.getHeader().uri);

	if (current && current->redirect.size() != 0)
		return true;
	return false;
}


/*----------------------------------------------------------------------------*/


bool	Response::isCGI(Client client) throw (ErrorHandler)
{
	if (checkExtensionCGI(client.request.getHeader().uri) == true)
	{
		const t_location *current = UtilParsing::findLocation(client.clientServer->getLocationSet(), client.request.getHeader().uri);
		if (current == NULL)
			throw ErrorHandler(ERR_404);
		std::string path = current->root + client.request.getHeader().uri;
		if (UtilParsing::isDirectory(path) == true)
			return false;
		if (access(path.c_str(), X_OK) != 0)
			throw ErrorHandler(ERR_403);
		return true;
	}
	return false;
}
/*----------------------------------------------------------------------------*/

bool Response::isRepository(Client client) throw (ErrorHandler)
{
	if (client.request.getHeader().uri == "/")
		return false;
	const t_location *current = UtilParsing::findLocation(client.clientServer->getLocationSet(), client.request.getHeader().uri);
	if (current == NULL)
		return false;
	std::string path = current->root + client.request.getHeader().uri;
	return UtilParsing::isDirectory(path);
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
