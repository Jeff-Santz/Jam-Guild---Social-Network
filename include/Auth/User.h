#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include "Core/Entity.h"

namespace Auth {

    class User : public Core::Entity {
    private:
        std::string username;
        std::string email;
        std::string passwordHash;
        std::string bio;
        std::string language;
        std::string birthDate;
        bool isPrivate = false;

    public:
        User();
        bool deleteAccount(int userId);
        
        // Getters
        std::string getUsername() const { return username; }
        std::string getEmail() const { return email; }
        std::string getBio() const { return bio; }
        std::string getLanguage() const { return language; }
        std::string getBirthDate() const { return birthDate; }
        bool getPrivate() const { return isPrivate; }

        // Setters
        void setUsername(const std::string& u) { username = u; }
        void setEmail(const std::string& e) { email = e; }
        void setBio(const std::string& b) { bio = b; }
        void setLanguage(const std::string& lang) { language = lang; }
        void setPassword(const std::string& plainPassword);
        void setPasswordHash(const std::string& h) { passwordHash = h; }
        void setBirthDate(const std::string& date) { birthDate = date; }
        void setPrivate(bool status) { isPrivate = status; }

        bool save() override;
        std::string getTableName() const override { return "users"; }
        
        // Search Methods
        static bool findByEmail(const std::string& email, User& outUser);
        static bool findById(int id, User& outUser);
        static std::vector<User> search(const std::string& query);

        // password verification
        bool checkPassword(const std::string& inputPass);
    
        // Friend management
        bool sendFriendRequest(User* other);
        bool acceptFriendRequest(User* other);
        bool removeOrRejectFriend(User* other);
        bool addFriend(User* newFriend);
        std::vector<User> getFriends();
        std::vector<User> getPendingRequests();
        // Status: -1 (Nothing), 0 (Pendant), 1 (Friends)
        int checkFriendshipStatus(User* other); // Tem q implementar ou excluir isso

        // Profile update
        bool update();
    };

}

#endif