




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


const std::string _dfltErrorPath = PATH_ERRPAGE;

ErrorHandler::ErrorHandler(std::string &errCode, std::string &msg, Client &current)
  :	_errcode(errCode), _msg(msg), _client(current)
{	}
/*----------------------------------------------------------------------------*/

ErrorHandler::ErrorHandler(const ErrorHandler &ref)
  : _errcode(ref._errcode), _msg(ref._msg), _client(ref._client)
{	}
/*----------------------------------------------------------------------------*/

ErrorHandler::~ErrorHandler()
{	}
/*----------------------------------------------------------------------------*/

ErrorHandler &	ErrorHandler::operator=(const ErrorHandler &ref)
{
	_errcode = ref._errcode;
	_msg = ref._msg;
	_client = ref._client;
	
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
std::string	ErrorHandler::findErrorFile(DIR *current, const std::string &errorCode) const
{
	std::string result;

	while (current)
	{
		result = "";
		struct dirent *dirp = readdir(current);
		if (dirp)
		{
			result = dirp->d_name;
			if (result.find(errorCode) != result.npos)
				break ;
		}
		else
			break ;
	}
	UtilParsing::safeCloseDirectory(current);
	if ( errno )
		perror("readdir()");
	return result ;
}
/*----------------------------------------------------------------------------*/

/* 
*/
std::string ErrorHandler::generateContent() const
{
	std::string filename("");
	try
	{
		filename = findErrorFile(UtilParsing::openDirectory(_client.clientServer->getConfig().pageErrorPath), _errcode);
		if (filename.empty())
		{
			filename = findErrorFile(UtilParsing::openDirectory(PATH_ERRPAGE), _errcode);
			if (filename.empty())
				return std::string(DFLT_ERRORPAGE);
		}
		filename.insert(0, _client.clientServer->getConfig().pageErrorPath);
#ifdef TEST
		std::cout	<< BRIGHT_YELLOW "IN " __FILE__ " AT LINE : " << __LINE__ << std::endl
					<< "comple path to the error page : [" << filename 
					<< "]" RESET << std::endl;
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
	HTTP/1.1 404 Not Found
	Date: Tue, 04 Mar 2025 12:34:56 GMT
	Server: MyMinimalWebServer/1.0
	Content-Type: text/html; charset=UTF-8
	Content-Length: 123
	Connection: close

	HTTP/1.1 500 Internal Server Error
	Date: Tue, 04 Mar 2025 12:35:10 GMT
	Server: MyMinimalWebServer/1.0
	Content-Type: text/html; charset=UTF-8
	Content-Length: 145
	Connection: close

	HTTP/1.1 200 OK
	Date: Tue, 04 Mar 2025 12:40:00 GMT
	Server: MyMinimalWebServer/1.0
	Content-Type: text/html; charset=UTF-8
	Content-Length: 5123
	Connection: keep-alive

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

/*----------------------------------------------------------------------------*/

/*	*
*/
void	ErrorHandler::generateErrorPage() const
{
	std::string	pageContent(generateContent());
	// std::string	headerContent(generateHeader());


	// 	std::ostringstream convert;
	// 	convert << response.length();
	// response.insert(0, 
	// "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " 
	// + convert.str() + "\r\n\r\n");



	// former le header

	// concatener les deux et creer la reponse client
	// passer en mode ecriture pour renvoyer la page d'erreur au client

}
/*----------------------------------------------------------------------------*/




