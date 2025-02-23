
#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

#include "webserv.hpp"

class LocationConfig 
{
public:
    std::string _path;
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
