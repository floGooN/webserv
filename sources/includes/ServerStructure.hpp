

#ifndef STRUCT_SERVER
# define STRUCT_SERVER

# include "webserv.hpp"

typedef struct t_location
{

} s_location;

typedef struct t_params
{
	t_params()
	{
		maxBodySize = 0;
		service.clear();
		nameList.clear();
	}
	
	t_params & operator=(t_params &ref)
	{
		maxBodySize = ref.maxBodySize;
		service = ref.service;
		nameList = ref.nameList;
	}

	size_t					maxBodySize;
	std::string				service;
	std::set<std::string>	nameList;

} s_params;

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
    o   << ITAL BRIGHT_CYAN "Location:\n";

	return o << RESET;
}

#endif