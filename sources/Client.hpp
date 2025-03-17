

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"
# include "ServerStructure.hpp"

# include "Request.hpp"
# include "Response.hpp"

class Client
{
	public:
		Client();
		Client(const Client &);
		~Client();
		Client &operator=(const Client &);
		// bool	operator<(const Client &);

		Server		*clientServer;
		Request		request;
		Response	response;

		void	checkRequestValidity() throw (ErrorHandler);
		void	buildResponse();
		void	clearData();

	private:
		void				checkAutorisation(const t_location *) const;
		const t_location	* Client::buildCompleteUri();

};

std::ostream & operator<<(std::ostream &, const Client &);

#endif
