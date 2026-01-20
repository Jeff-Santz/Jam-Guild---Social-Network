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

        // Mantemos o std::vector para bater com o Notification.h
        std::vector<crow::json::wvalue> Notification::getByUser(int userId) {
            auto* db = Core::Database::getInstance();
            std::vector<crow::json::wvalue> list;

            // O SQL continua o mesmo (com o JOIN para pegar o nome)
            std::string sql = "SELECT n.id, n.type, n.content, n.sender_id, n.is_read, n.created_at, "
                            "n.post_id, n.community_id, "
                            "u.username "  // <--- Coluna 8: O nome que faltava
                            "FROM notifications n "
                            "LEFT JOIN users u ON n.sender_id = u.id " // <--- JOIN mágico
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
                
                if (argv[6]) item["post_id"] = std::stoi(argv[6]); else item["post_id"] = 0;
                if (argv[7]) item["community_id"] = std::stoi(argv[7]); else item["community_id"] = 0;

                // --- AQUI ESTÁ A CORREÇÃO ---
                // Pegamos o username da tabela users e colocamos no JSON
                item["sender_name"] = argv[8] ? argv[8] : "Unknown";
                item["username"] = argv[8] ? argv[8] : "Unknown";

                list.push_back(std::move(item));
                return 0;
            });

            // Retorna o vetor puro (o Router transforma em JSON)
            return list;
        }

    void Notification::markAllAsRead(int userId) {
        auto* db = Core::Database::getInstance();
        db->execute("UPDATE notifications SET is_read = 1 WHERE user_id = " + std::to_string(userId));
    }
}