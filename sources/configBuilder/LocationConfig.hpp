
#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

#include "webserv.hpp"

class LocationConfig 
{
    public:
        LocationConfig();
        LocationConfig(const LocationConfig &ref);
        LocationConfig  & operator=(const LocationConfig &);
        bool            operator<(const LocationConfig &) const;
        
        std::string path;
        std::string root;
        std::string index;
        std::string cgipath;
        std::string autoindex;
        std::vector<std::string> methods;
        std::vector<std::string> redirect;

        void controlDefaultLocationConf();
        void checkSemiColonLocation();
};

std::ostream & operator<<(std::ostream &, const LocationConfig &ref);

#endif
