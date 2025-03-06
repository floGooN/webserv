#ifndef IPARSING_HPP
# define IPARSING_HPP

# include "webserv.hpp"
#include <sys/stat.h>
#include <sstream>
#include <fstream>

class UtilParsing
{
	public:
		virtual ~UtilParsing() = 0;

		template<typename T>
		static void	convertVectorToSet(std::set<T> &dest, const std::vector<T> &src) {
			for (typename std::vector<T>::const_iterator it = src.begin();
				it != src.end(); it++)
				dest.insert(*it);
		}
		template<typename T>
		static void	deepCopieSet(std::set<T> &dest, const std::set<T> &src) {
			for (typename std::set<T>::const_iterator it = src.begin();
				it != src.end(); it++)
					dest.insert(*it);
		}
		template<typename T>
		static void	deepCopieVector(std::vector<T> &dest, const std::vector<T> &src) {
			for (typename std::vector<T>::const_iterator it = src.begin();
				it != src.end(); it++)
				dest.push_back(*it);
		}

		static bool	isOnlySpace(const std::string & str);
		static void	displayVector(std::vector<std::string> vec);
		static void	printMapVector(const std::map<int, std::map<std::string, std::vector<std::string> > >& allMapRoads);
		static void	manageControlMapLocation(std::map<int, std::map<std::string, std::vector<std::string> > > allMapRoads);
		static void	controlMapLocation(std::map<int, std::map<std::string, std::vector<std::string> > > allMapRoads, std::string keyValue);
		
		static std::string	recoverExtension(const std::string &filename);
		static std::string	intToString(int value);
		static std::string	trim(const std::string& str);
		static std::string	trimSemicolon(const std::string& str);
		static std::string	recoverValue(std::string line, std::string key);

		static bool		fileExits(const std::string &filename);
		static bool		directoryExists(const std::string &dirname);
		static size_t	safeMultiply(size_t value, size_t factor);
		static size_t	convertBodySize(const std::string &);

		static std::vector<std::string>	cleanVector(const std::vector<std::string> vec);
        static std::vector<std::string>	cleanVectorClose(const std::vector<std::string> vec);
        static std::vector<std::string>	split(const std::string & line, const std::string & set); 
		static std::vector<std::string>	splitSpecialDeleteKey(const std::string & line, const std::string & set);
		static std::string convertHexaToString(std::string value);
		static int decryptHexa(std::string value);
	private:
		UtilParsing() {};
};

#endif
