

#ifndef ERRORHANDLER_HPP
# define ERRORHANDLER_HPP

# include "webserv.hpp"
# include <dirent.h>

class Client;

/*
	prend qu'un client en parametre
	le client peut etre null (donc il faut un ptr su Client en params)
	si le client n'existe pas (NULLPTR) c'est quil y a une erreur systeme (add client)
*/
class ErrorHandler
{
	public:
		ErrorHandler(std::string &, std::string &, Client &);
		ErrorHandler(const ErrorHandler &);
		~ErrorHandler();
		
		void	generateErrorPage() const;

	private:
		ErrorHandler		&operator=(const ErrorHandler &);
		const std::string	_dfltErrorPath;

		std::string	findErrorFile(DIR *, const std::string &) const;
		std::string generateContent() const;

		std::string	&_errcode;
		std::string	&_msg;
		Client		&_client;

};

#endif
