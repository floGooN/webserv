

#include "UtilParsing.hpp"

#include <fcntl.h>

#include <sstream>
#include <cstring>
#include <limits>

#define BUFFER_SIZE	4096

size_t	UtilParsing::safeMultiply(size_t value, size_t factor) {
	if (value > std::numeric_limits<size_t>::max() / factor) {
		throw std::overflow_error("Multiplication overflow");
	}
	return value * factor;
}

size_t	UtilParsing::convertBodySize(const std::string& strBodySize)
{
	size_t result;
	size_t idx = strBodySize.find_first_not_of("0123456789");
		
	if (idx == std::string::npos || idx == 0) {
		throw std::runtime_error("invalid bodySize");
	}

	std::stringstream ss(strBodySize.substr(0, idx));
	if (!(ss >> result)) {
		throw std::runtime_error("Error stringstream conversion\n");
	}

	char unit = strBodySize[idx];
	switch (unit)
	{
		case 'K': // Kilobytes -> result * 1024
			result = safeMultiply(result, 1024);
			break;
		case 'M': // Megabytes -> result * 1024 * 1024
			result = safeMultiply(result, 1024);
			result = safeMultiply(result, 1024);
			break;
		case 'G': // Gigabytes -> result * 1024 * 1024 * 1024
			result = safeMultiply(result, 1024);
			result = safeMultiply(result, 1024);
			result = safeMultiply(result, 1024);
			break;
		default:
			break;
	}
	return result;
}

std::vector<std::string>	UtilParsing::split(const std::string & line, const std::string & set)
 {
	std::vector<std::string> result;

	for (size_t i = 0; i != std::string::npos;)
	{
		size_t end = line.find_first_of(set, i);

		result.push_back(line.substr(i, end - i));
		if (result.back().empty() || UtilParsing::isOnlySpace(result.back()))
			result.pop_back();
		end != std::string::npos ? i = end + 1 : i = end;
	}
	return result; 
}

std::vector<std::string>	UtilParsing::splitSpecialDeleteKey(const std::string & line, const std::string & set)
 {
	std::vector<std::string> result;

	for (size_t i = 0; i != std::string::npos;) 
	{
		size_t end = line.find_first_of(set, i);
		result.push_back(line.substr(i, end - i));
		if (result.back().empty() || UtilParsing::isOnlySpace(result.back()))
			result.pop_back();
		end != std::string::npos ? i = end + 1 : i = end;
	}
	if (!result.empty())
		result.erase(result.begin());
	return result; 
}

std::vector<std::string>	UtilParsing::cleanVector(std::vector<std::string> vec)
{
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ) {
		if (*it == "{" || *it == "}") { 
			it = vec.erase(it);
		} 
		else 
			++it;
	}
	return vec;
}

std::vector<std::string>	UtilParsing::cleanVectorClose(std::vector<std::string> vec)
{
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ) {
		if (*it == "}") { 
			it = vec.erase(it);
		} else 
			++it;
	}
	return vec;
}

bool	UtilParsing::isOnlySpace(const std::string & str)
{
	size_t  i;
	size_t  size = str.size();

	for (i = 0; i < size; i++) {
		if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n') {
			return (i == size);
		}
	}
	return (i == size);
}

void	UtilParsing::printMapVector(const std::map<int, std::map<std::string, std::vector<std::string> > >& allMapRoads)
{
	for (std::map<int, std::map<std::string, std::vector<std::string> > >::const_iterator outerIt = allMapRoads.begin(); 
		outerIt != allMapRoads.end(); ++outerIt)
	{
		int index = outerIt->first;
		const std::map<std::string, std::vector<std::string> >& innerMap = outerIt->second;

		std::cout << "Index: " << index << std::endl;
		for (std::map<std::string, std::vector<std::string> >::const_iterator innerIt = innerMap.begin(); 
			 innerIt != innerMap.end(); 
			 ++innerIt)
		{
			const std::string& key = innerIt->first;
			const std::vector<std::string>& params = innerIt->second;

			std::cout << "  Key: " << key << "\n  Params: ";
			for (std::vector<std::string>::const_iterator paramIt = params.begin(); 
				 paramIt != params.end(); 
				 ++paramIt)
			{
				std::cout << *paramIt << " ";
			}
			std::cout << std::endl;
		}
	}
}

void	UtilParsing::controlMapLocation(std::map<int, std::map<std::string, std::vector<std::string> > > allMapRoads, std::string keyValue)
{
	int index = 0;
	for (std::map<int, std::map<std::string, std::vector<std::string> > >::const_iterator outerIt = allMapRoads.begin(); 
		 outerIt != allMapRoads.end(); 
		 ++outerIt)
	{
		std::map<std::string, std::vector<std::string> > innerMap = outerIt->second;
		std::map<std::string, std::vector<std::string> >::const_iterator innerIt = innerMap.find(keyValue);
		
		if (innerIt != innerMap.end()) {
			std::cout << "Find !" << std::endl;
		}
		else {
			std::cout << "Probleme aucune key : " << keyValue << " dans index : " << index <<  std::endl;
		}
		index++;
	}
}

void	UtilParsing::manageControlMapLocation(std::map<int, std::map<std::string, std::vector<std::string> > > allMapRoads)
{
	UtilParsing::controlMapLocation(allMapRoads, "location");
	UtilParsing::controlMapLocation(allMapRoads, "root");
	UtilParsing::controlMapLocation(allMapRoads, "index");
	UtilParsing::controlMapLocation(allMapRoads, "methods_accept");
}

void	UtilParsing::displayVector(std::vector<std::string> vec)
{
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
		std::cout   << "[" << *it << "]" << "\n";
}

std::string	UtilParsing::trim(const std::string& str) 
{
	std::size_t first = str.find_first_not_of(' ');
	std::size_t last = str.find_last_not_of(' ');

	if (first == std::string::npos) {
		return "";
	}

	return str.substr(first, last - first + 1);
}

std::string	UtilParsing::trimSemicolon(const std::string& str) 
{
	std::size_t first = str.find_first_not_of(';');
	std::size_t last = str.find_last_not_of(';');

	if (first == std::string::npos) {
		return "";
	}

	return str.substr(first, last - first + 1);
}

std::string	UtilParsing::recoverValue(std::string line, std::string key)
{
	if (line.find(key) != std::string::npos) 
	{
		std::size_t pos = line.find(key.c_str());
		if (pos != std::string::npos) 
		{
			std::size_t startPos = line.find_first_not_of(' ', pos + std::strlen(key.c_str()));
			std::size_t endPos = line.find(';', startPos);
			if (startPos != std::string::npos) 
			{
				std::string value = line.substr(startPos, endPos - startPos);
				return UtilParsing::trim(value);
			}
		}
	}
	return "";
}

std::string UtilParsing::intToString(int value)
{
	std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string UtilParsing::recoverExtension(const std::string &filename)
{
	std::cout << filename << std::endl;
	std::size_t start = filename.find_last_of(".");
	return filename.substr(start);
}

/*	* throws invalid_argument if acess is refused
*/
void UtilParsing::checkAccessRessource(const std::string &ressourcePath, int type)
{
	if (access(ressourcePath.c_str(), type))
		throw std::invalid_argument("checkAccessRessource() : access(\"" + ressourcePath + "\")" + std::string(strerror(errno)));
}

/*	* throws a runtime exception if closedir fail 
*/
void UtilParsing::safeCloseDirectory(DIR *current)
{
	if (closedir(current) == -1) {
		perror("closedir()");
		throw std::runtime_error(RED "Error system in safeCloseDirectory()" RESET);
	}
}

/*	* returns an open directory or throws invalid_argument if dir is not found
*/
DIR* UtilParsing::openDirectory(const std::string &dirPath)
{
	DIR *dirp = opendir(dirPath.c_str());
	if (!dirp)
		throw std::invalid_argument("[" + dirPath + "] doesn't found\n");
	return dirp;
}

/*	* return the content of the file or throw std::runtime_error
*/
std::string	UtilParsing::readFile(const std::string &filepath)
{
	int fd = open(filepath.c_str(), O_RDONLY);
	if (fd == -1) {
		perror("open()");
		throw std::runtime_error("In ErrorHandler (generateContent())");
	}

	std::string	result("");
	char		buff[BUFFER_SIZE] = {'\0'};
	ssize_t		bytesReaded = read(fd, buff, BUFFER_SIZE);

	if (bytesReaded == -1) {
		perror("read()");
		throw std::runtime_error("In ErrorHandler (generateContent())");
	}
	result = buff;

	while (bytesReaded == BUFFER_SIZE)
	{
		bytesReaded = read(fd, buff, BUFFER_SIZE);		
		if (bytesReaded == -1) {
			perror("read()");
			throw std::runtime_error("In ErrorHandler (generateContent())");
		}
		result.append(buff, bytesReaded);
	}
	if (close(fd) == -1)
		perror("close() in UtilsParsing::readFile()");
	return result;
}
