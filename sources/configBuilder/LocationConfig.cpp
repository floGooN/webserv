

#include "LocationConfig.hpp"
#include "UtilParsing.hpp"
#include <unistd.h>

LocationConfig::LocationConfig()
{
	path.clear();
	autoindex.clear();
	root.clear();
	index.clear();
	cgipath.clear();
	methods.clear();
	redirect.clear();
}

LocationConfig::LocationConfig(const LocationConfig &ref) {
	*this = ref;
}

std::ostream & operator<<(std::ostream &o, const LocationConfig & ref)
{
	o   << ITAL GREEN "LocationConfig: " << std::endl
		<< "path: " << ref.path << std::endl
		<< "autoindex: " << ref.autoindex << std::endl
		<< "root: " << ref.root << std::endl
		<< "index: " << ref.index << std::endl
		<< "cgipath: " << ref.cgipath << std::endl
		<< "methods: ";
	for (std::vector<std::string>::const_iterator it = ref.methods.begin();
		it != ref.methods.end(); it++)
		o << *it << " ";
	for (std::vector<std::string>::const_iterator iter = ref.redirect.begin();
		iter != ref.redirect.end(); iter++)
		o << *iter << " ";
	return o << RESET << std::endl;
}

LocationConfig & LocationConfig::operator=(const LocationConfig &ref)
{
	if (this != &ref)
	{
		path = ref.path;
		autoindex = ref.autoindex;
		root = ref.root;
		index = ref.index;
		cgipath = ref.cgipath;
		methods = ref.methods;
		redirect = ref.redirect;
	}
	return *this;
}

bool	LocationConfig::operator<(const LocationConfig &ref) const {
	return path < ref.path;
}

void	LocationConfig::controlDefaultLocationConf()
{
	if (path.empty())
		throw std::invalid_argument("a path should never not be empty in a location description\nlocation [PATH]\n{\n\r**description**\n}");
}

void	LocationConfig::checkSemiColonLocation()
{
	if (path.find(";") != std::string::npos)
		path = UtilParsing::trimSemicolon(path);
	if (root.find(";") != std::string::npos)
		root = UtilParsing::trimSemicolon(root);
	if (index.find(";") != std::string::npos)
		index = UtilParsing::trimSemicolon(index);
	for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); it++)
	{
		if (it->find(";") != std::string::npos)
			*it = UtilParsing::trimSemicolon(*it);
	}
	for (std::vector<std::string>::iterator iter = redirect.begin(); iter != redirect.end(); iter++)
	{
		if (iter->find(";") != std::string::npos)
			*iter = UtilParsing::trimSemicolon(*iter);
	}
}
