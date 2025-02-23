

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <webserv.hpp>

#include "Server.hpp"
#include <sys/wait.h>

#include <sys/types.h>
#include <unistd.h>
#include "UtilParsing.hpp"
#include "Request.hpp"

#include <sstream>
#include <fstream>
#include <iostream>


class Server;

class Client
{
	private:
		Client &operator=(const Client &);
	
	public:
		Client(const Request &);
		Client(const Client &);
		~Client();
		
		Request &getrequest();
		Request	request;
	
		Server	*clientServer;


		/*	* toutes ces variables seront remplaces par la class _request
		*/
		std::string _body;			// Contenu envoyer du client
		std::string _uri;			// PATH DEMANDER
		std::string _requestType;	// POST GET DELETE
		ServerConfig _config;		// config du server en cours

		/*
			* tout l'interet de mettre un underscore avant un nom de variable c'est de differencier facilement 
			* les variables privees et public facilement lors du devellopement ....
		*/
		std::string _response;		// renvoi final au client

		std::string _contentBody;	// body renvoyer au client
		std::string _codeResponse;	// code de la response
		std::string _contentType;	// champ obligatoire a renvoyer
		std::string _contentLength;	// taille du body renvoyer au client
		std::map<std::string, std::string> _mimeMap;	// map avec tout les mimes possible


		std::string processResponse();
		int		executeGetRequest();
		int		writeGetResponse();
		int		executeDeleteRequest();
		int		writeDeleteResponse();
		int		checkPossibilityFile();
		int		checkRequest();
		void	buildCodeResponse(int code);
		void	buildResponse();
		void	controlCodeResponse(int code);
		void	buildContentType();

		std::string buildDeleteResponse();
		std::string buildErrorPage(int code);
		std::map<std::string, std::string> initMapMime();

		// POST
		int executePostRequest();
		int save_file(const std::string& request_body);
		std::string playCGI();
		std::string extract_filename(const std::string& request_body);
};

std::ostream & operator<<(std::ostream &, Client &);

#endif

