

#ifndef ERR_GENERATOR_HPP
# define ERR_GENERATOR_HPP

# include "webserv.hpp"
# include <dirent.h>

class Client;

/*
	CLIENT &, ERR_KEY, ERR_LOG
*/
class ErrGenerator : virtual public std::exception
{
	public:
		ErrGenerator(const int, const std::string &, const std::string &);
		ErrGenerator(const ErrGenerator &);
		~ErrGenerator() throw();
		
		const char *	what() const throw() {
			return _errorLog.c_str();
		};
		void	generateErrorPage();

	private:
		std::string	findErrorFile(DIR *, const std::string &) const;
		std::string generateContent() const;
		std::string generateHeader() const;

		const int			_fdClient;
		const std::string	_errorCode;
		const std::string	_errorLog;

};

#endif
