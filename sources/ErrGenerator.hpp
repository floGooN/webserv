

#ifndef ERR_GENERATOR_HPP
# define ERR_GENERATOR_HPP

# include "webserv.hpp"
# include <dirent.h>

class Client;

/*
	CLIENT &, ERR_KEY, ERR_LOG
*/
class ErrGenerator : std::exception
{
	public:
		ErrGenerator(Client &, std::string, std::string err);
		ErrGenerator(const ErrGenerator &);
		ErrGenerator &operator=(const ErrGenerator &);
		~ErrGenerator() throw();
		
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
