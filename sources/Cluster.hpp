

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

#include "webserv.hpp"
#include "HttpConfig.hpp"
#include "Client.hpp"
#include "Server.hpp"

// timeout client ne fonctionne pas correctement

class Cluster
{
	class   InitException : virtual public std::exception
	{
		public:
			InitException(const char *file, int line, const std::string &msg, const int ret)
				: retAddr(ret), _file(file), _line(line), _msg(msg)
			{	}
			virtual ~InitException() throw() {}
			virtual const char *	what() const throw() {
				return _msg.c_str();
			};
			void			setSockExcept() const throw();
			const int 		retAddr;

		private:
			const char *_file;
			const int 	_line;
			std::string	_msg;
	};

	class   RunException : virtual public std::exception
	{
		public:
			RunException(const char *file, int line, const char *msg)
				: _file(file), _line(line), _msg(msg)
			{	}
			const char *	what() const throw() {
				return _msg;
			};
			void			runExcept() const throw();

		private:
			const char *	_file;
			const int 		_line;
			const char *	_msg;
	};

	public:
		Cluster(const std::string &);
		Cluster(const Cluster &);
		~Cluster();
		Cluster & operator=(const Cluster &);

		const HttpConfig				& getConfig() const;
		std::map<std::string, Server>	& getServersByPort() const;

		void	runCluster();

	private:
		int				_epollFd;		// fd vers structure epoll
		HttpConfig		_config;
		std::set<int>	_serverSockets;	// ensemble des socket serveur (un par port)
		std::map<std::string, Server >	_serversByService;

		Client	*addClient(const Request &req, const int);
		Client	*findClient(int fdClient);

		void	setEpollFd();
		void	setServersByPort();
		void	setServerSockets();
		
		void	closeFdSet() const;
		void	safeGetAddr(const char *, struct addrinfo **) const;
		void	createAndLinkSocketServer(const struct addrinfo &, const std::string &, int *);

		void	addFdInEpoll(const bool, const int)	const;
		void	changeEventMod(const bool, const int) const;
		void	acceptConnexion(const struct epoll_event &) const;
		void	closeConnexion(const struct epoll_event &event) const;

		void	sendData(const struct epoll_event &);
		void	recvData(const struct epoll_event &);
};

std::ostream	& operator<<(std::ostream &, const Cluster &);

#endif
