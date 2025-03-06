

#ifndef ERRORHANDLER_HPP
# define ERRORHANDLER_HPP

# include "webserv.hpp"
# include <dirent.h>

class Client;

/*
	CLIENT, ERR_KEY, ERR_LOG
*/
class ErrorHandler : std::exception
{
	public:
		ErrorHandler(Client &, std::string, std::string err);
		ErrorHandler(const ErrorHandler &);
		ErrorHandler &operator=(const ErrorHandler &);
		~ErrorHandler() throw();
		
		const char *	what() const throw() {
			return _errorLog.c_str();
		};
		void	generateErrorPage();

	private:
		std::string	findErrorFile(DIR *, const std::string &) const;
		std::string generateContent() const;
		std::string generateHeader() const;

		Client		&_client;
		std::string	_errorKey;
		std::string	_errorLog;

};

#endif
