#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Core {

    class Utils {
    public:
        static std::string validateISO(std::string date);
        static std::string formatForDisplay(std::string isoDate);

        static std::string getCurrentDateTime();
        static void loadEnv(const std::string& filename);
        static std::string saveBase64Image(const std::string& base64Data, int postId);
    };

}

#endif