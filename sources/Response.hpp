





#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

/*
	HTTP/1.1 404 Not Found
	Date: Tue, 04 Mar 2025 12:34:56 GMT
	Server: MyMinimalWebServer/1.0
	Content-Type: text/html; charset=UTF-8
	Content-Length: 123
	Connection: close
*/



class Response
{
	public:
		Response();
		Response(const Response &);
		~Response();
		Response &operator=(const Response &);

		void	clearResponse();
		void	getRequest(const Request &);
		void	postRequest(const Request &);
		void	deleteRequest(const Request &);

		std::string	body;
		std::string header;
		std::string	statusLine;

	private:
		// theses functions check 
};

std::ostream & operator<<(std::ostream &, const Response &);

#endif
