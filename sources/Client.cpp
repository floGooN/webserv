




#include "Client.hpp"

Client::Client(const Request &req) {
	request = req;
	clientServer = NULL;
}

Client::Client(const Client &ref) {
	*this = ref;
}

Client::~Client()
{	}

Client &Client::operator=(const Client &ref) {
	request = ref.request;
	clientServer = ref.clientServer;

	return *this;
}

std::ostream & operator<<(std::ostream &o, Client &ref)
{
	o	<< "CLIENT:\n"
		<< ref.getrequest()
		<< ref._response;
	return o;
}

/*
	-------------LIRE AVANT TOUT CHANGEMENT -----------------------------
	l'idee ca va etre que la on a le body de chaque Method
	maintenant il va falloir rajouter le header a chaque response 
	le body sera enregistrer dans _contentBody avant d'etre transferer a la valeur response
	ne pas prendre en compte les noms des variables utiliser dans le code ci-dessous 
	il va etre changer au moment ou nos deux codes seront liees
	----------------------------------------------------------------------
*/

Request & Client::getrequest(){
	return request;
}

/* 
	* pourquoi retourner response alors que c'est deja un membre de la classe ?
	* (cela fait une copie potentiellement longue a realiser et qui me parait inutile (mais peut etre pas) ?)
*/
std::string Client::processResponse()
{
	int code;
	if (_mimeMap.empty())
		_mimeMap = initMapMime();
	code = checkRequest();
	buildContentType();
	buildCodeResponse(code);
	controlCodeResponse(code);
	buildResponse();
	return _response;
}

int Client::checkRequest() 
{
	if (_response.find("POST") != std::string::npos)
		return executePostRequest();
	else if (_response.find("GET") != std::string::npos)
	{
		 return executeGetRequest();
	}
	else if (_response.find("DELETE") != std::string::npos)
	{
		return executeDeleteRequest();
	}
	else
	{
		return 400; // Bad Request
	}
}

// le body de la requete POST sera different donc a voir pour le script cgi

int Client::executeGetRequest()
{
	if (requestType.find("GET") != std::string::npos)
	{
		if (UtilParsing::fileExits(_uri))
		{
			return writeGetResponse(); // la je rempli le futur body mais il faut remplir les en-tete
		}
		else
			return 400;
	}
	return 404;
}

int Client::writeGetResponse()
{
	if (_uri.find("..") != std::string::npos)
		return 404; // 404 == page not found : 403 == forbidden path
	
	std::ifstream file(_uri.c_str());
	std::stringstream buffer;

	buffer << file.rdbuf();
	_contentBody = buffer.str();
	if (_contentBody.empty())
		return 404;
	return 200;
}

int Client::executeDeleteRequest()
{
	if (requestType.find("DELETE") != std::string::npos) // 
	{
		if (UtilParsing::fileExits(_uri))
		{
			if (checkPossibilityFile() == 0)
			   if (writeDeleteResponse() == 0)
					return 200;
		}
		else
			return 400;
	}
	return 404;
}

int Client::writeDeleteResponse()
{
	if (remove(_uri.c_str()) == 0)
	{
		_contentBody = buildDeleteResponse();
		return 0;
	}
	else
		return 1;
}

std::string Client::buildDeleteResponse()
{
	std::string output;

	output = "<!DOCTYPE html>\n<html>\n<head><title>Page Title</title></head>\n<body><h1>Suppression de " + _uri + " effectué</h1></body>\n</html>";
	return output;
}

int Client::checkPossibilityFile()
{
	if (_uri.find("upload/") != std::string::npos)
	{
		if (_uri.find("..") == std::string::npos) // ici on verifie qu'il n'y a pas de ..
		{
			return 0;
		}
	}
	return 1;
}

// La derniere fonction pour la renvoyer la response complete pour le server
// elle ne sera fonctionnel seulement apres la liaison des codes 
void Client::buildResponse()
{
	int sizeBody = _contentBody.length();
	_contentLength = UtilParsing::intToString(sizeBody);
	_response = "HTTP/1.1"  + _codeResponse + "\nContent-Type: " + _contentType + "\nContent-Length: " + _contentLength + "\r\n"
				+ _contentBody;
}

// faire un bodycontent pour chaque code erreur possible

void Client::buildCodeResponse(int code)
{
	switch (code)
	{
		case 400:
			_codeResponse = "400 Bad Request";
			break;
		case 401:
			_codeResponse = "401 Unauthorized";
			break;
		case 403:
			_codeResponse = "403 Forbidden";
			break;
		case 404:
			_codeResponse = "404 Not Found";
			break;
		case 405:
			_codeResponse = "405 Method Not Allowed";
			break;
		case 408:
			_codeResponse = "408 Request Timeout";
			break;
		case 500:
			_codeResponse = "500 Internal Server Error";
			break;
		case 501:
			_codeResponse = "501 Not Implemented";
			break;
		case 502:
			_codeResponse = "502 bad Gateway";
			break;
		case 503:
			_codeResponse = "503 Service Unavailable";
			break;
		case 200:
			_codeResponse = "200 OK";
			break;
		case 201:
			_codeResponse = "201 Created";
			break;
		case 202:
			_codeResponse = "202 Accepted";
			break;
		default:
			_codeResponse = "204 No Content";
			break;
	}
}

void Client::controlCodeResponse(int code)
{
	if (code != 200)
	{
		_contentBody = buildErrorPage(code);
	}
}

std::string Client::buildErrorPage(int code)
{
	if (_config._errorPath != "none")
	{
		std::string filePath = _config._errorPath + UtilParsing::intToString(code) + "error.html";
		std::ifstream file(filePath.c_str());
		if (!file)
		{
			std::cerr << "Erreur ouverture file error code" << std::endl; // pour debug
			return "";
		}
		std::string content;
		std::string line;
		while (std::getline(file, line))
		{
			content += line + '\n';
		}
		file.close();
		return content;
	}
	else
	{
		std::string filePathDefault = "./error_path/" + UtilParsing::intToString(code) + "error.html";
		std::ifstream fileDefault(filePathDefault.c_str());
		if (!fileDefault)
		{
			std::cerr << "Erreur ouverture file error code" << std::endl; // pour debug
			return "";
		}
		std::string content;
		std::string line;
		while (std::getline(fileDefault, line))
		{
			content += line + '\n';
		}
		fileDefault.close();
		return content;
	}

}

int Client::executePostRequest()
{
	if (requestType.find("POST") && _uri.find("/cgi-bin"))
	{
		if (UtilParsing::fileExits(_uri))
		{
			_contentBody = playCGI();
			if (_contentBody.empty())
				return 500;
			return 200;
		}
		else
			return 500;
	}
	else if (requestType.find("POST") && _uri.find(_config._uploadPath) != std::string::npos)
	{
		if (UtilParsing::directoryExists(_config._uploadPath))
		{
			return save_file(_body);
		}
	}
	else
	{
	   return 400; // la on retourne bad request car notre server ne supprote que ces deux type de POST. 
	}
	return 500;
}

std::string Client::playCGI()
{
	std::string output;
	int pipfd[2];
	if (pipe(pipfd) == -1)
	{
		std::cerr << "Error pipe CGI" << std::endl; // juste pour futur debug
		return "";
	}
	pid_t pid = fork();
	if (pid < 0)
	{
		std::cerr << "fork failled" << std::endl; // juste pour futur debug
		return "";
	}
	else if (pid == 0) 
	{
		std::cerr << "HERE" << std::endl;
		close(pipfd[0]);
		dup2(pipfd[1], STDOUT_FILENO); 
		close(pipfd[1]);
		char* const args[] = {const_cast<char*>(/* _uri.c_str() */"./cgi-bin/script.pl"), NULL};
		if (execv(args[0], args) == -1)  
		{
			std::cerr << "execv failed" << std::endl; // juste pour futur debug
			return "";
		}
	} 
	else 
	{
		std::cerr << "HERE" << std::endl;
		close(pipfd[1]);
		char buffer[128];
		ssize_t bytesRead;
		while ((bytesRead = read(pipfd[0], buffer, sizeof(buffer) - 1)) > 0)
		{
			buffer[bytesRead] = '\0';
			output.append(buffer);
		}
		close(pipfd[0]);
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			return output;
		else 
		{
			std::cout << "CGI script did not terminate normally" << std::endl;
		}
	}
	return output;
}

std::string Client::extract_filename(const std::string& request_body) // a voir si cette fonction peut etre utile ailleurs. Donc pas que la mettre dans POST
{
	std::string filename;
	std::string key = "filename=\"";

	size_t pos = request_body.find(key);
	if (pos != std::string::npos) 
	{
		pos += key.length();
		size_t end_pos = request_body.find("\"", pos);
		if (end_pos != std::string::npos) 
		{
			filename = request_body.substr(pos, end_pos - pos);
		}
	}
	if (filename.find("..") != std::string::npos) // rajout car si on a .. en nom de fichier ca peut faire de la merde et le telecharger ailleurs
	{
		return "";
	}
	return filename;
}

int Client::save_file(const std::string& request_body) 
{
	std::string filename = extract_filename(request_body);
	if (filename.empty()) 
	{
		return 404;
	}
	std::string boundary = "------WebKitFormBoundary";
	size_t file_start = request_body.find("\r\n\r\n");
	if (file_start == std::string::npos) 
	{
		return 404;
	}
	file_start += 4;
	size_t file_end = request_body.rfind("--" + boundary);
	if (file_end == std::string::npos) 
	{ 
		return 413; // taille max (fichier trop gros)
	}
	std::string file_data = request_body.substr(file_start, file_end - file_start);
	std::string file_path = "upload/" + filename;
	std::ofstream output(file_path.c_str(), std::ios::binary); // le ios binary permet de pas alterer le contenu donc gerer meme les images etc 
	if (!output) 
	{
		return 500; // la erreur au niveau du server on donne pas plus d'info qu'une erreur 500 
	}
	output.write(file_data.c_str(), file_data.size());
	output.close();
	return 200; // tout est ok
}

std::map<std::string, std::string> Client::initMapMime()
{
	std::map<std::string, std::string> fileMime;
	fileMime[".aac"] = "audio/acc";
	fileMime[".abw"] = "application/x-abiword";
	fileMime[".apng"] = "image/apng";
	fileMime[".arc"] = "application/x-freearc";
	fileMime[".avif"] = "image/avif";
	fileMime[".avi"] = "video/x-msvideo";
	fileMime[".csh"] = "application/x-csh";
	fileMime[".css"] = "text/css";
	fileMime[".csv"] = "text/csv";
	fileMime[".gif"] = "image/gif";
	fileMime[".html"] = "text/html";
	fileMime[".htm"] = "text/html";
	fileMime[".ico"] = "image/vnd.microsoft.icon";
	fileMime[".jpeg"] = "image/jpeg";
	fileMime[".jpg"] = "image/jpeg";
	fileMime[".js"] = "text/javascript";
	fileMime[".json"] = "application/json";
	fileMime[".jsonld"] = "application/ld+json";
	fileMime[".mjs"] = "text/javascript";
	fileMime[".mp3"] = "audio/mpeg";
	fileMime[".mp4"] = "video/mp4";
	fileMime[".png"] = "image/png";
	fileMime[".pdf"] = "application/pdf";
	fileMime[".php"] = "application/x-httpd-php";
	fileMime[".sh"] = "application/x-sh";
	fileMime[".svg"] = "image/svg+xml";
	fileMime[".tar"] = "application/x-tar";
	fileMime[".txt"] = "text/plain";
	fileMime[".webp"] = "image/webp";
	fileMime[".xhtml"] = "application/xhtml+xml";
	fileMime[".xml"] = "application/xml";
	fileMime[".zip"] = "application/zip";
	fileMime[".xul"] = "application/vnd.mozilla.xul+xml";
	fileMime[".3gp"] = "video/3gpp";
	fileMime[".7z"] = "application/x-7z-compressed";
	fileMime[".bin"] = "application/octet-stream";
	return fileMime;
}

void Client::buildContentType()
{
	std::string extension;
	extension = UtilParsing::recoverExtension(request.geturi());
	std::map<std::string, std::string>::iterator it = _mimeMap.find(extension);
	if (it != _mimeMap.end())
		_contentType = it->second;
	else
		_contentType = "application/octet-stream";
}
