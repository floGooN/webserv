




/*============================================================================*/
						/*### HEADERS & STATIC FIELD ###*/
/*============================================================================*/

#include "Client.hpp"

std::map<std::string, std::string> Client::_mimeMap;

/*============================================================================*/
				/*### CONSTRUCTORS - DESTRUCTOR - OVERLOAD OP ###*/
/*============================================================================*/

Client::Client(const Request &req) {
	request = req;
	clientServer = NULL;
	response.clear();
}
/*----------------------------------------------------------------------------*/

Client::Client(const Client &ref) {
	*this = ref;
}
/*----------------------------------------------------------------------------*/

Client::~Client()
{	}
/*----------------------------------------------------------------------------*/

Client &Client::operator=(const Client &ref)
{
	if (this != &ref)
	{
		request = ref.request;
		clientServer = ref.clientServer;
		response = ref.response;
	}
	return *this;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/


/*  * Formating header and body for client
    * 
*/
void	Client::responseFormating(int fdClient)
{   
    // comment se formatte la reponse client ?
    // concatener le chemins correctement suivant l'url et le serveur qui recoit la requete
    // verifier que toutes les conditions sont bonnes
        // dossier et fichiers existants ?
        // droits des requetes (POST ETC)
        // est ce qu'il y a des cgi

    
    // formatter header http
    // dans un try catch pour formatter le header une fois le serveur trouve

    // gestion requet favico
    // toujours matter a la racine du site courrant si il en a un 
    // sinon renvoyer le favico par defaut
	initMimeMap();
	std::cout	 << "IN formatResponse():" << std::endl
                << "clientFd: [" << fdClient << "]" << std::endl
                << "clientRequest: " << std::endl
				<< this->request << std::endl
                << "clientserver" << std::endl
                << *this->clientServer << std::endl;
    if (request.geturi().find("favicon") != std::string::npos)
        response = "";
}
/*----------------------------------------------------------------------------*/

/*	* doit mettre a jour suivant le fichier du serveur
*/
void Client::initMimeMap() {
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

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

void	Client::initHeader()
{

}
/*----------------------------------------------------------------------------*/

























// std::string Client::playCGI()
// {
// 	std::string output;
// 	int pipfd[2];
// 	if (pipe(pipfd) == -1)
// 	{
// 		std::cerr << "Error pipe CGI" << std::endl; // juste pour futur debug
// 		return "";
// 	}
// 	pid_t pid = fork();
// 	if (pid < 0)
// 	{
// 		std::cerr << "fork failled" << std::endl; // juste pour futur debug
// 		return "";
// 	}
// 	else if (pid == 0) 
// 	{
// 		std::cerr << "HERE" << std::endl;
// 		close(pipfd[0]);
// 		dup2(pipfd[1], STDOUT_FILENO); 
// 		close(pipfd[1]);
// 		char* const args[] = {const_cast<char*>(/* _uri.c_str() */"./cgi-bin/script.pl"), NULL};
// 		if (execv(args[0], args) == -1)  
// 		{
// 			std::cerr << "execv failed" << std::endl; // juste pour futur debug
// 			return "";
// 		}
// 	} 
// 	else 
// 	{
// 		std::cerr << "HERE" << std::endl;
// 		close(pipfd[1]);
// 		char buffer[128];
// 		ssize_t bytesRead;
// 		while ((bytesRead = read(pipfd[0], buffer, sizeof(buffer) - 1)) > 0)
// 		{
// 			buffer[bytesRead] = '\0';
// 			output.append(buffer);
// 		}
// 		close(pipfd[0]);
// 		int status;
// 		waitpid(pid, &status, 0);
// 		if (WIFEXITED(status))
// 			return output;
// 		else 
// 		{
// 			std::cout << "CGI script did not terminate normally" << std::endl;
// 		}
// 	}
// 	return output;
// }
// /*----------------------------------------------------------------------------*/



// /*============================================================================*/
// 							/*### EXCEPTIONS ###*/
// /*============================================================================*/

// /*----------------------------------------------------------------------------*/
