
#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "colors"
// # include "ErrorHandler.hpp"

# include <iostream>
# include <string>
# include <vector>
# include <cstdio>
# include <set>
# include <map>

/* -------------------------------------- */
/* --- DEFINE GENERIC ERROR MESSAGES  --- */
/* -------------------------------------- */
# define ENOSERVICE	   "\033[31mnone of the requested services are available\n"
# define PRINTUSAGE  std::cout   << YELLOW "USAGE :" << std::endl \
						<< "	-> a path to a file config (.conf)" << std::endl \
						<< "	-> no argument and default server is launch" \
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

# define DFLT_TIMEOUT		"65"
# define DFLT_WORKCONNEX	"1024"
# define DFLT_BACKLOG		20
# define DFLT_BODYSIZE		1000
# define DFLT_METHOD		"GET POST DELETE"
# define DFLT_HOSTNAME		"localhost"
# define DFLT_MIME_TYPE		"application/octet-stream"
# define DFLT_CONTENT_TYPE	"application/x-www-form-urlencoded"

/* -------------------------------------- */
/* ---		   OTHER VALUES  	   	  --- */
/* -------------------------------------- */

# define HTTP_ALLOW_CHARS	"_:%.,\\/\"\'?!(){}[]@<>=-+*#$&`|^"\
							"ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
							"abcdefghijklmnopqrstuvwxyz" \
							"0123456789"

# define	DFLT_ERRORPAGE		"<!DOCTYPE html>" \
								"<html lang=\"fr\">" \
								"<head>" \
    							"<meta charset=\"UTF-8\">" \
    							"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" \
    							"<title>404 - Page non trouvée</title>" \
								"</head>" \
								"<body>" \
    							"<h1>UNKNOWN ERROR</h1>" \
								"</body>" \
								"</html>"

#endif
