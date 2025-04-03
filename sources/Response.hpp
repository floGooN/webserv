





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
		void	getQuery(Client &);
		void	deleteQuery(const Client &);

		std::string	message;

	private:
		std::map<std::string, std::string>	_mimeMap;

		void		initMimeMap();
		void		uploadFile(const Client &) throw (ErrorHandler);
		bool		isCGI(Client client) throw (ErrorHandler);
		bool 		isRepository(Client client) throw (ErrorHandler);
		bool		isRedirect(const Client & client);
		std::string	&findMimeType(const std::string &uri);
		std::string	extractFilename(const std::string &bodyHeader) throw (ErrorHandler);
		std::string	setHeader(const Request &, const std::string &) throw (ErrorHandler);
		std::string	setHeaderRedirect(const Client &client, const Request &req) throw (ErrorHandler);
};

std::ostream & operator<<(std::ostream &, const Response &);

#endif
