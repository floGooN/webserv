




/*============================================================================*/
							/*### HEADER FILES ###*/
/*============================================================================*/

#include "ErrorHandler.hpp"
#include "UtilParsing.hpp"
#include "Client.hpp"
#include "Server.hpp"

/*============================================================================*/
			/*### CONSTRUCTORS - DESTRUCTOR _ OVERLOAD OPERATORS ###*/
/*============================================================================*/

ErrorHandler::ErrorHandler(Client &client, std::string errorKey, std::string errlog = "")
  :	_client(client), _errorLog(errlog)
{
	_errorKey = errorKey;
	if (errorKey.empty() == true)
		_errorKey = ERR_520;
}
/*----------------------------------------------------------------------------*/

ErrorHandler::ErrorHandler(const ErrorHandler &ref)
  : _client(ref._client), _errorKey(ref._errorKey), _errorLog(ref._errorLog)
{	}
/*----------------------------------------------------------------------------*/

ErrorHandler::~ErrorHandler() throw()
{	}
/*----------------------------------------------------------------------------*/

ErrorHandler &	ErrorHandler::operator=(const ErrorHandler &ref) {
	if (this != &ref)
	{
		_client = ref._client;
		_errorKey = ref._errorKey;
		_errorLog = ref._errorLog;
	}
	return *this;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

/*	* return the complete and correct file name or an empty str if nothing is found
*/
std::string	ErrorHandler::findErrorFile(DIR *current, const std::string &errorKey) const
{
	std::string result = "";
	std::string codeNumber = errorKey.substr(0, errorKey.find_first_of(" "));
	while (current)
	{
		struct dirent *dirp = readdir(current);
		if (dirp)
		{
			result = dirp->d_name;
			if (result.find(codeNumber) != result.npos) {
				break ;
			}
			result = "";
		}
		else
			break ;
	}
	UtilParsing::safeCloseDirectory(current);
	if ( result.empty() && errno )
		perror("readdir()");
	return result ;
}
/*----------------------------------------------------------------------------*/

std::string ErrorHandler::generateContent() const
{
	try
	{
		std::string filename("");
		std::string	dirErrorPath = _client.clientServer->getConfig().pageErrorPath;
		filename = findErrorFile(UtilParsing::openDirectory(dirErrorPath), _errorKey);
		if (filename.empty())
		{
			filename = findErrorFile(UtilParsing::openDirectory(PATH_ERRPAGE), _errorKey);
			if (filename.empty())
				return std::string(DFLT_ERRORPAGE);
		}
		if (filename[0] != '/' && dirErrorPath[dirErrorPath.size() - 1] != '/') {
			filename.insert(0, "/");
		}
		else if (filename[0] == '/' && dirErrorPath[dirErrorPath.size() - 1] == '/') {
			filename.erase(0, 1);
		}
		filename.insert(0, dirErrorPath);
#ifdef TEST
		// std::cout	<< BRIGHT_YELLOW "IN " __FILE__ " AT LINE : " << __LINE__ << std::endl
		// 			<< "comple path to the error page : [" << filename 
		// 			<< "]" RESET << std::endl;
#endif
		return UtilParsing::readFile(filename);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return std::string(DFLT_ERRORPAGE);
	}
}
/*----------------------------------------------------------------------------*/

/*	* exemple header

	HTTP/1.1 201 Created
	Date: Tue, 04 Mar 2025 12:41:10 GMT
	Server: MyMinimalWebServer/1.0
	Content-Type: application/json
	Content-Length: 45
	Connection: keep-alive
	Location: /uploads/newfile.txt
	
	Utilisé quand une action a réussi mais qu'il n'y a rien à retourner 
	(ex: suppression d'un fichier avec DELETE).
	HTTP/1.1 204 No Content
	Date: Tue, 04 Mar 2025 12:42:00 GMT
	Server: MyMinimalWebServer/1.0
	Connection: close
	*/
std::string	ErrorHandler::generateHeader() const
{
	std::string length = UtilParsing::intToString(static_cast<int>( _client.response.finalMessage.length() ));
	std::string final =	PROTOCOL_VERION + _errorKey + HTTP_SEPARATOR \
						"Date: TODAY" HTTP_SEPARATOR \
						"Server: Rob_&_Flo__WEBSERV42__/0.5" HTTP_SEPARATOR \
						"Content-Type: text/html; charset=UTF-8" HTTP_SEPARATOR \
						"Content-Length: " + length + HTTP_SEPARATOR \
						"Connection: close" \
						HTTP_SEPARATOR \
						HTTP_SEPARATOR;
	return final;
}
/*----------------------------------------------------------------------------*/

void	ErrorHandler::generateErrorPage()
{
	// tout est dans un try catch pour que le serveur ne crash pas
	try
	{
		_client.response.finalMessage = generateContent();
		if (_client.response.finalMessage.empty())
			_client.response.finalMessage = DFLT_ERRORPAGE;
		_client.response.finalMessage.insert(0, generateHeader());
		_client.request.keepAlive = false;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		_client.response.finalMessage = generateHeader() + DFLT_ERRORPAGE;
	}
}
/*----------------------------------------------------------------------------*/




