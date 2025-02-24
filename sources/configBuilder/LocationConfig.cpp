

#include "LocationConfig.hpp"
#include "UtilParsing.hpp"

LocationConfig::LocationConfig(const LocationConfig &ref) {
    *this = ref;
}

std::ostream & operator<<(std::ostream &o, const LocationConfig & ref)
{
    o   << ITAL GREEN "LocationConfig: " << std::endl
        << "_path: " << ref._path << std::endl
        << "autoindex: " << ref.autoindex << std::endl
        << "_root: " << ref._root << std::endl
        << "_index: " << ref._index << std::endl
        << "_cgipath: " << ref._cgipath << std::endl
        << "_methods: ";
    for (std::vector<std::string>::const_iterator it = ref._methods.begin();
        it != ref._methods.end(); it++)
        o << *it << " ";
    return o << RESET;
}

LocationConfig & LocationConfig::operator=(const LocationConfig &ref)
{
    if (this != &ref)
    {
        _path = ref._path;
        autoindex = ref.autoindex;
        _root = ref._root;
        _index = ref._index;
        _cgipath = ref._cgipath;
        _methods = ref._methods;
    }
    return *this;
}

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
    std::cout << "autoindex in control: [" << autoindex << "]" << std::endl;
    if (autoindex.empty())
        autoindex = "off";
    if (_root.empty())
        _root = "none";
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
