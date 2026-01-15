#ifndef COMMUNITY_H
#define COMMUNITY_H

#include "Core/Entity.h"
#include "Auth/User.h"
#include <string>
#include <vector>

namespace Social {

    enum class CommunityRole {
        MASTER_ADMIN = 1,
        ADMIN = 2,
        MEMBER = 3
    };

    class Community : public Core::Entity {
    private:
        std::string name;
        std::string description;
        int ownerId;
        bool isPrivate;

    public:
        Community();
        bool update();
        bool save() override;
        bool isMember(int communityId, int userId);
        bool requestJoin(int userId);
        static bool checkPermission(int communityId, int userId, CommunityRole requiredRole);
        bool updateRole(int targetUserId, CommunityRole newRole, int requesterId);
        bool transferOwnership(int currentMasterId, int newMasterId);
        bool canLeave(int userId);
        static std::vector<Auth::User> searchMembers(int communityId, const std::string& queryStr);
        bool removeMember(int targetUserId, int requesterId);
        bool destroy(int adminId);

        // Setters
        void setName(const std::string& n) { name = n; }        
        void setDescription(const std::string& d) { description = d; }
        void setOwnerId(int id) { ownerId = id; }
        void setPrivate(bool status) { isPrivate = status; }
        static bool addMember(int communityId, int userId, CommunityRole role);

        // Getters
        std::string getName() const { return name; }
        std::string getDescription() const { return description; }
        static int getUserRole(int communityId, int userId);
        std::string getTableName() const override { return "communities"; }
        bool getPrivate() const { return isPrivate; }
        int getOwnerId() const { return ownerId; }
        static std::vector<int> getMembers(int communityId);
        static std::vector<Auth::User> getRichMembers(int communityId);
        static std::vector<Auth::User> getPendingRequests(int communityId);

    };
}

#endif