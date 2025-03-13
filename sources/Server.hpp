

#ifndef SERVER_HPP
# define SERVER_HPP

# include "ServerStructure.hpp"

class Client;
class ServerConfig;

class Server
{
	public:
		Server(ServerConfig &, const std::string&);
		Server(const Server &);
		~Server();
		Server	&operator=(const Server &);
		bool 	operator<(const Server &other) const;
		
		const s_params				getParams() 		const;
		const std::set<s_location>	getLocationSet()	const;

	private:
		const s_params				_params;
		const std::set<s_location>	_locationSet;

		s_params				setParams(const ServerConfig &, const std::string &);
		
		std::set<s_location>	setLocations(const std::vector<LocationConfig> &);
		

		std::set<std::pair<int, std::string> >	_errorPathList;
		std::set<std::pair<int, std::string> >	setErrorPath();

};

std::ostream	& operator<<(std::ostream &, const Server &);

#endif
