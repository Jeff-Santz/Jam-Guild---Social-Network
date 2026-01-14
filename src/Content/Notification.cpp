#include "Content/Notification.h"
#include "Core/Database.h"
#include "Core/Utils.h"
#include "Core/Translation.h" 

namespace Content {

    bool Notification::create(int receiverId, int senderId, int type, int refId, std::string extraText) {
        if (receiverId == senderId) return false; 

        auto* db = Core::Database::getInstance();
        std::string date = Core::Utils::getCurrentDateTime();

        std::string sql = "INSERT INTO notifications (user_id, sender_id, type, reference_id, content, is_read, creation_date) VALUES (" 
                          + std::to_string(receiverId) + ", " 
                          + std::to_string(senderId) + ", " 
                          + std::to_string(type) + ", " 
                          + std::to_string(refId) + ", '" 
                          + extraText + "', 0, '" 
                          + date + "');";

        return db->execute(sql);
    }

    std::vector<crow::json::wvalue> Notification::getByUser(int userId) {
        std::vector<crow::json::wvalue> list;
        auto* db = Core::Database::getInstance();
        auto* tr = Core::Translation::getInstance(); // Para traduzir na hora de ler

        std::string sql = "SELECT n.id, n.type, n.reference_id, n.content, n.creation_date, n.is_read, u.username "
                          "FROM notifications n "
                          "JOIN users u ON n.sender_id = u.id "
                          "WHERE n.user_id = " + std::to_string(userId) + 
                          " ORDER BY n.creation_date DESC LIMIT 50;";

        auto callback = [&](int argc, char** argv, char** colNames) -> int {
            crow::json::wvalue item;
            item["id"] = std::stoi(argv[0]);
            int type = std::stoi(argv[1]);
            item["type"] = type;
            item["ref_id"] = std::stoi(argv[2]);
            
            std::string key = argv[3] ? argv[3] : "";
            item["text"] = tr->get(key); 
            
            item["date"] = argv[4] ? argv[4] : "";
            item["is_read"] = std::stoi(argv[5]) == 1; 
            item["sender"] = argv[6] ? argv[6] : "Unknown";
            
            list.push_back(item);
            return 0;
        };

        db->query(sql, callback);
        return list;
    }

    void Notification::markAllAsRead(int userId) {
        auto* db = Core::Database::getInstance();
        db->execute("UPDATE notifications SET is_read = 1 WHERE user_id = " + std::to_string(userId));
    }
}