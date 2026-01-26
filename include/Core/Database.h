#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <functional>
#include <sqlite3.h> 
#include <mutex>

namespace Core {
    class Database {
    private:
        sqlite3* db;
        static Database* instance;
        std::mutex dbMutex;
        Database(); // (Singleton)

    public:
        ~Database();
        static Database* getInstance();
        bool execute(const std::string& sql);
        using QueryCallback = std::function<int(int, char**, char**)>;
        bool query(const std::string& sql, QueryCallback callback);
        int getLastInsertId();
        static std::string escape(const std::string& input);
    };
}

#endif