#include "Auth/User.h"
#include "Core/Crypto.h"
#include "Core/Database.h"
#include <iostream>

namespace Auth {
    const std::string GLOBAL_SALT = "J3ff_Pr3gn@_P0ly_2026_#";

    User::User() {
        this->language = "pt_BR";
        this->bio = "";
    }

    void User::setPassword(const std::string& plainPassword) {
        this->passwordHash = Core::Crypto::sha256(plainPassword + GLOBAL_SALT);
    }

    bool User::checkPassword(const std::string& inputPass) {
        std::string inputHash = Core::Crypto::sha256(inputPass + GLOBAL_SALT);
        return this->passwordHash == inputHash;
    }

    bool User::save() {
        auto* db = Core::Database::getInstance();
        
        if (this->id == -1) {
            std::string sql = "INSERT INTO users (username, email, password_hash, bio, language, birth_date, creation_date) VALUES ('" +
                this->username + "', '" + 
                this->email + "', '" + 
                this->passwordHash + "', '" + 
                this->bio + "', '" + 
                this->language + "', '" +
                this->birthDate + "', '" +   
                this->creationDate + "');"; 

            if (db->execute(sql)) {
                this->id = db->getLastInsertId();
                return true;
            }
        }
        return false;
    }

    bool User::findByEmail(const std::string& email, User& outUser) {
        auto* db = Core::Database::getInstance();
        
        std::string sql = "SELECT id, username, email, password_hash, bio, language, creation_date, birth_date FROM users WHERE email = '" + email + "';";
        
        bool found = false;

        auto callback = [&](int argc, char** argv, char** colNames) -> int {
            found = true;
            outUser.setId(std::stoi(argv[0]));
            outUser.setUsername(argv[1] ? argv[1] : "");
            outUser.setEmail(argv[2] ? argv[2] : "");
            outUser.setPasswordHash(argv[3] ? argv[3] : ""); 
            outUser.setBio(argv[4] ? argv[4] : "");
            outUser.setLanguage(argv[5] ? argv[5] : "pt_BR");
            outUser.setCreationDate(argv[6] ? argv[6] : "");
            outUser.setBirthDate(argv[7] ? argv[7] : "");
            return 0;
        };

        db->query(sql, callback);
        return found;
    }

    bool User::findById(int id, User& outUser) {
        auto* db = Core::Database::getInstance();
        
        // Searching by ID
        std::string sql = "SELECT id, username, email, password_hash, bio, language, creation_date FROM users WHERE id = " + std::to_string(id) + ";";
        
        bool found = false;

        auto callback = [&](int argc, char** argv, char** colNames) -> int {
            found = true;
            outUser.setId(std::stoi(argv[0]));
            outUser.setUsername(argv[1] ? argv[1] : "");
            outUser.setEmail(argv[2] ? argv[2] : "");
            outUser.setPasswordHash(argv[3] ? argv[3] : "");
            outUser.setBio(argv[4] ? argv[4] : "");
            outUser.setLanguage(argv[5] ? argv[5] : "pt_BR");
            outUser.setCreationDate(argv[6] ? argv[6] : "");
            return 0;
        };

        db->query(sql, callback);
        return found;
    }

    // Add a friend relationship
    bool User::addFriend(User* newFriend) {
        if (this->id == -1 || newFriend->getId() == -1) return false;

        auto* db = Core::Database::getInstance();
        
        // Data atual para saber desde quando são amigos
        std::time_t t = std::time(nullptr);
        char dateStr[20];
        std::strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

        // Query: Jeff (ID 1) -> Lapis (ID 2)
        std::string sql = "INSERT INTO friendships (user_id_1, user_id_2, since_date) VALUES (" 
                          + std::to_string(this->id) + ", " 
                          + std::to_string(newFriend->getId()) + ", '" 
                          + std::string(dateStr) + "');";
        
        return db->execute(sql);
    }

    // Search and return friends
    std::vector<User> User::getFriends() {
        std::vector<User> friendsList;
        
        if (this->id == -1) return friendsList;

        auto* db = Core::Database::getInstance();

        std::string sql = "SELECT u.id, u.username, u.email, u.password_hash, u.bio, u.language, u.creation_date "
                          "FROM users u "
                          "INNER JOIN friendships f ON u.id = f.user_id_2 "
                          "WHERE f.user_id_1 = " + std::to_string(this->id) + ";";

        auto callback = [&](int argc, char** argv, char** colNames) -> int {
            User friendObj;
            friendObj.setId(std::stoi(argv[0]));
            friendObj.setUsername(argv[1] ? argv[1] : "");
            friendObj.setEmail(argv[2] ? argv[2] : "");
            friendObj.setPasswordHash(argv[3] ? argv[3] : "");
            friendObj.setBio(argv[4] ? argv[4] : "");
            friendObj.setLanguage(argv[5] ? argv[5] : "pt_BR");
            friendObj.setCreationDate(argv[6] ? argv[6] : "");
            
            friendsList.push_back(friendObj);
            return 0;
        };

        db->query(sql, callback);
        return friendsList;
    }
}