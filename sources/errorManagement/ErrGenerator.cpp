


/*============================================================================*/
							/*### HEADER FILES ###*/
/*============================================================================*/

#include "Client.hpp"
#include "Cluster.hpp"
#include "Utils.hpp"

/*============================================================================*/
			/*### CONSTRUCTORS - DESTRUCTOR _ OVERLOAD OPERATORS ###*/
/*============================================================================*/

ErrGenerator::ErrGenerator(Client& client, const std::string &errCode = ERR_520, const std::string & errlog = "")
:	_client(client), _errorCode(errCode), _errorLog(errlog)
{	}
/*----------------------------------------------------------------------------*/

ErrGenerator::ErrGenerator(const ErrGenerator &ref)
:	_client(ref._client), _errorCode(ref._errorCode), _errorLog(ref._errorLog)
{	}
/*----------------------------------------------------------------------------*/

ErrGenerator::~ErrGenerator() throw()
{	}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

void	ErrGenerator::generateErrorPage()
{
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
		Utils::printLog(ERROR, e.what());
		_client.response.message = generateHeader() + DFLT_ERRORPAGE;
	}
	if ( ! _errorLog.empty() )
		Utils::printLog(ERROR, this->_errorLog);
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

/*	* return the complete and correct file name or an empty str if nothing is found
*/
std::string	ErrGenerator::findErrorFile(DIR *current, const std::string &errorKey) const
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

	Utils::safeCloseDirectory(current);
	if ( result.empty() && errno && errno != ENOENT && errno != ENOTDIR && errno != EISDIR)
		Utils::printLog(ERROR, "readdir(): " + std::string(strerror(errno)));

	return result ;
}
/*----------------------------------------------------------------------------*/

void ErrGenerator::generateContent(std::string &message) const
{
	try
	{
		std::string filename("");
		std::string	dirErrorPath("");

		if (_client.clientServer != NULL)
			dirErrorPath = _client.clientServer->getParams().errorPath;
		else
			dirErrorPath = PATH_ERRPAGE;

		filename = findErrorFile(Utils::openDirectory(dirErrorPath), _errorCode);
		if (filename.empty())
		{
			filename = findErrorFile(Utils::openDirectory(PATH_ERRPAGE), _errorCode);
			dirErrorPath = PATH_ERRPAGE;
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
		Utils::readErrorFile(filename, message);
	}
	catch(const std::exception& e)
	{
		Utils::printLog(ERROR, e.what());
		message = DFLT_ERRORPAGE;
	}
}
/*----------------------------------------------------------------------------*/

std::string	ErrGenerator::generateHeader() const
{
	std::ostringstream oss;
	oss << _client.response.message.length();

	if (oss.fail())
		throw ErrorHandler(ERR_500, "In ErrGenerator::generateHeader()\nconversion of the length of the message faild\n");

	std::string header =	PROTOCOL_VERION " " + _errorCode + HTTP_SEPARATOR \
							"Date: TODAY" HTTP_SEPARATOR \
							"Server: Rob_&_Flo__WEBSERV42__/0.5" HTTP_SEPARATOR \
							"Content-Type: text/html; charset=UTF-8" HTTP_SEPARATOR \
							"Content-Length: " + oss.str() + HTTP_SEPARATOR \
							"Connection: close" \
							HTTP_SEPARATOR \
							HTTP_SEPARATOR;
	return header;
}
/*----------------------------------------------------------------------------*/
