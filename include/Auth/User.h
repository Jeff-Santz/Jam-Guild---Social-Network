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

    public:
        User(); 
        
        // Getters
        std::string getUsername() const { return username; }
        std::string getEmail() const { return email; }
        std::string getBio() const { return bio; }
        std::string getLanguage() const { return language; }
        std::string getBirthDate() const { return birthDate; }

        // Setters
        void setUsername(const std::string& u) { username = u; }
        void setEmail(const std::string& e) { email = e; }
        void setBio(const std::string& b) { bio = b; }
        void setLanguage(const std::string& lang) { language = lang; }
        void setPassword(const std::string& plainPassword);
        void setPasswordHash(const std::string& h) { passwordHash = h; }
        void setBirthDate(const std::string& date) { birthDate = date; }

        bool save() override;
        std::string getTableName() const override { return "users"; }
        
        // Search Methods
        // Email search
        static bool findByEmail(const std::string& email, User& outUser);

        // ID Search
        static bool findById(int id, User& outUser);

        // password verification
        bool checkPassword(const std::string& inputPass);
    
        // Friend management
        bool addFriend(User* newFriend);
        std::vector<User> getFriends();
    };

}

#endif