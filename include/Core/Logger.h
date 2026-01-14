#ifndef LOGGER_H
#define LOGGER_H

#include "crow_all.h"
#include <string>

namespace Core {
    class Logger {
    public:
        static void log(int userId, std::string action, std::string details);
        static std::string getAllLogs(); 
        static std::vector<crow::json::wvalue> getUserLogs(int userId);
    };
}

#endif