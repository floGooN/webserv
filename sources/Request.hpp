





#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "webserv.hpp"

# define BODY_SEPARATOR	"\r\n\r\n"
# define	PROTOCOL_VERION	"HTTP/1.1"

class Request
{
	// class   RequestException : virtual public std::exception
	// {
	// 	public:
	// 		RequestException(const std::string &msg, int errcode)
	// 			: _msg(msg), _errcode(errcode)
	// 		{	}
	// 		virtual ~RequestException() throw() {}
	// 		virtual const char *	what() const throw() {
	// 			return _msg.c_str();
	// 		};
	// 	private:
	// 		int			_errcode;
	// 		std::string	_msg;
	// };

	public:
		Request(const std::string &);
		Request(const Request &);
		~Request();
		Request() {};
		Request & operator=(const Request &);
	
		bool		getkeepalive() 		const;
		size_t		getcontentlength()	const;
		std::string	&getbody();
		const std::string	&gethostname()	const;
		const std::string	&gethostport()	const;
		const std::string	&geturi()	const;
		const std::string	&gettype()	const;
		
		void	setBody(const std::string &);
		void	clearRequest();

	private:		
		bool		_keepAlive;
		size_t		_contentLength;
		std::string	_uri;
		std::string	_hostName;
		std::string	_hostPort;
		std::string	_requestType;
		std::string	_body;


		void	initRequestLine(const std::string &);
		void	initHost(std::vector<std::string>::const_iterator &, std::vector<std::string>::const_iterator);
		void	initContentLength(const std::string &);
};
std::ostream & operator<<(std::ostream &, Request &);

#endif
