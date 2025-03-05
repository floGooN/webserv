





#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

# include "Request.hpp"

class Response
{
	public:
		Response();
		Response(const Response &);
		~Response();
		Response &operator=(const Response &);
		

		void	buildResponse(const Request &);
		void	clearResponse();


	private:
		// theses functions check 
		void	getRequest(const Request &);
		void	postRequest(const Request &);
		void	deleteRequest(const Request &);

		void	setHeader();
		void	setPageContent();

		std::string	_header;
		std::string	_pageContent;
};

std::ostream & operator<<(std::ostream &, const Response &);

#endif
