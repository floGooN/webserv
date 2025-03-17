





#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

typedef struct	s_header	t_header;
typedef struct	s_body		t_body;

class Response
{
	public:
		Response();
		Response(const Response &);
		~Response();
		Response &operator=(const Response &);

		void	clearResponse();

		t_header	header;
		t_body		body;

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
