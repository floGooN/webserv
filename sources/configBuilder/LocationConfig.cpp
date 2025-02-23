

#include "LocationConfig.hpp"
#include "UtilParsing.hpp"

void	LocationConfig::displayLocation()
{
    std::cout   << BOLD BRIGHT_CYAN "LOCATION FIELD:\n"
                << BRIGHT_CYAN "_path:		" RESET CYAN << _path << std::endl
                << BRIGHT_CYAN "_root:		" RESET CYAN << _root << std::endl
                << BRIGHT_CYAN "_index:		" RESET CYAN << _index << std::endl
                << BRIGHT_CYAN "_methods:	" RESET;
    UtilParsing::displayVector(_methods);
}

void	LocationConfig::controlDefaultLocationConf()
{
    if (_path.empty())
        throw std::invalid_argument("'location' must not be empty. Put the keyword (in quotes) followed by its value(s) separated by a space.");
    if (_root.empty())
        throw std::invalid_argument("'root' must not be empty. Put the keyword (in quotes) followed by its value(s) separated by a space.");
    if (_index.empty())
        throw std::invalid_argument("'index' must not be empty. Put the keyword (in quotes) followed by its value(s) separated by a space.");
    if (_methods.empty())
        throw std::invalid_argument("'methods_accept' must not be empty. Put the keyword (in quotes) followed by its value(s) separated by a space.");
}

void	LocationConfig::checkSemiColonLocation()
{
    if (_path.find(";") != std::string::npos)
        _path = UtilParsing::trimSemicolon(_path);
    if (_root.find(";") != std::string::npos)
        _root = UtilParsing::trimSemicolon(_root);
    if (_index.find(";") != std::string::npos)
        _index = UtilParsing::trimSemicolon(_index);
    for (std::vector<std::string>::iterator it = _methods.begin(); it != _methods.end(); it++)
    {
        if (it->find(";") != std::string::npos)
            *it = UtilParsing::trimSemicolon(*it);
    }
}

std::ostream & operator<<(std::ostream &o, const LocationConfig & ref)
{
    o   << "LocationConfig: " << std::endl
        << "_index" << ref._index << std::endl
        << "_root: " << ref._root << std::endl
        << "_path: " << ref._path << std::endl
        << "_methods: ";
    for (std::vector<std::string>::const_iterator it = ref._methods.begin();
        it != ref._methods.end(); it++)
        o << *it << " ";
    o   << std::endl;
    return o;
}
