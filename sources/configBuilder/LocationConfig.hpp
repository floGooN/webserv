
#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

#include "webserv.hpp"

class LocationConfig 
{
    public:
        LocationConfig() {
            _path.clear();
            autoindex.clear();
            _root.clear();
            _index.clear();
            _cgipath.clear();
            _methods.clear();
        }
        LocationConfig(const LocationConfig &ref);
        LocationConfig & operator=(const LocationConfig &);
        
        std::string _path;
        std::string autoindex;
        std::string _root;
        std::string _index;
        std::string _cgipath;
        std::vector<std::string> _methods;

        void displayLocation();
        void controlDefaultLocationConf();
        void checkSemiColonLocation();
};

std::ostream & operator<<(std::ostream &, const LocationConfig &ref);

#endif
