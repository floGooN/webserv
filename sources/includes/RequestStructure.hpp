

#ifndef STRUCT_REQUEST
# define STRUCT_REQUEST

# include "webserv.hpp"

typedef struct  s_header        t_header;
typedef struct  s_body          t_body;

/*	* 3 types value possible :
	*
	* APP : application/x-www-form-urlencoded
		-> Default setting. All characters are encoded before sent
			.spaces are converted to "+" symbols
			.special characters are converted to ASCII HEX values (ex: %20 == ASCII ' ' 32)
	* MULTIPART : multipart/form-data
		-> necessary if the user will upload a file through the form
			. body is detached from url
	* TXT : text/plain
		-> Sending data without any encoding at all. Not recommended
	
	for each of them, data is in a specific format in the request
*/
enum e_contentType
{
    APP,
    TXT,
    MULTIPART
};

enum e_requestType
{
    ERROR = -1,
    GET,
    POST,
    DELETE,
    EMPTY
};

struct s_header
{
    s_header() {
        this->clear();
    }

    s_header(const s_header &ref) {
        *this = ref;
    }

    s_header &  operator=(const s_header &ref)
    {
        if (this != &ref)
        {
            keepAlive = ref.keepAlive;
            uri = ref.uri;
            hostName = ref.hostName;
            hostPort = ref.hostPort;
            requestType = ref.requestType;
        }
        return *this;
    }

    void    clear()
    {
        keepAlive = false;
        uri.clear();
        hostName.clear();
        hostPort.clear();
        requestType = EMPTY;        
    }

    bool            keepAlive;
	std::string     uri;
	std::string     hostName;
	std::string     hostPort;
	e_requestType   requestType;
};

struct s_body
{
	s_body() {
        this->clear();
	}

	s_body(const s_body &ref) {
		*this = ref;
	}

	s_body	&operator=(const s_body &ref)
    {
		if (this != &ref)
        {
			body = ref.body;
			bound = ref.bound;
            contentLength = ref.contentLength;
            contentType = ref.contentType;
		}
		return *this;
	}

    void    clear()
    {
		body.clear();
		bound.clear();
        contentLength = 0;
        contentType = APP;
    }

    std::string	    body;
	std::string     bound;
    e_contentType   contentType;
    size_t  		contentLength;

};

std::ostream & operator<<(std::ostream &o, const t_header &ref)
{
    o   << ITAL BLUE "t_header:\n"
        << "URI: " << ref.uri
        << "Request type: " << ref.requestType
        << "HostPort: " << ref.hostPort
        << "HostName: " << ref.hostName
        << "Keep-alive: " << ref.keepAlive
        << RESET << std::endl;
}

std::ostream & operator<<(std::ostream &o, const t_body &ref)
{
    o   << ITAL BRIGHT_BLUE "t_header:\n"
        << "Content type: " << ref.contentType
        << "Content length: " << ref.contentLength
        << "Boundary: " << ref.bound
        << RESET << std::endl;
}

#endif
