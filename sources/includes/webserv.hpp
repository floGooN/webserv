
#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "colors"


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
# define PATH_DEBUGSERR		"./config/debug.conf"
# define PATH_ERRPAGE		"./error_pages"
# define PATH_MIME			"./config/mime.types"

/* -------------------------------------- */
/* ---		   DEFAULT VALUES  	   	  --- */
/* -------------------------------------- */

# define DFLT_TIMEOUT		65
# define DFLT_WORKCONNEX	1024
# define DFLT_BACKLOG		20
# define DFLT_METHOD		"GET POST DELETE"
# define DFLT_HOSTNAME		"localhost"
# define DFLT_TYPE			"application/octet-stream"

# ifndef DFLT_LISTENPORT
	# define DFLT_LISTENPORT	"8000"
#endif

#endif
