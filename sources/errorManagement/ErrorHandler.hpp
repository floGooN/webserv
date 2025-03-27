
#ifndef ERROR_HANDLER_HPP
# define ERROR_HANDLER_HPP

# include "webserv.hpp"

/*  * PARAMS :
    * ERR_XXX
    * error log (std::string)
*/
class ErrorHandler
{
    public:
        ErrorHandler(const std::string &errNum, const std::string &errLog = "")
        :   errorNumber(errNum), errorLog(errLog)
        {
            std::cout   << "IN ERR CONSTRUCTOR:\n"
                        << "errNum: " << errNum << std::endl
                        << "errLog: " << errLog << std::endl
                        << "errorNumber: " << errorNumber << std::endl
                        << "errorLog" << errorLog << std::endl;
        }

        ErrorHandler(const ErrorHandler& ref)
        :   errorNumber(ref.errorNumber), errorLog(ref.errorLog)
        {   }

        const std::string errorNumber;
        const std::string errorLog;
};

#endif
