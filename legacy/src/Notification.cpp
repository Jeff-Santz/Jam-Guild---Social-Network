#include "Notification.h"

Notification::Notification(std::string msg) : message(msg), read(false) {
    this->timestamp = std::time(nullptr);
}

std::string Notification::getFormattedDate() const {
    char buffer[80];
    struct tm* timeinfo = std::localtime(&timestamp);
    std::strftime(buffer, 80, "%d/%m %H:%M", timeinfo);
    return std::string(buffer);
}