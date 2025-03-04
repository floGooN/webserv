

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"

# include "Server.hpp"
# include "Request.hpp"
# include "Response.hpp"

class Server;

class Client
{
	public:
		Client(const Request &);
		Client(const Client &);
		~Client();
		Client &operator=(const Client &);
		
		Server		*clientServer;
		Request		request;
		Response	response;

		void	clearData();
		
	private:
		void	initMimeMap();
		static std::map<std::string, std::string>	_mimeMap;
};

std::ostream & operator<<(std::ostream &, const Client &);

#endif
