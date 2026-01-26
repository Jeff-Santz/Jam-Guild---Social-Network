#include "Core/Logger.h"
#include "Core/Database.h"
#include "Core/Utils.h"

namespace Core {
    void Logger::log(int userId, std::string action, std::string details) {
        auto* db = Database::getInstance();
        std::string date = Utils::getCurrentDateTime();
        
        std::string safeDetails = Core::Database::escape(details);

        std::string sql = "INSERT INTO system_logs ... VALUES (..., '" 
                          + action + "', '" 
                          + safeDetails + "', '" 
                          + date + "');";
        db->execute(sql);
    }
    
    std::vector<crow::json::wvalue> Logger::getUserLogs(int userId) {
        std::vector<crow::json::wvalue> logs;
        auto* db = Database::getInstance();
        
        std::string sql = "SELECT action, details, date FROM system_logs WHERE user_id = " + std::to_string(userId) + " ORDER BY date DESC LIMIT 50;";
        
        auto callback = [&](int, char** argv, char**) -> int {
            crow::json::wvalue item;
            item["action"] = argv[0];
            item["details"] = argv[1];
            item["date"] = argv[2];
            logs.push_back(item);
            return 0;
        };
        db->query(sql, callback);
        return logs;
    }
    
    std::string Logger::getAllLogs() {
        auto* db = Database::getInstance();
        std::string allLogs;
        
        std::string sql = "SELECT user_id, action, details, date FROM system_logs ORDER BY date DESC;";
        
        auto callback = [&](int, char** argv, char**) -> int {
            allLogs += "User ID: " + std::string(argv[0]) + " | Action: " + std::string(argv[1]) + " | Details: " + std::string(argv[2]) + " | Date: " + std::string(argv[3]) + "\n";
            return 0;
        };
        db->query(sql, callback);
        return allLogs;
    }
}