#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "crow_all.h"
#include <string>
#include <vector>

namespace Content {

    class Notification {
    public:
        // Tipos de Notificação
        enum Type {
            FRIEND_REQ = 1,
            LIKE = 2,
            COMMENT = 3,
            PAGE_INVITE = 4,
            FRIEND_POST = 5
        };
        static bool create(int receiverId, int senderId, int type, int refId, std::string extraText);

        static std::vector<crow::json::wvalue> getByUser(int userId);
        static void markAllAsRead(int userId);
    };
}

#endif