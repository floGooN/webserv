


#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "webserv.hpp"
# include "RequestStructure.hpp"

# include "ErrorHandler.hpp"


class Cluster;

class Request
{
	public:
		Request();
		Request(const std::string &) throw(ErrorHandler);
		Request(const Request &);
		~Request();
		Request & operator=(const Request &);
		
		const std::string	&getArgs()		const;
		const s_header		&getHeader()	const;
		const s_body		&getbody()		const;
		
		void	clearRequest();
		void	updateRequest(const std::string &) throw(ErrorHandler);
		void	updateRequest(const Request &req) throw(ErrorHandler);
	
		bool		keepAlive;
		size_t		totalBytesReceived;
		std::string	completeUri;

	private:
		t_body		_body;
		t_header	_header;	
		std::string	_args;

		void	setArgs()				 				throw(ErrorHandler);
		void 	setUri(const std::string &)		 		throw(ErrorHandler);
		void	setBody(const std::string &)			throw(ErrorHandler);
		void 	setHost(const std::string &) 			throw(ErrorHandler);
		void 	setKeepAlive(const std::string &);
		void 	setContentType(const std::string &) 	throw(ErrorHandler);
		void	checkProtocole(const std::string &) 	throw(ErrorHandler);
		void 	setRequestType(const std::string &) 	throw(ErrorHandler);
		void	setHeader(const std::string &header)	throw(ErrorHandler);
		void	setBoundLimiter(const std::string &)	throw(ErrorHandler);
		void 	setContentLength(const std::string &)	throw(ErrorHandler);
		void 	addHeaderField(const std::string &line);
};

std::ostream & operator<<(std::ostream &, const Request &);

#endif
