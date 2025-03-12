





#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "webserv.hpp"
# include "RequestStructure.hpp"

# define	BODY_SEPARATOR	"\r\n\r\n"
# define	PROTOCOL_VERION	"HTTP/1.1"


class Cluster;

class Request
{
	public:
		Request(const std::string &) throw(ErrorHandler);
		Request(const Request &);
		~Request();
		Request & operator=(const Request &);

		Request(const Cluster &);
		
		const s_header	&getHeader()	const;
		const s_body	&getbody()		const;

		void	updateRequest(const std::string &) throw(ErrorHandler);
		void	clearRequest();

		size_t	totalBytessended;
		bool	keepAlive;

	private:
		s_header	_header;	
		s_body		_body;

		void	setHeader(const std::string &header) throw(ErrorHandler);
		
		void	setBody(const std::string &)				throw(ErrorHandler);
		void	setContentType() throw(ErrorHandler);
		void	setBoundLimiter()							throw(ErrorHandler);
		

};

std::ostream & operator<<(std::ostream &, Request &);

#endif
