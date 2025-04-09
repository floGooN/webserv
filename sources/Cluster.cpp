/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fberthou <fberthou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 05:26:30 by fberthou          #+#    #+#             */
/*   Updated: 2025/04/09 07:42:35 by fberthou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
						/*### HEADERS & STATIC FIELD  ###*/
/*============================================================================*/

#define MAXEVENT	10

int g_runServer = 1;

#include "Cluster.hpp"
#include "Request.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "ConfigParser.hpp"

#include <sys/epoll.h>
#include <fcntl.h>
#include <netdb.h>
#include <csignal>
#include <ctime>

void hand(int, siginfo_t *, void *) {
    g_runServer = 0;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
				/*### CONSTRUCTORS - DESTRUCTOR - OVERLOAD OP ###*/
/*============================================================================*/

Cluster::Cluster(const std::string &filepath) throw (std::exception, InitException)
{
	HttpConfig conf = ConfigParser().parse(filepath);

	try {
		setKeepAlive(conf.keepalive_timeout);
		setServersByPort(conf);	
		_serverSockets.clear();
		setServerSockets();
	}
	catch(const std::exception& e) {
		closeAllSockets();
		throw;
	}

	if (_serverSockets.empty() == true) 
		throw InitException(NULL, 0, ENOSERVICE, 0);

	try {
		_epollFd = -1;
		setEpollFd();
	}
	catch(const InitException& e) {
		e.setSockExcept();
		closeAllSockets();
		throw;
	}
#ifdef TEST
	std::cout	<< *thisSOCK_NONBLOCK
				<< BOLD BRIGHT_YELLOW "INIT TERMINATED\n" RESET
				<< std::endl;
#endif
}
/*----------------------------------------------------------------------------*/

Cluster::Cluster(const Cluster & )
{	}
/*----------------------------------------------------------------------------*/

Cluster::~Cluster()
{
	if (_epollFd > 0 && close(_epollFd) == -1)
		perror("close() in Cluster destuctor");
	closeAllSockets();
}
/*----------------------------------------------------------------------------*/

Cluster & Cluster::operator=(const Cluster & ) {
	return *this;
}
/*----------------------------------------------------------------------------*/

std::ostream	& operator<<(std::ostream & o, const Cluster &ref)
{
	o	<< BOLD "CLUSTER:" RESET << std::endl
		<< "_keepAliveTimeout: " << ref.getKeepAlive() << std::endl
		<< "_serversByService:\n";
	for (std::map<std::string, Server >::const_iterator it = ref.getServersByPort().begin();
		it != ref.getServersByPort().end(); it++) {
			o	<< it->second << std::endl;
		}
	
	return o << RESET;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

std::map<std::string, Server> & Cluster::getServersByPort() const {
	return const_cast<std::map<std::string, Server> & >(_serversByService);
}
/*----------------------------------------------------------------------------*/

time_t	Cluster::getKeepAlive() const {
	return _keepAlive;
}
/*----------------------------------------------------------------------------*/

/*	* main loop server
*/
void	Cluster::runCluster()
{
	std::string	dot[3] = {".  ", ".. ", "..."};
	int 		n = 0;
	
	struct epoll_event	events[MAXEVENT];
	while (g_runServer)
	{
		int nbEvents = epoll_wait(_epollFd, events, MAXEVENT, 777);
		if (nbEvents == -1)
			Utils::printLog(ERROR, "epoll_wait: " + std::string(strerror(errno)));
		else if (nbEvents > 0)
		{
			for (int i = 0; i < nbEvents; i++)
			{
				try
				{
					if (_serverSockets.count(events[i].data.fd) > 0)
						acceptConnexion(events[i]);
					else
					{
						if (events[i].events & EPOLLIN)
							recvData(events[i]);
						else if (events[i].events & EPOLLOUT)
							sendData(events[i]);
						else if (events[i].events & EPOLLRDHUP)
							closeConnexion(events[i]);
						else
							Utils::printLog(ERROR, "EPOLLERR");
					}
				}
				catch(ErrGenerator &e)
				{
					if (e.what() != NULL)
						std::cerr << RED << e.what() << RESET << std::endl;
					else {
						e.generateErrorPage();
						changeEventMod(false, events[i].data.fd);
					}
				}
			}
		}
		else
		{
			Client *client = updateClientsTime();
			if (client)
			{
				ErrGenerator e = ErrGenerator(*client, ERR_408, "");
				e.generateErrorPage();
				changeEventMod(false, e.getClient().fdClient);
			}
			std::cout	<< "\rWaiting on a connection" << dot[n == 3 ? n = 0 : n++] << std::flush;
		}
	}
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

void	Cluster::setKeepAlive(const std::string &keepalive) throw (InitException)
{
	if (keepalive.empty() == false)
	{
		std::istringstream	iss(keepalive);
		iss >> _keepAlive;
		if (iss.fail())
			throw InitException(__FILE__, __LINE__ - 2, "Error\n_keepAlive conversion", 0);
	}
	else
		_keepAlive = DFLT_TIMEOUT;
}
/*----------------------------------------------------------------------------*/

Client & Cluster::findClient(const int fd) throw (std::runtime_error)
{
	try {
		return  _clientList.at(fd);
	}
	catch(const std::exception& e) {
		throw std::runtime_error("no matching client");
	}
}
/*----------------------------------------------------------------------------*/

/*	* Create a new client on the matching sended with success to the client server
	* init the client with the parsed request
	* assign a pointer to the server associated with the request
*/
void Cluster::updateClient(Client &client) throw (ErrGenerator)
{
	if (client.clientServer == NULL)
	{
		try {
			client.clientServer = &getServersByPort().at(client.request.getHeader().hostPort);
		}
		catch(const std::exception& e) {
			throw ErrGenerator(client, ERR_500, "We didn't recovered the service\n");
		}
	}

	if (client.request.getHeader().uri.size() > DFLT_URISIZE)
		throw ErrGenerator(client, ERR_414, "URI exceed the limit of the server\n");

	if (client.request.getbody().contentLength > client.clientServer->getParams().maxBodySize)
		throw ErrGenerator(client, ERR_413, "The content length of the request exceed the limit allowed by the server\n");
}
/*----------------------------------------------------------------------------*/

/*	* get BUFFERSIZE octets of the message from client
	* append message safety
	* return nb bytes received
*/
ssize_t	Cluster::safeRecv(const int clientFd, std::string &message) throw (std::exception)
{
	char	buffer[STATIC_BUFFSIZE] = {'\0'};
	ssize_t	bytesReceived = recv(clientFd, buffer, STATIC_BUFFSIZE, 0);
	
	if ( ! bytesReceived || bytesReceived == -1 )
	{
		if ( bytesReceived )
			Utils::printLog(ERROR, "recv()\n");
		return bytesReceived;
	}
	try {
		message.clear();
		message.assign(buffer, bytesReceived);
	}
	catch(const std::exception& e) {
		Utils::printLog(ERROR, "safeRecv(): " + std::string(e.what(), '\n'));
		return -1;
	}
	return bytesReceived;
}
/*----------------------------------------------------------------------------*/

void	Cluster::checkByteReceived(const struct epoll_event &event, ssize_t bytes) throw (ErrGenerator)
{
	if ( ! bytes ) {
		throw ErrGenerator(findClient(event.data.fd), ERR_499, "Client closed connexion\n");
	}
	if ( bytes == -1 ) {
		throw ErrGenerator(findClient(event.data.fd), ERR_500, "");
	}
}
/*----------------------------------------------------------------------------*/

/*  * Receive data, parse the header, and extract the body of the request  
    * If it's a new client, create one; if it already exists, update its request  
*/
void	Cluster::recvData(const struct epoll_event &event) throw (ErrGenerator)
{
#ifdef TEST
	std::cout	<< BOLD BRIGHT_PURPLE "\nFunction -> recvData() {\n"
				<< "ClientSocket [" RESET PURPLE << event.data.fd << BOLD BRIGHT_PURPLE "]" RESET
				<< std::endl;
#endif
	std::string	message("\0");
	ssize_t		bytesReceived;
	Client		&currentClient = findClient(event.data.fd);

	bytesReceived = safeRecv(event.data.fd, message);
	checkByteReceived(event, bytesReceived);

	if (currentClient.request.getHeader().requestType == EMPTY)
	{
		try {
			currentClient.request = Request(message);
			updateClient(currentClient);
		}
		catch(const ErrorHandler &e) {
			throw ErrGenerator(currentClient, e.errorNumber, e.errorLog);
		}
	}
	else
	{
		try {
			currentClient.request.updateRequest(message);
		}
		catch(const ErrorHandler &e) {
			throw ErrGenerator(currentClient, e.errorNumber, e.errorLog);
		}	
	}

	currentClient.totalBytesReceived += bytesReceived;

	if (currentClient.request.getbody().body.size() == \
		currentClient.request.getbody().contentLength)
	{
		try {
			currentClient.checkRequestValidity();
			changeEventMod(false, event.data.fd);
		}
		catch(const ErrorHandler& e) {
			throw ErrGenerator(findClient(event.data.fd), e.errorNumber, e.errorLog);
		}
	}
}
/*----------------------------------------------------------------------------*/

/*	* Send data to the client and manage connexion (keep-Alive or not)
*/
void	Cluster::sendData(const struct epoll_event &event) throw (ErrGenerator)
{
#ifdef TEST
	std::cout	<< BOLD BRIGHT_PURPLE "\nFunction -> sendData()\n"
				<< "ClientSocket [" RESET PURPLE << event.data.fd
				<< BOLD BRIGHT_PURPLE "]\n" RESET
				<< std::endl;
#endif

	Client	&client = _clientList.at(event.data.fd);

	try {
		client.buildResponse();
	}
	catch(const ErrorHandler& e) {
		throw ErrGenerator(client, e.errorNumber, e.errorLog);
	}

	ssize_t ret = send(event.data.fd, client.response.message.c_str(), \
									client.response.message.length(), 0);
	if (ret <= 0) {
		if (ret == -1)
			throw ErrGenerator(client, ERR_500, "send(): an error occured");
	}

	client.response.totalBytesSended += ret;
	
	if (client.response.message.length() == client.response.totalBytesSended && \
		client.request.keepAlive == true)
	{
		client.clearData();
		updateTime(client);
		changeEventMod(true, event.data.fd);

		std::stringstream ss;
		ss << "response sended with success to the client [" << client.fdClient << "]" << std::endl;
		Utils::printLog(INFO, ss.str());
	}
	else
	{
		std::stringstream ss;
		ss << "response sended with success to the client [" << client.fdClient << "]" << std::endl;
		Utils::printLog(INFO, ss.str());	
		closeConnexion(event);
	}
}
/*----------------------------------------------------------------------------*/

/*	* init epoll events for servers & new client socket and add the fd in epoll set
*/
void	Cluster::addFdInEpoll(const bool isServerSocket, const int fd) const throw (std::runtime_error)
{
	struct epoll_event	ev;
	memset(&ev, 0, sizeof(ev));

	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLRDHUP;
	
	if (isServerSocket == false)
		ev.events |= EPOLLET;

	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &ev) == -1)
		throw std::runtime_error("addFdInEpoll : error epoll_ctl()\n");
}
/*----------------------------------------------------------------------------*/

/*	* init servers from _serverconfig
*/
void Cluster::setServersByPort(const HttpConfig &config)
{
	std::vector<ServerConfig>::const_iterator	itConfigServer = config.serversConfig.begin();
	std::pair<std::map<std::string, Server>::const_iterator, bool>	result;

	while (itConfigServer != config.serversConfig.end())
	{
		std::vector<std::string>::const_iterator	itServiceList = itConfigServer->listenPort.begin();
		while (itServiceList != itConfigServer->listenPort.end())
		{
			result = _serversByService.insert(std::make_pair(*itServiceList, Server(*itConfigServer, *itServiceList)));
			if (!result.second)
			{
				std::stringstream ss;
				ss	<< YELLOW "Port [" << *itServiceList << "] still required by server "
					<< *(result.first->second.getParams().nameList.begin())
					<< RESET << std::endl;
				throw InitException(__FILE__, __LINE__ - 2, ss.str(), 0);
			}
			itServiceList++;
		}
		itConfigServer++;
	}
}
/*----------------------------------------------------------------------------*/

/*	* open sockets server and bind them
*/
void	Cluster::setServerSockets() throw (InitException)
{
	std::map<std::string, Server>::iterator itServer = _serversByService.begin();
	
	while (itServer != _serversByService.end())
	{
		int				sockFd = 0;
		struct addrinfo	*res = NULL;
		try
		{
			safeGetAddr(itServer->first.c_str(), &res);
			createAndLinkSocketServer(*res, itServer->first, &sockFd);
			freeaddrinfo(res);
			itServer++;
		}
		catch(const InitException &e)
		{
			if (sockFd < 0){
				e.setSockExcept();
				throw;
			}
			else if (sockFd > 0 && close(sockFd) != 0) {
				freeaddrinfo(res);
				throw InitException(__FILE__, __LINE__ - 2, "Error -> close()", 0);
			}
			freeaddrinfo(res);
			e.setSockExcept();
			std::cerr << (e.what() != NULL ? e.what() : "") << std::endl;

			std::map<std::string, Server>::iterator tmp = itServer;
			tmp++;
			_serversByService.erase(itServer);
			itServer = tmp;
		}
	}
}
/*----------------------------------------------------------------------------*/

/*	* open epoll & add the server sockets to the set 
*/
void	Cluster::setEpollFd() throw (InitException)
{
	_epollFd = epoll_create(1);
	if (_epollFd < 0)
		throw InitException(__FILE__, __LINE__ - 2, "error creation epoll()", 0);

	for (std::set<int>::iterator it = _serverSockets.begin(); it != _serverSockets.end(); it++)
		addFdInEpoll(true, *it);
}
/*----------------------------------------------------------------------------*/

/*	* get an availble address on an avaible service (port)
	*
	* ai_family = AF_INET6					-> Specifies that we use IPv6 addresses. With AI_V4MAPPED, this also allows support for IPv4
	* ai_socktype = SOCK_STREAM				-> Indicates a socket for stream-based communication (connection-oriented, like TCP)
	* ai_protocol = IPPROTO_TCP				-> Sets the transport protocol to TCP (Transmission Control Protocol)
	* ai_flags = AI_PASSIVE | AI_V4MAPPED	-> AI_PASSIVE: returns an address usable by bind() to listen on all local interfaces
											-> AI_V4MAPPED: allows accepting IPv4 connections as mapped IPv6 addresses
*/
void	Cluster::safeGetAddr(const char *serviceName, struct addrinfo **res) const throw (InitException)
{
	struct addrinfo	hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE | AI_V4MAPPED;

	int ret = getaddrinfo(NULL, serviceName, &hints, res);
	if (ret != 0) {
		std::string errorMsg = "Error -> safeGetAddr() on port [" + std::string(serviceName) + "]";
		throw InitException(__FILE__, __LINE__ - 2, errorMsg.c_str(), ret);
	}
}
/*----------------------------------------------------------------------------*/

/*	* opening + settings options + link SOCKET SERVER
	* non-blocking
	* SO_REUSEADDR allows reusing the port immediately after server shutdown,
	* avoiding "Address already in use" errors
*/
void	Cluster::createAndLinkSocketServer(const struct addrinfo &res, const std::string & serviceName, int *sockfd) throw (InitException)
{
	for (const struct addrinfo *currNode = &res; currNode != NULL; currNode = currNode->ai_next)
	{
		*sockfd = socket(currNode->ai_family, currNode->ai_socktype | SOCK_NONBLOCK, currNode->ai_protocol);
		if (*sockfd < 0) {		
			throw InitException(__FILE__, __LINE__ - 2, "Error -> socket()", 0);
		}
		int opt = 1;
		if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
			throw InitException(__FILE__, __LINE__ - 1, "Error -> setsockopt()", 0);
		}
		if (bind(*sockfd, currNode->ai_addr, currNode->ai_addrlen) != 0) {
			throw InitException(__FILE__, __LINE__ - 1, std::string("Error -> bind() on port [" + serviceName + "]").c_str(), 0);
		}
		if (listen(*sockfd, SOMAXCONN) != 0) {
			throw InitException(__FILE__, __LINE__ - 1, std::string("Error -> listen() on port [" + serviceName + "]").c_str(), 0);
		}
		_serverSockets.insert(*sockfd);
	}
}
/*----------------------------------------------------------------------------*/

/*	* accept a new client connexion, set the socket and add the fd in the epoll set
	* doesn't throwing exception
*/
void	Cluster::acceptConnexion(const struct epoll_event &event)
{
	struct sockaddr addr;
	socklen_t		addrSize = sizeof(addr);
	const int		clientSocket = accept(event.data.fd, &addr, &addrSize);

#ifdef TEST
	std::cout	<< BOLD BRIGHT_PURPLE "\nFunction -> acceptConnexion()\n"
				<< "ClientSocket [" RESET PURPLE << clientSocket
				<< BOLD BRIGHT_PURPLE "]" RESET
				<< std::endl;
#endif
	if (clientSocket < 0) {
		Utils::printLog(ERROR, "accept() in acceptConnexion(): " + std::string(strerror(errno)));
		return;
	}
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
	{
		Utils::printLog(ERROR, "fcntl() in acceptConnexion(): " + std::string(strerror(errno)));
		if (close(clientSocket) == -1)
			Utils::printLog(ERROR, "close in acceptConnexion(): " + std::string(strerror(errno)));
		return ;
	}

	try {
		addFdInEpoll(false, clientSocket);
		_clientList.insert(std::make_pair(clientSocket, Client(clientSocket)));

	}
	catch(const std::exception& e)
	{
		Utils::printLog(ERROR, "insert() in acceptConnexion(): " + std::string(e.what()));
		if (close(clientSocket) == -1)
			Utils::printLog(ERROR, "close in acceptConnexion(): " + std::string(strerror(errno)));
		return ;
	}

	std::stringstream ss;
	ss << "New client connected: fd=" << clientSocket << std::endl;
	Utils::printLog(INFO, ss.str());
}
/*----------------------------------------------------------------------------*/

/*	* switch events mode between EPOLLOUT and EPOLLIN
*/
void	Cluster::changeEventMod(const bool changeForRead, const int fd) throw (ErrGenerator)
{
	struct epoll_event	ev;
	memset(&ev, 0, sizeof(ev));
	
	ev.data.fd = fd;
	ev.events = EPOLLRDHUP | (changeForRead ? EPOLLIN : EPOLLOUT);
	
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev) == -1)
		throw ErrGenerator(findClient(fd), ERR_500, "changeEventMod(): " + std::string(strerror(errno)));
}
/*----------------------------------------------------------------------------*/

/*	* close a client connexion
*/
void	Cluster::closeConnexion(const struct epoll_event &event)
{
#ifdef TEST
	std::cout	<< BOLD BRIGHT_PURPLE "\nFunction -> closeConnexion()\n"
				<< "Client fd [" RESET PURPLE << event.data.fd
				<< BOLD BRIGHT_PURPLE "]" RESET
				<< std::endl;
#endif

	try {
		_clientList.erase(event.data.fd);
	}
	catch (const std::exception &e) {
		Utils::printLog(ERROR, "erase in closeConnexion(): " + std::string(e.what()));
	}

	if (event.data.fd > 0 && epoll_ctl(_epollFd, EPOLL_CTL_DEL, event.data.fd, NULL) == -1)
		Utils::printLog(ERROR, "epoll_ctl() in closeConnexion(): " + std::string(strerror(errno)));

	if (event.data.fd > 0 && close(event.data.fd) == -1)
		Utils::printLog(ERROR, "close() in closeConnexion(): " + std::string(strerror(errno)));
	
	std::stringstream ss;
	ss	<< "Close connexion for client [" << event.data.fd << "]" << std::endl;
	Utils::printLog(INFO, ss.str()); 
}
/*----------------------------------------------------------------------------*/

void	Cluster::updateTime(Client &client) throw (ErrGenerator)
{
	client.time = time(NULL);
	if (client.time == (time_t) - 1)
		throw ErrGenerator(client, ERR_500, "In updateTime(): ");
}
/*----------------------------------------------------------------------------*/

Client *Cluster::updateClientsTime()
{
	std::map<const int, Client>::iterator it = _clientList.begin();
	
	while (it != _clientList.end())
	{
		if (std::time(NULL) - it->second.time >= _keepAlive)
		{
			std::stringstream ss;
			ss	<< "Connexion timeout for client [" << it->first << "]" << std::endl;
			Utils::printLog(INFO, ss.str());

			return &it->second;
		}
		it++;
	}

	return NULL;
}
/*----------------------------------------------------------------------------*/

/*	* close all server & client sockets
*/
void	Cluster::closeAllSockets() const
{
	for (std::set<int>::iterator it = _serverSockets.begin(); it != _serverSockets.end(); it++)
		if (*it > 0 && close(*it) != 0)
			Utils::printLog(ERROR, "1st close() in closeAllSockets(): " + std::string(strerror(errno)));

	for (std::map<int, Client>::const_iterator it = _clientList.begin(); it != _clientList.end(); it++)
		if (it->first > 0 && close(it->first) != 0)
			Utils::printLog(ERROR, "2nd close() in closeAllSockets(): " + std::string(strerror(errno)));
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
							/*### EXCEPTIONS ###*/
/*============================================================================*/
void	Cluster::InitException::setSockExcept() const throw() {
	if (errno != 0)
		Utils::printLog(ERROR, strerror(errno));
	if (retAddr != 0)
		Utils::printLog(ERROR, gai_strerror(retAddr) + std::string(": "));

	std::stringstream ss;
	ss  << YELLOW "at file [" << _file << "] line [" << _line << "]" << RESET << std::endl;
	std::string msg(ss.str());
	
	Utils::printLog(ERROR, msg);
}
/*----------------------------------------------------------------------------*/
