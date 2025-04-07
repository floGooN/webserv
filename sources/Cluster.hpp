/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fberthou <fberthou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 06:49:45 by fberthou          #+#    #+#             */
/*   Updated: 2025/04/07 14:21:49 by fberthou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include "webserv.hpp"
# include "HttpConfig.hpp"
# include "ErrGenerator.hpp"

class Client;
class Server;

class Cluster
{
	class InitException : virtual public std::exception
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
		Cluster(const std::string &) throw (std::exception, InitException);
		~Cluster();
		Cluster & operator=(const Cluster &);

		void	runCluster();

		std::map<std::string, Server>	& getServersByPort() const;
		time_t							getKeepAlive() const;

	private:
		Cluster(const Cluster &);
		time_t							_keepAlive;
		int								_epollFd;
		std::set<int>					_serverSockets;
		std::map<const int, Client>		_clientList;
		std::map<std::string, Server >	_serversByService;

		void	setEpollFd() throw (InitException);
		void	setServersByPort(const HttpConfig &);
		void	setServerSockets() throw (InitException);
		void	setKeepAlive(const std::string &) throw (InitException);
		void	safeGetAddr(const char *, struct addrinfo **) const throw (InitException);
		void	createAndLinkSocketServer(const struct addrinfo &, const std::string &, int *) throw (InitException);

		void	acceptConnexion(const struct epoll_event &);
		void	addFdInEpoll(const bool, const int)	const throw (std::runtime_error);

		void	recvData(const struct epoll_event &) throw (ErrGenerator);
		void	sendData(const struct epoll_event &) throw (ErrGenerator);

		Client	&findClient(const int) throw (std::runtime_error);
		void	updateClient(Client &client) throw (ErrGenerator);
		ssize_t	safeRecv(const int, std::string &) throw (std::exception);
		void	changeEventMod(const bool, const int) throw (ErrGenerator);
		void	checkByteReceived(const struct epoll_event &event, ssize_t bytes) throw (ErrGenerator);

		Client	*updateClientsTime();
		void	closeAllSockets() const;
		void	updateTime(Client &client) throw (ErrGenerator);
		void	closeConnexion(const struct epoll_event &event);
};

std::ostream	& operator<<(std::ostream &, const Cluster &);

#endif
