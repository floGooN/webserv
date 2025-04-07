/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fberthou <fberthou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 05:17:47 by fberthou          #+#    #+#             */
/*   Updated: 2025/04/07 05:17:49 by fberthou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"
# include "ServerStructure.hpp"

# include "ErrorHandler.hpp"
# include "Server.hpp"
# include "Request.hpp"
# include "Response.hpp"

class Server;

class Client
{
	public:
		Client(const int);
		Client(const Client &);
		~Client();
		Client &operator=(const Client &);

		Server		*clientServer;
		Request		request;
		Response	response;
		time_t		time;
		size_t		totalBytesReceived;
		const int	fdClient;

		void	checkRequestValidity() throw (ErrorHandler);
		void	buildResponse() throw (ErrorHandler);
		void	clearData();

	private:
		const t_location	*buildCompleteUri();
		bool				isAutoindex() throw (ErrorHandler);
		void				validTheUriPath() const throw (ErrorHandler);
		void				checkUriContent() const throw (ErrorHandler);
		void				checkAutorisation(const t_location *) const throw (ErrorHandler);
};

std::ostream & operator<<(std::ostream &, const Client &);

#endif
