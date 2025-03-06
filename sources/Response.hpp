





#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"
# include "Request.hpp"

class Client;

class Response
{
	public:
		Response();
		Response(const Response &);
		~Response();
		Response &operator=(const Response &);

		void	clearResponse();

		std::string	completeUri;
		std::string	finalMessage;
	
	private:
		// theses functions check 
		void	getRequest(const Request &);
		void	postRequest(const Request &);
		void	deleteRequest(const Request &);
};

std::ostream & operator<<(std::ostream &, const Response &);

#endif
