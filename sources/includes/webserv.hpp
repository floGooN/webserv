
#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "colors"

# include <iostream>
# include <string>
# include <vector>
# include <cstdio>
# include <cstring>
# include <set>
# include <map>

/* -------------------------------------- */
/* ---    DEFINE GENERICS MESSAGES    --- */
/* -------------------------------------- */
# define ENOSERVICE	RED "none of the requested services are available\n" RESET
# define ERROR		"ERROR"
# define INFO		"INFO"
# define PRINTUSAGE  std::cout	<< YELLOW << "USAGE :" << std::endl \
								<< "  -> a path to a config file (.conf)" << std::endl \
								<< "  -> no argument and the default server is launched" << std::endl \
								<< RESET << std::endl
/* -------------------------------------- */
/* ---		    DEFINE PATH   	   	  --- */
/* -------------------------------------- */
# define PATH_DEFAULTSERV	"./config/default.conf"
# define PATH_ERRPAGE		"./error_pages"
# define PATH_MIME			"./config/mime.types"

/* -------------------------------------- */
/* ---		   DEFAULT VALUES  	   	  --- */
/* -------------------------------------- */

# define DFLT_TIMEOUT		65
# define DFLT_WORKCONNEX	"1024"
# define DFLT_BACKLOG		20
# define DFLT_BODYSIZE		1000
# define DFLT_URISIZE		2000
# define DFLT_METHOD		"GET POST DELETE"
# define DFLT_HOSTNAME		"localhost"
# define DFLT_MIME_TYPE		"application/octet-stream"
# define DFLT_CONTENT_TYPE	"application/x-www-form-urlencoded"
# define DFLT_PATH_PERL		"/usr/bin/perl"
# define DFT_PATH_PY		"/usr/bin/python3"

# define	DFLT_ERRORPAGE		"<!DOCTYPE html>" \
								"<html lang=\"fr\">" \
								"<head>" \
    							"<meta charset=\"UTF-8\">" \
    							"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" \
    							"<title>UNKNOW</title>" \
								"</head>" \
								"<body>" \
    							"<h1>UNKNOWN ERROR</h1>" \
								"</body>" \
								"</html>"

/* -------------------------------------- */
/* ---		   HTTP SPEC CHAR  	   	  --- */
/* -------------------------------------- */

# define HTTP_ALLOW_CHARS	"_:%.,\\/\"\'?!(){}[]@<>=-+*#$&`|^"\
							"ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
							"abcdefghijklmnopqrstuvwxyz" \
							"0123456789"

# define	HTTP_SEPARATOR	"\r\n"
# define	BODY_SEPARATOR	"\r\n\r\n"

/* -------------------------------------- */
/* ---    DEFINE ALL HTTP CODES	      --- */
/* -------------------------------------- */

# define	COD_200	"200 Ok"
# define	COD_201	"201 Created"
# define	COD_204	"204 No Content"

# define	ERR_400	"400 Bad Request"
# define	ERR_403	"403 Forbidden"
# define	ERR_404	"404 Not Found"
# define	ERR_405	"405 Method Not Allowed"
# define	ERR_408	"408 Request Time-out"
# define	ERR_413	"413 Request Entity Too Large"
# define	ERR_414	"414 Request-URI Too Long"
# define	ERR_415	"415 Unsupported Media Type"
# define	ERR_444	"444 No Response"
# define	ERR_499	"499 Client Closed Request"

# define	ERR_500	"500 Internal Server Error"
# define	ERR_501	"501 Not Implemented"
# define	ERR_502 "502 Bad Gateway"
# define	ERR_505	"505 HTTP Version not supported"
# define	ERR_520	"520 Unknown Error"

/* -------------------------------------- */
/* ---		    USEFULL VARS   	   	  --- */
/* -------------------------------------- */

# define	STATIC_BUFFSIZE	4096
# define	PROTOCOL_VERION	"HTTP/1.1"

/* -------------------------------------- */
/* ---		  STRUCT - ENUM   	   	  --- */
/* -------------------------------------- */

enum e_methods
{
    GET,
    POST,
    DELETE,
    EMPTY
};

enum e_contentType
{
    APP,
    TXT,
    MULTIPART
};

#endif
