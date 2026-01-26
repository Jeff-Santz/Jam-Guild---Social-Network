#include "Core/Database.h"
#include "Core/Translation.h"
#include <iostream>

namespace Core {

    Database* Database::instance = nullptr;

    Database::Database() {
        auto* tr = Translation::getInstance();
        
        // Adicionei flags de segurança na abertura
        // SQLITE_OPEN_FULLMUTEX garante segurança extra em multi-thread
        int rc = sqlite3_open_v2("social_graph.db", &db, 
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr);
        
        if (rc) {
            std::cerr << tr->get("DB_ERROR") << ": " << sqlite3_errmsg(db) << std::endl;
        } else {
            std::cout << tr->get("DB_SUCCESS") << std::endl;
            sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
            // Melhora performance e segurança contra travamentos
            sqlite3_exec(db, "PRAGMA journal_mode = WAL;", nullptr, nullptr, nullptr); 
        }
    }

    Database::~Database() {
        sqlite3_close(db);
    }

    Database* Database::getInstance() {
        if (instance == nullptr) {
            instance = new Database();
        }
        return instance;
    }

    bool Database::execute(const std::string& sql) {
        std::lock_guard<std::mutex> lock(dbMutex);
        char* zErrMsg = 0;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            auto* tr = Translation::getInstance();
            std::cerr << tr->get("SQL_ERROR") << ": " << zErrMsg << "\nQuery: " << sql << std::endl;
            sqlite3_free(zErrMsg);
            return false;
        }
        return true;
    }

    bool Database::query(const std::string& sql, QueryCallback callback) {
        std::lock_guard<std::mutex> lock(dbMutex);
        char* zErrMsg = 0;
        
        auto c_callback = [](void* data, int argc, char** argv, char** azColName) -> int {
            auto& cb = *static_cast<QueryCallback*>(data);
            return cb(argc, argv, azColName);
        };

        int rc = sqlite3_exec(db, sql.c_str(), c_callback, &callback, &zErrMsg);

        if (rc != SQLITE_OK) {
            auto* tr = Translation::getInstance();
            std::cerr << tr->get("SQL_ERROR") << ": " << zErrMsg << "\nQuery: " << sql << std::endl;
            sqlite3_free(zErrMsg);
            return false;
        }
        return true;
    }

    int Database::getLastInsertId() {
        std::lock_guard<std::mutex> lock(dbMutex);
        return (int)sqlite3_last_insert_rowid(db);
    }

    std::string Database::escape(const std::string& input) {
        std::string output;
        output.reserve(input.size()); // Otimização
        for (char c : input) {
            if (c == '\'') {
                output += "''"; 
            } else {
                output += c;
            }
        }
        return output;
    }
}