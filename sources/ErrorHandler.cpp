




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


int	ErrorHandler::findErrorFile(DIR *current, const std::string &errorCode) const
{
	struct dirent *dirp = readdir(current);
	if ( ! dirp )
		perror("readdir()");
	while (current)
	{
		;		
	}
	return -1;
}

void	ErrorHandler::generateErrorPage() const
{
	// trouver le bon fichier d'erreur
	// recuperer sont contenu
	
	//chercher le bon fichier dans le dossier
	int fd = findErrorFile(UtilParsing::openDirectory(_client.clientServer->getConfig().pageErrorPath), _errcode);


	
	if (_client.clientServer)
	{

	}

	// former le header
	// 	std::ostringstream convert;
	// 	convert << response.length();
	// response.insert(0, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + convert.str() + "\r\n\r\n");

	// concatener les deux et creer la reponse client
	// passer en mode ecriture pour renvoyer la page d'erreur au client

}
/*----------------------------------------------------------------------------*/




