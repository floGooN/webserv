

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"

# include "Server.hpp"
# include "Request.hpp"

class Server;

struct s_response 
{
	/* data */
};

typedef	struct s_response t_response;

class Client
{
	public:
		Client(const Request &);
		Client(const Client &);
		~Client();
		Client &operator=(const Client &);
		
		Server		*clientServer;
		Request		request;
		
		
		// std::string response;
		
		void	responseFormating(); // call in recvData()
		t_response	response;

	private:
		void	initMimeMap();
		
		void	handlePostRequest();

		void	buildUri(const std::string &);

		static std::map<std::string, std::string>	_mimeMap;

		std::string _header;
};

std::ostream & operator<<(std::ostream &, const Client &);

#endif
