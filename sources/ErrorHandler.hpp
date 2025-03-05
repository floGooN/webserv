

#ifndef ERRORHANDLER_HPP
# define ERRORHANDLER_HPP

# include "webserv.hpp"
# include <dirent.h>

class Client;

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

		int	findErrorFile(DIR *, const std::string &) const;

		std::string	&_errcode;
		std::string	&_msg;
		Client		&_client;

};

#endif
