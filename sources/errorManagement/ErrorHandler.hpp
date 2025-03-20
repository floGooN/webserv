
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
        {   }

        ErrorHandler(const ErrorHandler& ref)
        :   errorNumber(ref.errorNumber), errorLog(ref.errorLog)
        {   }

        const std::string errorNumber;
        const std::string errorLog;
};

#endif
