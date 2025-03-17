

#ifndef SERVER_HPP
# define SERVER_HPP

# include "ServerStructure.hpp"

class Client;
class ServerConfig;

class Server
{
	public:
		Server(const ServerConfig &, const std::string&);
		Server(const Server &);
		~Server();
		Server	&operator=(const Server &);
		bool 	operator<(const Server &other) const;
		
		const t_params				getParams() 		const;
		const std::set<t_location>	getLocationSet()	const;

	private:
		const t_params				_params;
		const std::set<t_location>	_locationSet;

		t_params	setParams(const ServerConfig &, const std::string &);
		void	setNameList(const std::vector<std::string> &, t_params &);
		void	setMethod(const std::vector<std::string> &, t_params &);
		void	setUploadPath(const std::string &, t_params &);
		void	setErrorPath(const std::string &, t_params &);
		void	setIndexFile(const std::string &, t_params &);
		void	setBodySize(const std::string &, t_params &);
		void	setRootPath(const std::string &, t_params &);
		void	setService(const std::string &, t_params &);

		std::set<t_location>	setLocations(const std::vector<LocationConfig> &);
		void	setMethods(const std::vector<std::string> &, t_location &);
		void	setAutoindex(const std::string &, t_location &);
		void	setCgiPath(const std::string &, t_location &);
		void	setIndex(const std::string &, t_location &);
		void	setPath(const std::string &, t_location &);
		void	setRoot(const std::string &, t_location &);
};

std::ostream	& operator<<(std::ostream &, const Server &);

#endif
