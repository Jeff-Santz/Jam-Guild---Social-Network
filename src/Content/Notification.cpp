#include "Content/Notification.h"
#include "Core/Database.h"
#include "Core/Utils.h"
#include "Core/Translation.h" 

namespace Content {

    bool Notification::create(int receiverId, int senderId, int type, int refId, std::string extraText) {
        
        if (receiverId == senderId) return false; 

        auto* db = Core::Database::getInstance();
        std::string date = Core::Utils::getCurrentDateTime();
        std::string postIdVal = "-1";
        std::string commIdVal = "-1";

        if (type == LIKE || type == COMMENT || type == FRIEND_POST || type == REPLY) {
            postIdVal = std::to_string(refId);
        } 
        else if (type == PAGE_INVITE) {
            commIdVal = std::to_string(refId);
        }

        // --- PROTEÇÃO ---
        std::string safeText = Core::Database::escape(extraText);
        // ----------------

        std::string sql = "INSERT INTO notifications (user_id, sender_id, type, post_id, community_id, content, is_read, created_at) VALUES (" 
                          + std::to_string(receiverId) + ", " 
                          + std::to_string(senderId) + ", " 
                          + std::to_string(type) + ", " 
                          + postIdVal + ", "   
                          + commIdVal + ", '" 
                          + safeText + "', 0, '" 
                          + date + "');";

        return db->execute(sql);
    }

    std::vector<crow::json::wvalue> Notification::getByUser(int userId) {
        auto* db = Core::Database::getInstance();
        std::vector<crow::json::wvalue> list;

        std::string sql = "SELECT n.id, n.type, n.content, n.sender_id, n.is_read, n.created_at, "
                          "n.post_id, n.community_id, "
                          "u.username, u.avatar_url "
                          "FROM notifications n "
                          "LEFT JOIN users u ON n.sender_id = u.id "
                          "WHERE n.user_id = " + std::to_string(userId) + 
                          " ORDER BY n.id DESC LIMIT 50;";

        db->query(sql, [&](int argc, char** argv, char** colNames) {
            crow::json::wvalue item;
            
            item["id"] = std::stoi(argv[0]);
            item["type"] = std::stoi(argv[1]);
            item["content"] = argv[2] ? argv[2] : "";
            item["sender_id"] = std::stoi(argv[3]);
            item["read"] = (std::string(argv[4]) == "1");
            item["created_at"] = argv[5] ? argv[5] : "";
            item["post_id"] = (argv[6] && std::string(argv[6]) != "-1") ? std::stoi(argv[6]) : 0;
            item["community_id"] = (argv[7] && std::string(argv[7]) != "-1") ? std::stoi(argv[7]) : 0;
            item["sender_name"] = argv[8] ? argv[8] : "Unknown";
            item["username"] = argv[8] ? argv[8] : "Unknown";
            item["sender_avatar"] = argv[9] ? argv[9] : "";

            list.push_back(std::move(item));
            return 0;
        });

        return list;
    }

    void Notification::markAllAsRead(int userId) {
        auto* db = Core::Database::getInstance();
        db->execute("UPDATE notifications SET is_read = 1 WHERE user_id = " + std::to_string(userId));
    }
}