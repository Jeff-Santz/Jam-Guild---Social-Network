#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <ctime>

namespace Core {

    class Entity {
    protected:
        int id;
        std::string creationDate;
        bool active;

    public:
        Entity(); 
        virtual ~Entity() = default; 

        // Getters and Setters
        int getId() const { return id; }
        void setId(int newId) { id = newId; }

        std::string getCreationDate() const { return creationDate; }
        void setCreationDate(const std::string& date) { creationDate = date; }

        bool isActive() const { return active; }
        void setActive(bool status) { active = status; }

        virtual bool save() = 0; 
        virtual std::string getTableName() const = 0;
    };
}

#endif