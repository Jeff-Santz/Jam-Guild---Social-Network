#include "Core/Database.h"
#include "Core/Translation.h"
#include <iostream>

namespace Core {

    Database* Database::instance = nullptr;

    Database::Database() {
        // Instância do Tradutor
        auto* tr = Translation::getInstance();

        int rc = sqlite3_open("social_graph.db", &db);
        
        if (rc) {
            std::cerr << tr->get("DB_ERROR") << ": " << sqlite3_errmsg(db) << std::endl;
        } else {
            std::cout << tr->get("DB_SUCCESS") << std::endl;
            sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
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

    // --- A FUNÇÃO QUE ESTAVA FALTANDO E CAUSOU O ERRO ---
    bool Database::query(const std::string& sql, QueryCallback callback) {
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
        return (int)sqlite3_last_insert_rowid(db);
    }
}