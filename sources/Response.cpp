










/*============================================================================*/
						/*### HEADERS & STATIC FIELD ###*/
/*============================================================================*/

#include "Response.hpp"
#include "Client.hpp"
#include "UtilParsing.hpp"

/*============================================================================*/
				/*### CONSTRUCTORS - DESTRUCTOR - OVERLOAD OP ###*/
/*============================================================================*/

Response::Response()
{
	completeUri.clear();
	finalMessage.clear();
}
/*----------------------------------------------------------------------------*/

Response::Response(const Response &ref) {
	*this = ref;
}
/*----------------------------------------------------------------------------*/

Response::~Response()
{	}
/*----------------------------------------------------------------------------*/

Response &Response::operator=(const Response &ref)
{
	if (this != &ref)
	{
		completeUri = ref.completeUri;
		finalMessage = ref.finalMessage;
	}
	return *this;
}
/*----------------------------------------------------------------------------*/

std::ostream & operator<<(std::ostream &o, const Response &)
{
	// o	<< ""
	return o;
}
/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PUBLIC METHODS ###*/
/*============================================================================*/

/*----------------------------------------------------------------------------*/

/*============================================================================*/
						/*### PRIVATE METHODS ###*/
/*============================================================================*/

void	Response::getRequest(const Request &)
{
	std::cout << BRIGHT_GREEN "GET QUERY" << RESET << std::endl;
}
/*----------------------------------------------------------------------------*/

void	Response::postRequest(const Request &request)
{
	std::cout	<< BRIGHT_YELLOW "POST QUERY" << std::endl
				<< "bodySize: " << request.getbody().size() << std::endl
				<< "request:\n" RESET << const_cast<Request&>(request) << std::endl;
}
/*----------------------------------------------------------------------------*/

void	Response::deleteRequest(const Request &)
{
	std::cout << BRIGHT_CYAN "DELETE QUERY" << RESET << std::endl;
}
/*----------------------------------------------------------------------------*/

void	Response::clearResponse()
{
	completeUri.clear();
	finalMessage.clear();
}
/*----------------------------------------------------------------------------*/
