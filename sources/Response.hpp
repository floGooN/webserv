
/*  * List of header fields 
    * 
    * GENERAL FIELDS
    * Date:         -> date and hour when the message is sended
    * Server:       -> name of the server used
    * Connection:   -> 2 values : keep-alive or close
                    -> define if connection is close or not after sending message
    * 
    * CONTENT INFORMATION FIELD
    * Content-Type:         -> text/html; charset=UTF-8
    * Content-length:       -> body size in bytes
    * Content-Encoding:     -> indicate the compression method
    * Content-Disposition:  -> Indicates whether the content should be downloaded and suggests a file name
    * Last-Modified:        -> Last modification of the ressource
    * ETag:                 -> hash of version of the ressource
    * 
    * CACHE CONTROL FIELD
    * Cache-Controle:       -> Sets caching directives (ex: no-cache)
    * Expire:               -> indicate the date of the expiration of the validity of the ressource
    * 
    * REDIRECTION FIELD
    * Location:     -> indicate the new url for the ressource fetched (redirection 3xx)
    * 
    * SECURITY FIELD
    * Strict-Transport-Security:    -> HTTPS force (STL) (ex: max-age=31536000; includeSubDomains)
    * Set-Cookie:                   -> set a cookie for client (ex: Set-Cookie: sessionId=abc123; Path=/; Secure; HttpOnly)
    * and other see RFC
    * 
    * DIVERS FIELDS
    * Allow:        -> indicate methods allows on the ressources

*/

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

class Request;

typedef struct s_httpHeader t_httpHeader;

struct s_httpHeader
{
    std::string _date;          // of the sending message (fill in sendData)
    std::string _server;        // server software name
    std::string _connection;    // keep-alive or not
    
    std::string _contentType;   // mime + encoding (; charset=UTF-8)
    std::string _contentLength; // body size
    std::string _contentDisposition;    // if it's a downloaded ressource (like pdf)

    std::string _cacheControle; // set at no-cache
    
    std::string _allow;         // list of the allowed methods on the ressource
    std::string _redirection;   // new url (new location of the page fetched)
};

class Response
{
	public:
		Response(const Request &);
		Response(const Response &);
		~Response();
		Response &operator=(const Response &);
		
	private:
    t_httpHeader    _header;
    std::string     _uriReconstituated;   
};

#endif
