#include "Social/Report.h"
#include "Core/Database.h"
#include "Core/Utils.h"

namespace Social {

    Report::Report() {
        this->status = 0; 
        this->reporterId = -1;
        this->targetId = -1;
        this->targetType = 0;
        this->category = 5; // Padrão "Outros"
    }

    bool Report::save() {
        auto* db = Core::Database::getInstance();
        this->creationDate = Core::Utils::getCurrentDateTime();

        std::string safeReason = Core::Database::escape(this->reason);

        // INCLUÍDO O CAMPO CATEGORY
        std::string sql = "INSERT INTO reports (reporter_id, target_id, target_type, category, reason, status, creation_date) VALUES (" +
            std::to_string(this->reporterId) + ", " + 
            std::to_string(this->targetId) + ", " + 
            std::to_string(this->targetType) + ", " + 
            std::to_string(this->category) + ", '" + 
            safeReason + "', 0, '" + 
            this->creationDate + "');";

        if (db->execute(sql)) {
            this->id = db->getLastInsertId();
            return true;
        }
        return false;
    }

    std::vector<Report> Report::getAllPending() {
        auto* db = Core::Database::getInstance();
        std::vector<Report> list;

        std::string sql = "SELECT id, reporter_id, target_id, target_type, category, reason, creation_date FROM reports WHERE status = 0 ORDER BY id DESC;";
        
        auto callback = [&](int argc, char** argv, char**) -> int {
            Report r;
            r.setId(std::stoi(argv[0]));
            r.setReporterId(std::stoi(argv[1]));
            r.setTargetId(std::stoi(argv[2]));
            r.setTargetType(std::stoi(argv[3]));
            r.setCategory(std::stoi(argv[4])); 
            r.setReason(argv[5] ? argv[5] : "");
            r.setCreationDate(argv[6] ? argv[6] : "");
            
            list.push_back(r);
            return 0;
        };

        db->query(sql, callback);
        return list;
    }

    bool Report::resolve(int reportId, int newStatus) {
        auto* db = Core::Database::getInstance();
        std::string sql = "UPDATE reports SET status = " + std::to_string(newStatus) + 
                          " WHERE id = " + std::to_string(reportId) + ";";
        return db->execute(sql);
    }
}