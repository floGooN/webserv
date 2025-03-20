





#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

class Request;

class Response
{
	public:
		Response();
		Response(const Response &);
		~Response();
		Response &operator=(const Response &);

		size_t		totalBytesSended;

		void	clearResponse();
		void	getQuery(const Request &);
		void	postQuery(const Request &);
		void	deleteQuery(const Request &);

		const std::string							&getMessage()	const;
		const std::map<std::string, std::string>	&getHeader()	const;

	private:
		std::string							_message;
		std::map<std::string, std::string>	_header;
		std::map<std::string, std::string>	_mimeMap;

		void		initMimeMap();
		bool		isCGI(const Request &);
		void		setHeader(const Request &);
		std::string	&findMimeType(const std::string &uri);
};

std::ostream & operator<<(std::ostream &, const Response &);

#endif
