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
        std::string city;   
        std::string state;  
        std::string avatarUrl;
        std::string coverUrl;
        int role = 0;
        bool isPrivate = false;
        bool isVerified = false;

    public:
        User();
        bool save() override;
        bool update();
        bool login(const std::string& identifier, const std::string& plainPassword);
        bool deleteAccount(int userId);

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
        bool markEmailAsVerified();
        // Status: -1 (Nothing), 0 (Pendant), 1 (Friends)
        int checkFriendshipStatus(User* other); // Tem q implementar ou excluir isso
        
        // Setters
        void setUsername(const std::string& u) { username = u; }
        void setEmail(const std::string& e) { email = e; }
        void setBio(const std::string& b) { bio = b; }
        void setLanguage(const std::string& lang) { language = lang; }
        void setPassword(const std::string& plainPassword);
        void setPasswordHash(const std::string& h) { passwordHash = h; }
        void setBirthDate(const std::string& date) { birthDate = date; }
        void setPrivate(bool status) { isPrivate = status; }
        void setVerified(bool v) { isVerified = v; }
        void setCity(const std::string& c) { city = c; }
        void setState(const std::string& s) { state = s; }
        void setAvatarUrl(const std::string& url) { avatarUrl = url; }
        void setCoverUrl(const std::string& url) { coverUrl = url; }
        void setRole(const int r) { role = r; }

        // Getters
        std::string getUsername() const { return username; }
        std::string getEmail() const { return email; }
        std::string getBio() const { return bio; }
        std::string getLanguage() const { return language; }
        std::string getBirthDate() const { return birthDate; }
        std::string getCity() const { return city; }
        std::string getState() const { return state; }
        std::string getAvatarUrl() const { return avatarUrl; }
        std::string getCoverUrl() const {return coverUrl; }
        bool getPrivate() const { return isPrivate; }
        bool getVerified() const { return isVerified; }
        int getRole() const { return role; }
        std::string getTableName() const override { return "users"; }
        std::vector<User> getFriends();
        std::vector<User> getPendingRequests();
        
    };

}

#endif