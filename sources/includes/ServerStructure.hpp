

#ifndef STRUCT_SERVER
# define STRUCT_SERVER

# include "webserv.hpp"

typedef struct s_location	t_location;
typedef struct s_params		t_params;

struct s_location
{
	s_location() {
		clear();
	}

	s_location(const s_location &ref) {
		*this = ref;
	}

	s_location & operator=(const s_location &ref)
	{
		if (this != &ref)
		{
			autoindex = ref.autoindex;
			cgipath = ref.cgipath;
			index = ref.index;
			path = ref.path;
			root = ref.root;
			methods = ref.methods;
		}
		return *this;
	}

	void clear()
	{
		autoindex = false;
		cgipath.clear();
		index.clear();
		path.clear();
		root.clear();
		methods.clear();
	}

	bool		autoindex;
	std::string	cgipath;
	std::string	index;
	std::string	path;
	std::string	root;
	std::set<std::string> methods;

};

struct s_params
{
	s_params() {
		clear();
	}
	
	s_params(const s_params & ref) {
		*this = ref;
	}

	s_params & operator=(const s_params &ref)
	{
		if (this != &ref)
		{
			maxBodySize = ref.maxBodySize;
			service = ref.service;
			methods = ref.methods;
			rootPath = ref.rootPath;
			nameList = ref.nameList;
			indexFile = ref.indexFile;
			errorPath = ref.errorPath;
			uploadPath = ref.uploadPath;
		}
		return *this;
	}

	void clear()
	{
		maxBodySize = 0;
		service.clear();
		methods.clear();
		rootPath.clear();
		nameList.clear();
		indexFile.clear();
		errorPath.clear();
		uploadPath.clear();
	}

	size_t					maxBodySize;
	std::string				uploadPath;
	std::string				errorPath;
	std::string				indexFile;
	std::string				rootPath;
	std::string				service;
	std::set<std::string>	methods;
	std::set<std::string>	nameList;

};

std::ostream & operator<<(std::ostream &o, const t_params &ref)
{
    o   << ITAL CYAN "params:\n"
        << "Max body size: " << ref.maxBodySize << std::endl
        << "Service: " << ref.service << std::endl
        << "Name list: ";
    
	std::set<std::string>::iterator it = ref.nameList.begin();
    for (; it != ref.nameList.end(); it++) {
        o   << *it << " ";
    }

    return o << RESET << std::endl;
}

std::ostream & operator<<(std::ostream &o, const t_location &ref)
{
    o   << ITAL BRIGHT_CYAN "Location:\n"
		<< "autoindex: " << ref.autoindex << std::endl
		<< "cgipath: " << ref.cgipath << std::endl
		<< "index: " << ref.index << std::endl
		<< "path: " << ref.path << std::endl
		<< "root: " << ref.root << std::endl
		<< "methods: ";
		
		std::set<std::string>::iterator it = ref.methods.begin();
		for (; it != ref.methods.end(); it++) {
			o << *it << " ";
		}

	return o << RESET << std::endl;
}

#endif