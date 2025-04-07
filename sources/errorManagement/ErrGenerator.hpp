/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrGenerator.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fberthou <fberthou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 14:14:48 by fberthou          #+#    #+#             */
/*   Updated: 2025/04/07 14:17:18 by fberthou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <dirent.h>
# include "webserv.hpp"

# include <exception>

class Client;

#ifndef ERRGENERATOR_HPP
# define ERRGENERATOR_HPP

class ErrGenerator : virtual public std::exception
{
	public:
		ErrGenerator(Client &, const std::string &, const std::string &);
		ErrGenerator(const ErrGenerator &);
		~ErrGenerator() throw();
		
		const char *	what() const throw() {
			return NULL;
		}
		void			generateErrorPage();
		const Client	&getClient() const {
			return _client;
		}

	private:
		void		generateContent(std::string &) const;
		std::string	findErrorFile(DIR *, const std::string &) const;
		std::string generateHeader() const;

		Client &			_client;
		const std::string	_errorCode;
		const std::string	_errorLog;
};	

#endif
