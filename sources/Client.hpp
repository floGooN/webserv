

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"
# include "ServerStructure.hpp"

# include "ErrorHandler.hpp"

# include "Request.hpp"
# include "Response.hpp"

class Server;

class Client
{
	public:
		Client();
		Client(const Client &);
		~Client();
		Client &operator=(const Client &);

		Server		*clientServer;
		Request		request;
		Response	response;

		void	checkRequestValidity() throw (ErrorHandler);
		void	buildResponse() throw (ErrorHandler);
		void	clearData();

	private:
		const t_location *buildCompleteUri();

		bool	isAutoindex() throw (ErrorHandler);
		void	validTheUriPath() const throw (ErrorHandler);
		void	checkUriContent() const throw (ErrorHandler);
		void	checkAutorisation(const t_location *) const throw (ErrorHandler);
};

std::ostream & operator<<(std::ostream &, const Client &);

#endif
