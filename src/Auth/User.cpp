#include "Auth/User.h"
#include "Core/Crypto.h"
#include "Core/Database.h"
#include "Core/Utils.h"
#include "Content/Notification.h"
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

    bool User::update() {
        if (this->id == -1) return false; 

        auto* db = Core::Database::getInstance();
        
        std::string sql = "UPDATE users SET bio = '" + this->bio + 
                          "', language = '" + this->language + 
                          "', birth_date = '" + this->birthDate + 
                          "' WHERE id = " + std::to_string(this->id) + ";";

        return db->execute(sql);
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

    //Friendship management methods
    // Não esqueça de incluir no topo: #include "Core/Utils.h"

    // 1. Enviar Solicitação (Cria com Status 0)
    bool User::sendFriendRequest(User* other) {
        if (this->id == -1 || other->getId() == -1) return false;
        
        auto* db = Core::Database::getInstance();
        std::string date = Core::Utils::getCurrentDateTime(); // Data e Hora atual

        // SQL: Insere status 0 (Pendente)
        // user_id_1 = Quem pediu (Eu)
        // user_id_2 = Quem recebeu (Outro)
        std::string sql = "INSERT INTO friendships (user_id_1, user_id_2, status, since_date) VALUES (" 
                          + std::to_string(this->id) + ", " 
                          + std::to_string(other->getId()) + ", 0, '" + date + "');";
        
        bool success = db->execute(sql);
        if (success) {
            // Inclua "Content/Notification.h" no topo do User.cpp se não tiver!
            Content::Notification::create(other->getId(), this->id, Content::Notification::FRIEND_REQ, this->id, "NOTIF_FRIEND_REQ");
        }
        return success;
    }

    // 2. Aceitar Solicitação (Muda Status para 1)
    bool User::acceptFriendRequest(User* other) {
        auto* db = Core::Database::getInstance();
        
        // Atualiza para 1 onde o OUTRO pediu e EU aceito
        std::string sql = "UPDATE friendships SET status = 1 WHERE user_id_1 = " 
                          + std::to_string(other->getId()) + " AND user_id_2 = " 
                          + std::to_string(this->id) + ";";
        
        return db->execute(sql);
    }

    // 3. Recusar ou Desfazer (Remove do banco)
    bool User::removeOrRejectFriend(User* other) {
        auto* db = Core::Database::getInstance();
        
        // Remove qualquer relação entre os dois (independente de quem pediu)
        std::string sql = "DELETE FROM friendships WHERE "
                          "(user_id_1 = " + std::to_string(this->id) + " AND user_id_2 = " + std::to_string(other->getId()) + ") OR "
                          "(user_id_1 = " + std::to_string(other->getId()) + " AND user_id_2 = " + std::to_string(this->id) + ");";
                          
        return db->execute(sql);
    }

    // 4. Ver notificações (Quem pediu pra mim?)
    std::vector<User> User::getPendingRequests() {
        std::vector<User> pendingList;
        auto* db = Core::Database::getInstance();

        // user_id_2 sou EU (recebi), user_id_1 é ELE (pediu), status = 0
        std::string sql = "SELECT u.id, u.username, u.email, u.bio FROM users u "
                          "JOIN friendships f ON u.id = f.user_id_1 "
                          "WHERE f.user_id_2 = " + std::to_string(this->id) + " AND f.status = 0;";

        auto callback = [&](int argc, char** argv, char** colNames) -> int {
            User u;
            u.setId(std::stoi(argv[0]));
            u.setUsername(argv[1] ? argv[1] : "");
            // Bio e Email são opcionais na listagem rapida
            pendingList.push_back(u);
            return 0;
        };

        db->query(sql, callback);
        return pendingList;
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

    // General search by username
    std::vector<User> User::search(const std::string& queryStr) {
        std::vector<User> results;
        auto* db = Core::Database::getInstance();

        // O operador LIKE %texto% busca em qualquer parte do nome
        std::string sql = "SELECT id, username, email, bio FROM users WHERE username LIKE '%" + queryStr + "%';";

        auto callback = [&](int argc, char** argv, char** colNames) -> int {
            User u;
            u.setId(std::stoi(argv[0]));
            u.setUsername(argv[1] ? argv[1] : "");
            u.setEmail(argv[2] ? argv[2] : "");
            u.setBio(argv[3] ? argv[3] : "");
            results.push_back(u);
            return 0;
        };

        db->query(sql, callback);
        return results;
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