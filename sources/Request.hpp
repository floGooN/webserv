


#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "webserv.hpp"
# include "RequestStructure.hpp"

class Cluster;

class Request
{
	public:
		Request(const std::string &) throw(ErrorHandler);
		Request(const Request &);
		~Request();
		Request & operator=(const Request &);
		
		const std::string	&getArgs()		const;
		const s_header		&getHeader()	const;
		const s_body		&getbody()		const;
		
		void	clearRequest();
		void	updateRequest(const std::string &) throw(ErrorHandler);

		size_t	totalBytessended;
		bool	keepAlive;

	private:
		std::string	_args;
		s_header	_header;	
		s_body		_body;

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
};

std::ostream & operator<<(std::ostream &, Request &);

#endif
