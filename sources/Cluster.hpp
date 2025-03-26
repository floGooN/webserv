




#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include "webserv.hpp"
# include "HttpConfig.hpp"

# include "Client.hpp"
# include "Server.hpp"

# include <dirent.h>

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

	class ErrGenerator : virtual public std::exception
	{
		public:
			ErrGenerator(Client &, const std::string &, const std::string &);
			ErrGenerator(const ErrGenerator &);
			~ErrGenerator() throw();
			
			const char *	what() const throw() {
				return NULL;
			};
			void	generateErrorPage();

		private:
			void		generateContent(std::string &) const;
			std::string	findErrorFile(DIR *, const std::string &) const;
			std::string generateHeader() const;

			Client &			_client;
			const std::string	_errorCode;
			const std::string	_errorLog;
	};	

	public:
		Cluster(const std::string &);
		Cluster(const Cluster &);
		~Cluster();
		Cluster & operator=(const Cluster &);

		void	runCluster();
		
		std::map<std::string, Server>	& getServersByPort() const;

	private:
		int								_epollFd;
		std::set<int>					_serverSockets;
		std::map<const int, Client>		_clientList;
		std::map<std::string, Server >	_serversByService;

		void	setEpollFd();
		void	setServerSockets();
		void	setServersByPort(const HttpConfig &);
		void	safeGetAddr(const char *, struct addrinfo **) const;
		void	createAndLinkSocketServer(const struct addrinfo &, const std::string &, int *);

		void	acceptConnexion(const struct epoll_event &);
		void	addFdInEpoll(const bool, const int)	const throw (std::runtime_error);
		
		void	recvData(const struct epoll_event &);
		void	sendData(const struct epoll_event &);

		Client	&findClient(const int) throw (std::runtime_error);
		ssize_t	safeRecv(const int, std::string &);
		void	updateClient(Client &client) throw (ErrGenerator);
		void	changeEventMod(const bool, const int) throw (ErrGenerator);
		void	checkByteReceived(const struct epoll_event &event, ssize_t bytes) throw (ErrGenerator);

		void	closeConnexion(const struct epoll_event &event);
		void	closeAllSockets() const;
};

std::ostream	& operator<<(std::ostream &, const Cluster &);

#endif
