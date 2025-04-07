/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fberthou <fberthou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 05:11:55 by fberthou          #+#    #+#             */
/*   Updated: 2025/04/07 07:11:25 by fberthou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "ServerStructure.hpp"

class Client;
class ServerConfig;
class LocationConfig;

class Server
{
	public:
		Server(const ServerConfig &, const std::string&);
		Server(const Server &);
		~Server();
		Server	&operator=(const Server &);
		bool 	operator<(const Server &other) const;
		
		const t_params				&getParams() 		const;
		const std::set<t_location>	&getLocationSet()	const;

	private:
		const t_params				_params;
		const std::set<t_location>	_locationSet;

		void	setPath(const std::string &, t_location &);
		void	setRoot(const std::string &, t_location &);
		void	setIndex(const std::string &, t_location &);
		void	setService(const std::string &, t_params &);
		void	setRootPath(const std::string &, t_params &);
		void	setBodySize(const std::string &, t_params &);
		void	setErrorPath(const std::string &, t_params &);
		void	setIndexFile(const std::string &, t_params &);
		void	setCgiPath(const std::string &, t_location &);
		void	setUploadPath(const std::string &, t_params &);
		void	setRedir(const std::vector<std::string> &, t_location &);
		void	setNameList(const std::vector<std::string> &, t_params &);
		void	setAutoindex(const std::string &, t_location &) throw (std::invalid_argument);
		void	setMethod(const std::vector<std::string> &, t_params &) throw (std::invalid_argument);
		void	setMethods(const std::vector<std::string> &, t_location &) throw (std::invalid_argument);

		std::set<t_location>	setLocations(const std::vector<LocationConfig> &);
		t_params				setParams(const ServerConfig &, const std::string &);
};

std::ostream	& operator<<(std::ostream &, const Server &);

#endif
