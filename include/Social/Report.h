#ifndef REPORT_H
#define REPORT_H

#include "Core/Entity.h"
#include <string>
#include <vector>

namespace Social {
    class Report : public Core::Entity {
    private:
        int reporterId;
        int targetId;
        int targetType; // 1: User, 2: Post, 3: Community, 4: Comment
        int category; 
        std::string reason; // 1=Spam, 2=Hate, 3=Nudity, 4=DMCA, 5=Other
        int status;     // 0: Open, 1: Resolved, 2: Ignored
        std::string creationDate;

    public:
        Report();
        
        bool save() override;
        std::string getTableName() const override { return "reports"; }
        
        // Setters
        void setReporterId(int id) { reporterId = id; }
        void setTargetId(int id) { targetId = id; }
        void setTargetType(int type) { targetType = type; }
        void setCategory(int c) { category = c; }
        void setReason(const std::string& r) { reason = r; }
        void setStatus(int s) { status = s; }
        void setCreationDate(const std::string& d) { creationDate = d; }

        // Getters
        int getReporterId() const { return reporterId; }
        int getTargetId() const { return targetId; }
        int getTargetType() const { return targetType; }
        int getCategory() const { return category; }
        std::string getReason() const { return reason; }
        int getStatus() const { return status; }
        std::string getCreationDate() const { return creationDate; }

        // Métodos Estáticos (Admin)
        static std::vector<Report> getAllPending();
        static bool resolve(int reportId, int status);
    };
}

#endif