





#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"
# include "ErrorHandler.hpp"


class Request;
class Client;

class Response
{
	public:
		Response();
		Response(const Response &);
		~Response();
		Response &operator=(const Response &);

		size_t		totalBytesSended;

		void	clearResponse();
		void	getQuery(const Client &);
		void	postQuery(const Client &);
		void	deleteQuery(const Client &);

		std::string	message;

	private:
		std::map<std::string, std::string>	_mimeMap;

		void		initMimeMap();
		bool		isCGI(const Request &) throw (ErrorHandler);
		std::string	&findMimeType(const std::string &uri);
		std::string	setHeader(const Request &) throw (ErrorHandler);
};

std::ostream & operator<<(std::ostream &, const Response &);

#endif
