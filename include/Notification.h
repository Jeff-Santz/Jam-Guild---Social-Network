#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>
#include <ctime>

class Notification {
private:
    std::string message;
    std::time_t timestamp;
    bool read;

public:
    Notification(std::string msg);
    std::string getMessage() const { return message; }
    std::string getFormattedDate() const;
    bool isRead() const { return read; }
    void markAsRead() { read = true; }
};

#endif