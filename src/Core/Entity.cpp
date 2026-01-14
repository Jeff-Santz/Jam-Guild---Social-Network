#include "Core/Entity.h"
#include <iomanip>
#include <sstream>

namespace Core {

    Entity::Entity() {
        this->id = -1; 
        this->active = true;

        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        
        std::stringstream ss;
        ss << std::put_time(now, "%Y-%m-%d %H:%M:%S");
        this->creationDate = ss.str();
    }

}