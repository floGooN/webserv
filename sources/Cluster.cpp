


/*============================================================================*/
								/*### HEADERS ###*/
/*============================================================================*/
#include "Cluster.hpp"
#include "ConfigParser.hpp"

#include <sys/epoll.h>
#include <fcntl.h>
#include <netdb.h>

/*	* ressources provisoirs
*/
#include <csignal>
#include <sstream>

#define MAXEVENT	10
#define BUFFERSIZE	2048

int g_runServer = 0;

void hand(int, siginfo_t *, void *) {
    g_runServer = 0;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
				/*### CONSTRUCTORS - DESTRUCTOR - OVERLOAD OP ###*/
/*============================================================================*/

Cluster::Cluster(const std::string &filepath)
  : _config(ConfigParser().parse(filepath))
{
	setServersByPort();
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

Cluster::~Cluster() {
	_epollFd > 0 ? close(_epollFd) : _epollFd;
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
		it != ref.getServersByPort().end(); it++)
			o	<< it->second << std::endl;
	return o << RESET;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

std::map<std::string, Server> & Cluster::getServersByPort() const {
	return const_cast<std::map<std::string, Server> & >( _serversByService);
}
/*----------------------------------------------------------------------------*/

const HttpConfig & Cluster::getConfig() const {
	return _config;
}
/*----------------------------------------------------------------------------*/

/*	* server main loop
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
						else if (events[i].events & (EPOLLHUP | EPOLLRDHUP))
							closeConnexion(events[i]);
						else
							std::cerr << "have to print EPOLLERR" << std::endl;
					}
				}
				catch(const std::exception& e) {
					std::cerr << e.what() << std::endl; // error page builder exception
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
Client * Cluster::addClient(const Request &req, const int fdClient)
{
	Server *current = NULL;
	try {
		current = &getServersByPort().at(req.gethostport());
	}
	catch(const std::exception& e) {
		throw std::runtime_error("addClient(): error 404\n"); // ressource not found
	}

	try {
		std::pair<std::map<int, Client>::iterator, bool> it;
		
		it = current->getClientList().insert(std::pair<int, Client>(fdClient, Client(req)));
		it.first->second.clientServer = current;
		
		return &it.first->second;
	}
	catch(const std::exception& e) {
		std::cerr << RED " Error\nadding client to list\n" RESET << e.what();
		throw std::runtime_error("addClient(): error 500\n");
	}
	throw std::runtime_error("addClient(): error 500\n");
}
/*----------------------------------------------------------------------------*/

/*	* Search for a client and return a pointer to it	
    * If no client is found, return a null pointer
	* Don't throw an exception
*/
Client *	Cluster::findClient(int fdClient)
{
	std::map<std::string, Server>::iterator itServer = _serversByService.begin();
	while (itServer != _serversByService.end())
	{
		try {
			Client *client = &itServer->second.getClientList().at(fdClient);
			client->clientServer = &itServer->second;
			return client;
		}
		catch(const std::exception& e) {
			;
		}
		itServer++;
	}
	return NULL;
}
/*----------------------------------------------------------------------------*/

/*	* get BUFFERSIZE octets of the message from client
	* append message safety
	* return nb bytes received (for exit loop)
	* throw error with correct page if an error occured
*/
ssize_t	Cluster::safeRecv(const int clientFd, std::string &message)
{
	char	buffer[BUFFERSIZE] = {'\0'};
	ssize_t	bytesReceived = recv(clientFd, buffer, BUFFERSIZE, 0);
	
	if (bytesReceived == -1)
		throw std::runtime_error("safeRecv(): bytesReceived == -1 error 500\n"); // throw error 500 (internal error)
	if (bytesReceived == 0)
		throw std::runtime_error("safeRecv(): bytesReceived == 0  error 499\n"); // throw error 499 (client closed connexion)
	
	try {
		message.empty() ? message.assign(buffer, bytesReceived) : message.append(buffer, bytesReceived);
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		throw std::runtime_error("safeRecv(): error 500\n");
	}
	return bytesReceived;
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
	Client		*currentClient = findClient(event.data.fd);
	ssize_t		bytesReceived = BUFFERSIZE;
	std::string	message("\0");

	while (bytesReceived == BUFFERSIZE)
	{
		bytesReceived = safeRecv(event.data.fd, message);
		if (!currentClient)
		{
			currentClient = addClient(Request(message), event.data.fd);
			if (currentClient->request.getcontentlength() > currentClient->clientServer->getMaxBodySize())
				throw std::runtime_error("recvData(): error 413 Request Entity Too Large\n"); // Request Entity Too Large
		}
		else
		{
			if (currentClient->request.gettype().empty() == false) {
				try {
					currentClient->request.setBody(message);
				}
				catch(const std::exception& e) {
					std::cerr << e.what() << '\n';
					std::runtime_error("recvData(): error 500\n");
				}
			}
			else
				currentClient->request = Request(message);
		}
		
		currentClient->request.totalBytesReceived += static_cast<size_t>(bytesReceived);
		if (currentClient->request.totalBytesReceived > currentClient->clientServer->getMaxBodySize())
			throw std::runtime_error("recvData(): error 413 Request Entity Too Large\n"); // Request Entity Too Large
	}
	
	if (currentClient->request.getcontentlength() == currentClient->request.getbody().size())
	{
		currentClient->responseFormating(event.data.fd);
		try {
			changeEventMod(false, event.data.fd);
		}
		catch(const RunException& e) {
			e.runExcept();
			closeConnexion(event);
			std::runtime_error("recvData(): error 500\n");
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


	char buff[BUFFERSIZE] = {'\0'};
	memset(buff, '\0', sizeof(buff));
	int fd = open("./website/devis.com/formulaires/formUpload.html"/* "./googleIndex.html"*/, O_RDONLY);

	if (fd == -1)
		perror("OPENTEST");

	std::string response("\0");
	while (true) {
		ssize_t ret = read(fd, buff, 1);
		if (ret == 0)
			break;
		if (ret == -1){
			perror("read() in sendData");
			return;
		}
		response.append(buff, strlen(buff));
	}
	std::ostringstream convert;
	convert << response.length();
	response.insert(0, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + convert.str() + "\r\n\r\n");

	// std::cout << "RESPONSE :\n" << response << std::endl;
	Client	*client = findClient(event.data.fd);
	if (!client)
		std::cerr << RED "NO CLIENT" RESET << std::endl;

	while (client->request.totalBytessended != response.size())
	{
		// std::cout << "IN SENDATA:\n" << *client << std::endl;
		ssize_t ret = send(event.data.fd, response.c_str(), response.length(), 0);
		if (ret <= 0)
			break;
		client->request.totalBytessended += ret;
	}
	
	if (client->request.getkeepalive() == true && \
		client->request.totalBytesReceived == client->request.totalBytessended)
	{
		try {
			changeEventMod(true, event.data.fd);
		}
		catch(const RunException& e) {
			e.runExcept();
			closeConnexion(event);
			throw;
		}
	}
	else if(client->request.totalBytesReceived == client->request.totalBytessended)
		closeConnexion(event);
}
/*----------------------------------------------------------------------------*/

/*	* init servers from _serverconfig
*/
void Cluster::setServersByPort()
{
	std::vector<ServerConfig>::iterator	itConfigServer = _config.serversConfig.begin();
	std::pair<std::map<std::string, Server>::iterator, bool>	result;

	while (itConfigServer != _config.serversConfig.end())
	{
		std::vector<std::string>::iterator	itServiceList = itConfigServer->listenPort.begin();
		while (itServiceList != itConfigServer->listenPort.end())
		{
			result = _serversByService.insert(std::make_pair(*itServiceList, Server(*itConfigServer, *itServiceList)));
			if (!result.second)
			{
				std::cerr << YELLOW "Port [" << *itServiceList << "] still required by server "
						  << *(result.first->second.getNameList().begin())
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
	*
*/
void	Cluster::acceptConnexion(const struct epoll_event &event) const
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
	try {
		if (clientSocket < 0)
			throw RunException(__FILE__, __LINE__ , "Error accept():");
	}
	catch(const RunException& e) {
		e.runExcept();
		throw std::runtime_error("error 500 in acceptConnexion\n");
	}

	try {
		if (fcntl(clientSocket, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
			throw RunException(__FILE__, __LINE__, "Error fcntl():");
		addFdInEpoll(false, clientSocket);
	}
	catch(const RunException& e)
	{
		if (close(clientSocket) == -1)
			perror("close() in acceptConnexion()");
		std::cerr << e.what() << '\n';
		throw std::runtime_error("error 500 in accept connexion\n");
	}
}
/*----------------------------------------------------------------------------*/

/*	* close a client connexion
*/
void	Cluster::closeConnexion(const struct epoll_event &event) const
{
#ifdef TEST
	std::cout	<< BOLD BRIGHT_PURPLE "\nFunction -> closeConnexion()\n"
				<< "Client fd [" RESET PURPLE << event.data.fd
				<< BOLD BRIGHT_PURPLE "]" RESET
				<< std::endl;
#endif
	
	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, event.data.fd, NULL) == -1)
		perror("epoll_ctl() in closeConnexion()");

	if (event.data.fd > 0 && close(event.data.fd) == -1)
		perror("close() in closeConnexion()");
}
/*----------------------------------------------------------------------------*/

/*	* init epoll events for servers & new client socket and add the fd in epoll set
*/
void	Cluster::addFdInEpoll(const bool isServerSocket, const int fd) const
{
	struct epoll_event	ev;
	memset(&ev, 0, sizeof(ev));

	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
	
	if (isServerSocket == false)
		ev.events |= EPOLLET;
	
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &ev) == -1)
		throw RunException(__FILE__, __LINE__ - 1, "Error epoll_ctl() in addFdInEpoll() :"); // throw 500 internal error
}
/*----------------------------------------------------------------------------*/

/*	* switch events mode between EPOLLOUT and EPOLLIN
*/
void	Cluster::changeEventMod(const bool changeForRead, const int fd) const
{
	struct epoll_event	ev;
	memset(&ev, 0, sizeof(ev));
	
	ev.data.fd = fd;
	ev.events = EPOLLET | EPOLLHUP | EPOLLRDHUP | (changeForRead ? EPOLLIN : EPOLLOUT);
	
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev) == -1)
		throw RunException(__FILE__, __LINE__ - 1, "Error epoll_ctl() in changeEventMod():");
}
/*----------------------------------------------------------------------------*/

/*	* close all server sockets
*/
void	Cluster::closeFdSet() const
{
	for (std::set<int>::iterator it = _serverSockets.begin(); it != _serverSockets.end(); it++)
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

void	Cluster::RunException::runExcept() const throw() {
	std::cerr << RED "Error" << std::endl;
	if (errno != 0)
		std::cerr << strerror(errno) << ": ";
	std::cerr	<< YELLOW "at file [" << _file << "] line [" << _line << "]"
				<< RESET << std::endl;
}
/*----------------------------------------------------------------------------*/
