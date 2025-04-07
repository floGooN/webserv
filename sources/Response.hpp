/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fberthou <fberthou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 06:49:28 by fberthou          #+#    #+#             */
/*   Updated: 2025/04/07 06:57:19 by fberthou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
		bool 		isRepository(Client client);
		bool		isRedirect(const Client & client);
		bool		isCGI(Client client) throw (ErrorHandler);
		void		uploadFile(const Client &) throw (ErrorHandler);

		std::string	&findMimeType(const std::string &uri);
		std::string	setHeaderRedirect(const Client &client) throw (ErrorHandler);
		std::string	setHeader(const Request &, const std::string &) throw (ErrorHandler);
		std::string	extractFilename(const std::string &bodyHeader) throw (ErrorHandler);
};

std::ostream & operator<<(std::ostream &, const Response &);

#endif
