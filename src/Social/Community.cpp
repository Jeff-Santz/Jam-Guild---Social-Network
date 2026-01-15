#include "Content/Notification.h"
#include "Social/Community.h"
#include "Core/Database.h"
#include "Core/Utils.h"

namespace Social {

    Community::Community() : Core::Entity() {
        this->name = "";
        this->description = "";
    }

    bool Community::save() {
        auto* db = Core::Database::getInstance();
        if (this->id == -1) {
            std::string sql = "INSERT INTO communities (name, description, owner_id, is_private, creation_date) VALUES ('" +
                this->name + "', '" + 
                this->description + "', " + 
                std::to_string(this->ownerId) + ", " + 
                std::to_string(this->isPrivate ? 1 : 0) + ", '" + 
                this->creationDate + "');";

            if (db->execute(sql)) {
                this->id = db->getLastInsertId();
                return addMember(this->id, this->ownerId, CommunityRole::MASTER_ADMIN);
            }
        }
        return false;
    }

    bool Community::update() {
        if (this->id == -1) return false;
        auto* db = Core::Database::getInstance();
        
        std::string sql = "UPDATE communities SET "
                        "name = '" + this->name + "', "
                        "description = '" + this->description + "', "
                        "is_private = " + std::to_string(this->isPrivate ? 1 : 0) +
                        " WHERE id = " + std::to_string(this->id) + ";";

        return db->execute(sql);
    }

    bool Community::checkPermission(int communityId, int userId, CommunityRole requiredRole) {
        auto* db = Core::Database::getInstance();
        std::string sql = "SELECT role FROM community_members WHERE community_id = " + 
                          std::to_string(communityId) + " AND user_id = " + std::to_string(userId) + " LIMIT 1;";
        
        int userRole = 0;
        auto callback = [&](int argc, char** argv, char** colNames) -> int {
            userRole = std::stoi(argv[0]);
            return 0;
        };

        db->query(sql, callback);
        return userRole != 0 && userRole <= static_cast<int>(requiredRole);
    }

    bool Community::updateRole(int targetUserId, CommunityRole newRole, int requesterId) {
        auto* db = Core::Database::getInstance();

        // Regra de Negócio: Apenas o MASTER_ADMIN pode mudar cargos
        if (!checkPermission(this->id, requesterId, CommunityRole::MASTER_ADMIN)) {
            return false;
        }

        std::string sql = "UPDATE community_members SET role = " + 
                        std::to_string(static_cast<int>(newRole)) +
                        " WHERE community_id = " + std::to_string(this->id) + 
                        " AND user_id = " + std::to_string(targetUserId) + ";";

        return db->execute(sql);
    }

    bool Community::addMember(int communityId, int userId, CommunityRole role) {
        auto* db = Core::Database::getInstance();
        std::string date = Core::Utils::getCurrentDateTime();
        
        std::string sql = "INSERT OR IGNORE INTO community_members (community_id, user_id, role, join_date) VALUES (" +
            std::to_string(communityId) + ", " + 
            std::to_string(userId) + ", " + 
            std::to_string(static_cast<int>(role)) + ", '" + date + "');";
            
        return db->execute(sql);
    }

    int Community::getUserRole(int communityId, int userId) {
        auto* db = Core::Database::getInstance();
        std::string sql = "SELECT role FROM community_members WHERE community_id = " + 
                          std::to_string(communityId) + " AND user_id = " + std::to_string(userId) + " LIMIT 1;";
        
        int userRole = -1;
        auto callback = [&](int argc, char** argv, char** colNames) -> int {
            userRole = std::stoi(argv[0]);
            return 0;
        };

        db->query(sql, callback);
        return userRole;
    }

    bool Community::transferOwnership(int currentMasterId, int newMasterId) {
        auto* db = Core::Database::getInstance();
        
        if (getUserRole(this->id, currentMasterId) != 1) return false;
        db->execute("BEGIN TRANSACTION;");

        std::string sql1 = "UPDATE community_members SET role = 1 WHERE community_id = " + 
                        std::to_string(this->id) + " AND user_id = " + std::to_string(newMasterId) + ";";
        std::string sql2 = "UPDATE community_members SET role = 2 WHERE community_id = " + 
                        std::to_string(this->id) + " AND user_id = " + std::to_string(currentMasterId) + ";";
        std::string sql3 = "UPDATE communities SET owner_id = " + std::to_string(newMasterId) + 
                        " WHERE id = " + std::to_string(this->id) + ";";

        if (db->execute(sql1) && db->execute(sql2) && db->execute(sql3)) {
            db->execute("COMMIT;");
            return true;
        }
        
        db->execute("ROLLBACK;");
        return false;
    }

    bool Community::canLeave(int userId) {
        return getUserRole(this->id, userId) != 1;
    }

    bool Community::destroy(int adminId) {
        auto* db = Core::Database::getInstance();
        
        if (getUserRole(this->id, adminId) != 1) return false;

        db->execute("BEGIN TRANSACTION;");
        db->execute("DELETE FROM likes WHERE post_id IN (SELECT id FROM posts WHERE community_id = " + std::to_string(this->id) + ");");
        db->execute("DELETE FROM posts WHERE community_id = " + std::to_string(this->id) + ";");
        db->execute("DELETE FROM community_members WHERE community_id = " + std::to_string(this->id) + ";");
        db->execute("DELETE FROM community_requests WHERE community_id = " + std::to_string(this->id) + ";");
        db->execute("DELETE FROM communities WHERE id = " + std::to_string(this->id) + ";");
        db->execute("COMMIT;");
        return true;
    }

    bool Community::removeMember(int targetUserId, int requesterId) {
        auto* db = Core::Database::getInstance();

        int requesterRole = getUserRole(this->id, requesterId);
        int targetRole = getUserRole(this->id, targetUserId);

        // 1 é Master, 2 é Admin, 3 é Member
        if (requesterRole >= 3 || requesterRole >= targetRole) {
            return false;
        }

        std::string sql = "DELETE FROM community_members WHERE community_id = " + 
                        std::to_string(this->id) + " AND user_id = " + std::to_string(targetUserId) + ";";

        return db->execute(sql);
    }

    std::vector<int> Community::getMembers(int communityId) {
        std::vector<int> members;
        auto* db = Core::Database::getInstance();

        std::string sql = "SELECT user_id FROM community_members WHERE community_id = " + std::to_string(communityId) + ";";

        auto callback = [&](int argc, char** argv, char** colNames) -> int {
            members.push_back(std::stoi(argv[0]));
            return 0;
        };

        db->query(sql, callback);
        return members;
    }

    std::vector<Auth::User> Community::getRichMembers(int communityId) {
        std::vector<Auth::User> richList;
        auto* db = Core::Database::getInstance();

        std::string sql = "SELECT u.id, u.username, u.bio FROM users u "
                        "JOIN community_members m ON u.id = m.user_id "
                        "WHERE m.community_id = " + std::to_string(communityId) + ";";

        auto callback = [&](int argc, char** argv, char**) -> int {
            Auth::User u;
            u.setId(std::stoi(argv[0]));
            u.setUsername(argv[1] ? argv[1] : "");
            u.setBio(argv[2] ? argv[2] : "");
            richList.push_back(u);
            return 0;
        };

        db->query(sql, callback);
        return richList;
    }

    bool Community::isMember(int communityId, int userId) {
        auto* db = Core::Database::getInstance();
        std::string sql = "SELECT 1 FROM community_members WHERE community_id = " + 
                          std::to_string(communityId) + " AND user_id = " + std::to_string(userId) + " LIMIT 1;";
        bool found = false;
        db->query(sql, [&](int, char**, char**) { found = true; return 0; });
        return found;
    }

    bool Community::requestJoin(int userId) {
        auto* db = Core::Database::getInstance();
        
        if (isMember(this->id, userId)) {
            return false; 
        }

        if (!this->isPrivate) {
            return Community::addMember(this->id, userId, CommunityRole::MEMBER);
        }

        std::string sql = "INSERT INTO community_requests (community_id, user_id, request_date) VALUES (" +
                          std::to_string(this->id) + ", " + std::to_string(userId) + ", '" + 
                          Core::Utils::getCurrentDateTime() + "');";
        
        if (db->execute(sql)) {
            std::string getAdmsSql = "SELECT user_id FROM community_members WHERE community_id = " + 
                                     std::to_string(this->id) + " AND role <= 2;"; 
            
            auto notifyCallback = [&](int argc, char** argv, char** colNames) -> int {
                int adminId = std::stoi(argv[0]);
                // Tipo 5 = FRIEND_POST 
                Content::Notification::create(adminId, userId, 5, this->id, "COMM_REQUEST_JOIN");
                return 0;
            };

            db->query(getAdmsSql, notifyCallback);
            return true;
        }
        return false;
    }

    std::vector<Auth::User> Community::getPendingRequests(int communityId) {
        std::vector<Auth::User> pendingList;
        auto* db = Core::Database::getInstance();

        std::string sql = "SELECT u.id, u.username, u.bio FROM users u "
                        "JOIN community_requests r ON u.id = r.user_id "
                        "WHERE r.community_id = " + std::to_string(communityId) + ";";

        auto callback = [&](int argc, char** argv, char**) -> int {
            Auth::User u;
            u.setId(std::stoi(argv[0]));
            u.setUsername(argv[1] ? argv[1] : "");
            u.setBio(argv[2] ? argv[2] : "");
            pendingList.push_back(u);
            return 0;
        };

        db->query(sql, callback);
        return pendingList;
    }

    std::vector<Auth::User> Community::searchMembers(int communityId, const std::string& queryStr) {
        std::vector<Auth::User> results;
        auto* db = Core::Database::getInstance();

        std::string sql = "SELECT u.id, u.username, u.bio FROM users u "
                        "JOIN community_members m ON u.id = m.user_id "
                        "WHERE m.community_id = " + std::to_string(communityId) + 
                        " AND u.username LIKE '%" + queryStr + "%';";

        auto callback = [&](int argc, char** argv, char**) -> int {
            Auth::User u;
            u.setId(std::stoi(argv[0]));
            u.setUsername(argv[1] ? argv[1] : "");
            u.setBio(argv[2] ? argv[2] : "");
            results.push_back(u);
            return 0;
        };

        db->query(sql, callback);
        return results;
    }


}



