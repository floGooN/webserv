




/*============================================================================*/
							/*### HEADER FILES ###*/
/*============================================================================*/
#include "Cluster.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "UtilParsing.hpp"

#include "ErrGenerator.hpp"

/*============================================================================*/
			/*### CONSTRUCTORS - DESTRUCTOR _ OVERLOAD OPERATORS ###*/
/*============================================================================*/

Cluster::ErrGenerator::ErrGenerator(Client& client, const std::string &errCode = ERR_520, const std::string & errlog = "")
:	_client(client), _errorCode(errCode), _errorLog(errlog)
{	}
/*----------------------------------------------------------------------------*/

Cluster::ErrGenerator::ErrGenerator(const ErrGenerator &ref)
:	_client(ref._client), _errorCode(ref._errorCode), _errorLog(ref._errorLog)
{	}
/*----------------------------------------------------------------------------*/

Cluster::ErrGenerator::~ErrGenerator() throw()
{	}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

void	Cluster::ErrGenerator::generateErrorPage()
{
	// tout est dans un try catch pour que le serveur ne crash pas
	try
	{
		generateContent(_client.response.message);
		if (_client.response.message.empty()) {
			_client.response.message = DFLT_ERRORPAGE;
		}
		_client.response.message.insert(0, generateHeader());
		_client.request.keepAlive = false;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		_client.response.message = generateHeader() + DFLT_ERRORPAGE;
	}

}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

/*	* return the complete and correct file name or an empty str if nothing is found
*/
std::string	Cluster::ErrGenerator::findErrorFile(DIR *current, const std::string &errorKey) const
{
	std::string result("");
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
	if ( result.empty() && errno ) {
		perror("readdir()");
	}

	return result ;
}
/*----------------------------------------------------------------------------*/

void Cluster::ErrGenerator::generateContent(std::string &message) const
{
	try
	{
		std::string filename("");
		std::string	dirErrorPath = _client.clientServer->getParams().errorPath;

		filename = findErrorFile(UtilParsing::openDirectory(dirErrorPath), _errorCode);
		if (filename.empty())
		{
			filename = findErrorFile(UtilParsing::openDirectory(PATH_ERRPAGE), _errorCode);
			if (filename.empty())
				message = DFLT_ERRORPAGE;
		}
		if (filename[0] != '/' && dirErrorPath[dirErrorPath.size() - 1] != '/') {
			filename.insert(0, "/");
		}
		else if (filename[0] == '/' && dirErrorPath[dirErrorPath.size() - 1] == '/') {
			filename.erase(0, 1);
		}
		filename.insert(0, dirErrorPath);
		UtilParsing::readErrorFile(filename, message);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		message = DFLT_ERRORPAGE;
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
std::string	Cluster::ErrGenerator::generateHeader() const
{
	std::string length = UtilParsing::intToString(static_cast<int>( _client.response.message.length() ));
	std::string final =	PROTOCOL_VERION " " + _errorCode + HTTP_SEPARATOR \
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
