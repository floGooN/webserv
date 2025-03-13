

#ifndef REQUEST_STRUCT
# define REQUEST_STRUCT

# include "webserv.hpp"

typedef struct  t_header    s_header;
typedef struct  t_body      s_body;

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
    GET,
    POST,
    DELETE
};

struct t_header
{
    t_header() {
        this->clear();
    }

    t_header(const t_header &ref) {
        *this = ref;
    }

    t_header &  operator=(const t_header &ref)
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
        requestType = GET;        
    }

    bool            keepAlive;
	std::string	    uri;
	std::string	    hostName;
	std::string     hostPort;
	e_requestType	requestType;
};

struct t_body
{
	t_body() {
        this->clear();
	}

	t_body(const t_body &ref) {
		*this = ref;
	}

	t_body	&operator=(const t_body &ref)
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

	std::string     bound;
    std::string	    body;
    size_t  		contentLength;
    e_contentType   contentType;

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
    o   << ITAL CYAN "t_header:\n"
        << "Content type: " << ref.contentType
        << "Content length: " << ref.contentLength
        << "Boundary: " << ref.bound
        << RESET << std::endl;
}

#endif
