#ifndef IPARSING_HPP
# define IPARSING_HPP

# include "webserv.hpp"
# include "ServerStructure.hpp"

# include "ErrorHandler.hpp"

# include <unistd.h>
# include <dirent.h>

# include <sstream>
# include <fstream>


class Utils
{
	public:
		virtual ~Utils() = 0;

		template<typename T>
		static void	convertVectorToSet(std::set<T> &dest, const std::vector<T> &src) {
			for (typename std::vector<T>::const_iterator it = src.begin();
				it != src.end(); it++)
				dest.insert(*it);
		}

		static bool	isOnlySpace(const std::string & str);
		static bool fileExits(const std::string &filename);
		static bool	isDirectory(const std::string &) throw (ErrorHandler);

		static void	safeCloseDirectory(DIR *);
		static void	displayVector(std::vector<std::string> vec);
		static void	checkAccessRessource(const std::string &, int);
		static void	printLog(const std::string& level, const std::string& message);
		static void	readErrorFile(const std::string &filepath, std::string &buffer);
		static void	readFile(const std::string &filepath, std::string &buffer) throw (ErrorHandler);
		static void	printMapVector(const std::map<int, std::map<std::string, std::vector<std::string> > >& allMapRoads);
		static void	manageControlMapLocation(std::map<int, std::map<std::string, std::vector<std::string> > > allMapRoads);
		static void	controlMapLocation(std::map<int, std::map<std::string, std::vector<std::string> > > allMapRoads, std::string keyValue);
		
		static std::string	intToString(int value);
		static std::string	trim(const std::string& str);
		static std::string	emethodsTypeToString(e_methods type);
		static std::string	convertHexaToString(std::string value);
		static std::string	trimSemicolon(const std::string& str);
		static std::string	econtentTypeToString(e_contentType type);
		static std::string	recoverExtension(const std::string &filename);
		static std::string	recoverValue(std::string line, std::string key);

		static int				decryptHexa(std::string value);
		static size_t			convertBodySize(const std::string &);
		static size_t			safeMultiply(size_t value, size_t factor);
		static DIR				*openDirectory(const std::string &dirPath);
		static const t_location	*findLocation(const std::set<t_location> &, const std::string &);

		static std::vector<std::string>	cleanVector(const std::vector<std::string> vec);
        static std::vector<std::string>	cleanVectorClose(const std::vector<std::string> vec);
        static std::vector<std::string>	split(const std::string & line, const std::string & set); 
		static std::vector<std::string>	splitSpecialDeleteKey(const std::string & line, const std::string & set);
	private:
		Utils() {};
};

#endif
