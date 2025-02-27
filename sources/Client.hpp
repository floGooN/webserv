

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"

# include "Server.hpp"
# include "Request.hpp"

class Server;

class Client
{
	public:
		Client(const Request &);
		Client(const Client &);
		~Client();
		
		std::map<std::string, std::string> &getMime();
		Server		*clientServer;
		Request		request;
		std::string response;
		
		void	responseFormating(int);

	private:
		Client &operator=(const Client &);
		
		void	initMimeMap();
		void	checkUriValidity();
		void	initHeader();
		
		std::string	pathToRessource;
		static std::map<std::string, std::string>	_mimeMap;

		std::string _header;
};

std::ostream & operator<<(std::ostream &, const Client &);

#endif
