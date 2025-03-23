





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
		void	postQuery(Client &);
		void	getQuery(const Client &);
		void	deleteQuery(const Client &);

		std::string	message;

	private:
		std::map<std::string, std::string>	_mimeMap;

		void		initMimeMap();
		void		uploadFile(const Client &) throw (ErrorHandler);
		bool		isCGI(const Request &) throw (ErrorHandler);
		std::string	&findMimeType(const std::string &uri);
		std::string extractFilename(const std::string &) throw (ErrorHandler);
		std::string	setHeader(const Request &, const std::string &) throw (ErrorHandler);
};

std::ostream & operator<<(std::ostream &, const Response &);

#endif
