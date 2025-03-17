


/*============================================================================*/
								/*### HEADERS ###*/
/*============================================================================*/
#include "Cluster.hpp"
#include "Request.hpp"
#include "ConfigParser.hpp"

#include "ErrGenerator.hpp"

#include <sys/epoll.h>
#include <fcntl.h>
#include <netdb.h>

/*	* ressources provisoirs
*/
#include <csignal>
#include <sstream>
#include <cstdlib>

#define MAXEVENT	10

int g_runServer = 0;

void hand(int, siginfo_t *, void *) {
    g_runServer = 0;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
				/*### CONSTRUCTORS - DESTRUCTOR - OVERLOAD OP ###*/
/*============================================================================*/

Cluster::Cluster(const std::string &filepath)
{
	setServersByPort(ConfigParser().parse(filepath));
	try {
		_serverSockets.clear();
		setServerSockets();
	}
	catch(const std::exception& e) {
		closeFdSet();
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
		closeFdSet();
		throw;
	}
#ifdef TEST
	std::cout	<< *this
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
	closeFdSet();
}
/*----------------------------------------------------------------------------*/

Cluster & Cluster::operator=(const Cluster & ) {
	return *this;
}
/*----------------------------------------------------------------------------*/

std::ostream	& operator<<(std::ostream & o, const Cluster &ref)
{
	o	<< BOLD "CLUSTER:" RESET << std::endl
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

/*	* main loop server
*/
void	Cluster::runCluster()
{
	std::string	dot[3] = {".  ", ".. ", "..."};
	int 		n = 0;
	
	struct epoll_event	events[MAXEVENT];
	g_runServer = 1;
	while (g_runServer)
	{
		int nbEvents = epoll_wait(_epollFd, events, MAXEVENT, 777);
		if (nbEvents == -1)
			perror("\nepoll_wait");
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
						else if (events[i].events & EPOLLRDHUP) {
							std::cout << "IN RUN" << std::endl;
							closeConnexion(events[i]);
						}
						else
							std::cerr << "have to print EPOLLERR" << std::endl;
					}
				}
				catch(ErrGenerator &e)
				{
					if (e.what() != NULL) {
						std::cerr << e.what() << RESET << std::endl;
					}
					else {
						e.generateErrorPage();
					}
				}
			}
		}
		else
			std::cout	<< "\rWaiting on a connection" << dot[n == 3 ? n = 0 : n++]
						<< std::flush;
	}
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

/*	* Create a new client on the matching server
	* init the client with the parsed request
	* assign a pointer to the server associated with the request
*/
Client * Cluster::addClient(const Request &req, const int fdClient) throw (ErrGenerator)
{
	Client	*current = NULL;
	Server	*server = NULL;
	
	try {
		current = &_clientList.at(fdClient);
	}
	catch(const std::exception& e) {
		throw ErrGenerator(fdClient, ERR_500, "We didn't recovered the client");
	}

	current->request = req;
	
	try {
		server = &getServersByPort().at(current->request.getHeader().hostPort);
	}
	catch(const std::exception& e) {
		throw ErrGenerator(fdClient, ERR_500, "We didn't recovered the service");
	}


	if (current->request.getbody().contentLength > server->getParams().maxBodySize)
		throw ErrGenerator(fdClient, ERR_413, "The content length of the request exceed the limit allowed by the server");

	current->clientServer = server;
	return current;
}
/*----------------------------------------------------------------------------*/

/*	* get BUFFERSIZE octets of the message from client
	* append message safety
	* return nb bytes received (for exit loop)
	* throw error with correct page if an error occured
*/
ssize_t	Cluster::safeRecv(const int clientFd, std::string &message)
{
	char	buffer[STATIC_BUFFSIZE] = {'\0'};
	ssize_t	bytesReceived = recv(clientFd, buffer, STATIC_BUFFSIZE, 0);
	
	if ( ! bytesReceived || bytesReceived == -1 )
	{
		if ( bytesReceived )
			perror("recv()");
		return bytesReceived;
	}
	try {
		message.clear();
		message.assign(buffer, bytesReceived);
	}
	catch(const std::exception& e) {
		std::cerr << "safeRecv(): " << e.what() << std::endl;
		return -1;
	}
	return bytesReceived;
}
/*----------------------------------------------------------------------------*/

void	Cluster::checkByteReceived(const struct epoll_event &event, ssize_t bytes) throw (ErrGenerator)
{
	if ( ! bytes ) {
		throw ErrGenerator(event.data.fd, ERR_499, "Client closed connexion");
	}
	if ( bytes == -1 ) {
		throw ErrGenerator(event.data.fd, ERR_500, "");
	}
}
/*----------------------------------------------------------------------------*/

/*  * Receive data, parse the header, and extract the body of the request  
    * The while loop receives data  
    * If it's a new client, create one; if it already exists, update its request  
*/
void	Cluster::recvData(const struct epoll_event &event)
{
#ifdef TEST
	std::cout	<< BOLD BRIGHT_PURPLE "\nFunction -> recvData() {\n"
				<< "ClientSocket [" RESET PURPLE << event.data.fd << BOLD BRIGHT_PURPLE "]" RESET
				<< std::endl;
#endif
	std::string	message("\0");
	ssize_t		bytesReceived;
	Client		*currentClient = NULL;
	
	bytesReceived = safeRecv(event.data.fd, message);
	checkByteReceived(event, bytesReceived);
	currentClient = addClient(Request(message), event.data.fd);

	while (bytesReceived == STATIC_BUFFSIZE)
	{
		bytesReceived = safeRecv(event.data.fd, message);
		checkByteReceived(event, bytesReceived);
		currentClient->request.updateRequest(message);
	
		if (currentClient->request.getbody().body.size() > \
			currentClient->clientServer->getParams().maxBodySize) {
			throw ErrGenerator(event.data.fd, ERR_413, "Max body size reached");
		}
	}

	if (currentClient->request.getbody().body.size() == \
		currentClient->request.getbody().contentLength)
	{
		try {
			currentClient->checkRequestValidity();
			changeEventMod(false, event.data.fd);
		}
		catch(const ErrorHandler& e) {
			throw ErrGenerator(event.data.fd, e.errorNumber, e.errorLog);
		}
	}
}
/*----------------------------------------------------------------------------*/

/*	* Send data to the client and manage connexion (keep-Alive or not)
*/
void	Cluster::sendData(const struct epoll_event &event)
{
#ifdef TEST
	std::cout	<< BOLD BRIGHT_PURPLE "\nFunction -> sendData()\n"
				<< "ClientSocket [" RESET PURPLE << event.data.fd
				<< BOLD BRIGHT_PURPLE "]\n" RESET
				<< std::endl;
#endif

	Client	&client = _clientList.at(event.data.fd);
	
	
	if (client->response.finalMessage.empty() == true)
		client->buildResponse();

	while (client->request.totalBytessended != client->response.finalMessage.size())
	{
		ssize_t ret = send(event.data.fd, client->response.finalMessage.c_str(), \
										client->response.finalMessage.length(), 0);
		if (ret <= 0) {
			// throw ErrorHandler(*client, ERR_??, "0 - 1"); 
			break;
		}
		client->request.totalBytessended += ret;
	}

	if (client->response.finalMessage.length() == client->request.totalBytessended && \
		client->request.keepAlive == true)
	{
		client->clearData();
		try {
			changeEventMod(true, event.data.fd);
			std::cout << "sended with success" << std::endl;
		}
		catch(const std::exception& e) {
			throw ErrorHandler(*client, ERR_500, e.what());
		}
	}
	else {
		std::cout << "closeConnexion in sendData()\nsended with success" << std::endl;
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
		throw std::runtime_error("addFdInEpoll : error epoll_ctl()");
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
				std::cerr << YELLOW "Port [" << *itServiceList << "] still required by server "
						  << *(result.first->second.getParams().nameList.begin())
						  << RESET << std::endl;
			}
			itServiceList++;
		}
		itConfigServer++;
	}
}
/*----------------------------------------------------------------------------*/

/*	* open sockets server and bind them
*/
void	Cluster::setServerSockets()
{
# ifdef TEST
	std::cout	<< BOLD BLUE << "Function -> setServerSockets() {"
				<< RESET << std::endl;
# endif

	std::map<std::string, Server>::iterator itServer = _serversByService.begin();
	
	while (itServer != _serversByService.end())
	{
		int				sockFd = 0;
		struct addrinfo	*res = NULL;
		try {
			safeGetAddr(itServer->first.c_str(), &res);
			createAndLinkSocketServer(*res, itServer->first, &sockFd);
			freeaddrinfo(res);
			itServer++;
		}
		catch(const InitException &e) {
			if (sockFd == -1){
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
# ifdef TEST
	std::cout	<< BOLD BLUE "}\n" RESET
				<< std::endl;
# endif
}
/*----------------------------------------------------------------------------*/

/*	* open epoll & add the server sockets to the set 
*/
void	Cluster::setEpollFd()
{
#ifdef TEST
	std::cout	<< BOLD BLUE << "Function -> setEpollFd() {"
				<< RESET << std::endl;
#endif

	_epollFd = epoll_create(1);
	if (_epollFd < 0)
		throw InitException(__FILE__, __LINE__ - 2, "error creation epoll()", 0);

	for (std::set<int>::iterator it = _serverSockets.begin(); it != _serverSockets.end(); it++)
		addFdInEpoll(true, *it);

#ifdef TEST
	for (std::set<int>::iterator it = _serverSockets.begin(); \
		it != _serverSockets.end(); it++)
		std::cout << BLUE "fd [" << *it << "] added in epollFd" << std::endl;
	std::cout	<< BOLD BLUE "}\n" RESET
				<< std::endl;
#endif
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
void	Cluster::safeGetAddr(const char *serviceName, struct addrinfo **res) const
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
void	Cluster::createAndLinkSocketServer(const struct addrinfo &res, const std::string & serviceName, int *sockfd)
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
		perror("accept() in acceptConnexion()");
		return;
	}
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
	{
		perror("fcntl() in acceptConnexion()");
		if (close(clientSocket) == -1)
			perror("close in acceptConnexion()");
		return ;
	}

	try {
		addFdInEpoll(false, clientSocket);
		_clientList.insert(std::make_pair(clientSocket, Client()));
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		if (close(clientSocket) == -1)
			perror("close in acceptConnexion()");
		return ;
	}
}
/*----------------------------------------------------------------------------*/

/*	* switch events mode between EPOLLOUT and EPOLLIN
*/
void	Cluster::changeEventMod(const bool changeForRead, const int fd) const throw (ErrGenerator)
{
	struct epoll_event	ev;
	memset(&ev, 0, sizeof(ev));
	
	ev.data.fd = fd;
	ev.events = EPOLLET | EPOLLRDHUP | (changeForRead ? EPOLLIN : EPOLLOUT);
	
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev) == -1)
		throw ErrGenerator(fd, ERR_500, "changeEventMod(): " + std::string(strerror(errno)));
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
	std::map<std::string, Server>::iterator it = _serversByService.begin();
	for (; it != _serversByService.end(); it++) {
		it->second.getClientList().erase(event.data.fd);
	}

	_clientSockets.erase(event.data.fd);

	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, event.data.fd, NULL) == -1)
		perror("epoll_ctl() in closeConnexion()");

	if (event.data.fd > 0 && close(event.data.fd) == -1)
		perror("close() in closeConnexion()");
}
/*----------------------------------------------------------------------------*/

/*	* close all server & client sockets
*/
void	Cluster::closeFdSet() const
{
	for (std::set<int>::iterator it = _serverSockets.begin(); it != _serverSockets.end(); it++)
		if (*it > 0 && close(*it) != 0)
			perror("close()");

	for (std::set<int>::iterator it = _clientSockets.begin(); it != _clientSockets.end(); it++)
		if (*it > 0 && close(*it) != 0)
			perror("close()");
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
							/*### EXCEPTIONS ###*/
/*============================================================================*/
void	Cluster::InitException::setSockExcept() const throw() {
	if (errno != 0)
		std::cerr << RED << strerror(errno) << ": ";
	if (retAddr != 0)
		std::cerr << RED << gai_strerror(retAddr) << ": ";
	
	std::cerr	<< YELLOW "at file [" << _file << "] line [" << _line << "]"
				<< RESET << std::endl;
}
/*----------------------------------------------------------------------------*/
