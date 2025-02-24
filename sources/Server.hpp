#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"
# include "ServerConfig.hpp"
# include "Client.hpp"
class Client;

class Server
{
	public:
		Server(ServerConfig &, const std::string&);
		Server(const Server &);
		~Server();
		Server() {};
		Server	&operator=(const Server &);
		bool 	operator<(const Server &other) const;

		size_t				getBodySize() const;
		const ServerConfig	&getConfig() const;
		const std::string	&getService() const;
		std::map<int, Client>		&getClientList() const;
		const std::set<std::string>	&getNameList() const;
		const std::set<std::string>	&getLocationPath() const;

	private:
		ServerConfig	_config;
		size_t					_maxBodySize;
		std::string				_service;		//service name(port)
		std::set<std::string>	_nameList;		//localhost - serverExemple.com - www.serverExemple.fr
		std::set<std::string>	_locationPath;	//websites handle by the server (/website/site1.com)
		std::map<int, Client>	_clientList;
		
		std::set<std::pair<int, std::string> >	_errorPathList;
		void	setLocationPath();
		void	setErrorPath();

};
std::ostream	& operator<<(std::ostream &, const Server &);

#endif
