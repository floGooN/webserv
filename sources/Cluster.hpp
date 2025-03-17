




#ifndef CLUSTER_HPP
# define CLUSTER_HPP

#include "webserv.hpp"
#include "HttpConfig.hpp"

#include "Client.hpp"
#include "Server.hpp"

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
			}
			void			setSockExcept() const throw();
			const int 		retAddr;

		private:
			const char *_file;
			const int 	_line;
			std::string	_msg;
	};

	public:
		Cluster(const std::string &);
		Cluster(const Cluster &);
		~Cluster();
		Cluster & operator=(const Cluster &);

		void	runCluster();
		
		const HttpConfig				& getConfig() const;
		std::map<std::string, Server>	& getServersByPort() const;

	private:
		int				_epollFd;
		HttpConfig		_config;
		std::set<int>	_serverSockets;
		std::set<int>	_clientSockets;

		void	setEpollFd();
		void	setServersByPort();
		void	setServerSockets();
		void	safeGetAddr(const char *, struct addrinfo **) const;
		void	createAndLinkSocketServer(const struct addrinfo &, const std::string &, int *);


		std::map<std::string, Server >		_serversByService;
		std::set<Client>					_clientList;
		
		void	acceptConnexion(const struct epoll_event &);
		void	addFdInEpoll(const bool, const int)	const throw (std::runtime_error);
		
		
		void	recvData(const struct epoll_event &);
		ssize_t	safeRecv(const int, std::string &);
		void	checkByteReceived(const struct epoll_event &event, ssize_t bytes) throw (ErrGenerator);
		
		Client	*addClient(const Request &req, const int) throw (ErrGenerator);

		
		
		void	closeConnexion(const struct epoll_event &event);
		void	closeFdSet() const;
		void	changeEventMod(const bool, const int) const;
		
		void	sendData(const struct epoll_event &);
		Client	*findClient(const int fdClient);
};

std::ostream	& operator<<(std::ostream &, const Cluster &);

#endif
