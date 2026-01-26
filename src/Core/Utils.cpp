#include "Core/Utils.h"
#include "Core/Translation.h" 
#include <iostream>
#include <regex>
#include <stdexcept>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cstdlib>

namespace Core {

    static const std::string base64_chars = 
                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                 "abcdefghijklmnopqrstuvwxyz"
                 "0123456789+/";

    bool is_base64(unsigned char c) {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

    std::string Utils::validateISO(std::string date) {
        auto* tr = Translation::getInstance();

        if (date.length() == 8 && date.find('/') == std::string::npos && date.find('-') == std::string::npos) {
            date = date.substr(0, 2) + "/" + date.substr(2, 2) + "/" + date.substr(4, 4);
        }

        int day, month, year;
        bool isISO = false;

        std::regex isoPattern("^([0-9]{4})-([0-9]{2})-([0-9]{2})$"); 
        std::regex brPattern("^([0-9]{2})/([0-9]{2})/([0-9]{4})$");
        
        std::smatch matches;

        if (std::regex_match(date, matches, isoPattern)) {
            year  = std::stoi(matches[1]);
            month = std::stoi(matches[2]);
            day   = std::stoi(matches[3]);
            isISO = true;
        } 
        else if (std::regex_match(date, matches, brPattern)) {
            day   = std::stoi(matches[1]);
            month = std::stoi(matches[2]);
            year  = std::stoi(matches[3]);
            isISO = false; 
        } 
        else {
            throw std::invalid_argument(tr->get("ERR_DATE_FORMAT"));
        }

        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);
        int currentYear = now->tm_year + 1900;

        if (year < 1900 || year > currentYear) {
            throw std::out_of_range(tr->get("ERR_DATE_RANGE"));
        }

        int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        
        if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
            daysInMonth[2] = 29;
        }

        if (month < 1 || month > 12 || day < 1 || day > daysInMonth[month]) {
            throw std::logic_error(tr->get("ERR_DATE_INVALID"));
        }

        std::stringstream ss;
        ss << year << "-" 
           << std::setw(2) << std::setfill('0') << month << "-" 
           << std::setw(2) << std::setfill('0') << day;
           
        return ss.str(); 
    }

    std::string Utils::formatForDisplay(std::string isoDate) {
        if (isoDate.length() < 10) return isoDate; 

        try {
            int year = std::stoi(isoDate.substr(0, 4));
            int month = std::stoi(isoDate.substr(5, 2));
            int day = std::stoi(isoDate.substr(8, 2));

            std::string format = Translation::getInstance()->get("DATE_FORMAT");
            std::tm tm = {};
            tm.tm_year = year - 1900;
            tm.tm_mon = month - 1;
            tm.tm_mday = day;

            char buffer[80];
            std::strftime(buffer, 80, format.c_str(), &tm);
            
            return std::string(buffer);
        } catch (...) {
            return isoDate;
        }
    }

    std::string Utils::getCurrentDateTime() {
        std::time_t t = std::time(nullptr);
        std::tm* timeinfo = std::localtime(&t);
        
        char buffer[80];
        std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
        return std::string(buffer);
    }

    void Utils::loadEnv(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << ">> Warning: .env file not found!" << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            // Ignorar comentários e linhas vazias
            if (line.empty() || line[0] == '#') continue;

            std::istringstream lineStream(line);
            std::string key, value;
            if (std::getline(lineStream, key, '=') && std::getline(lineStream, value)) {
                // Define a variável de ambiente no processo atual
                #ifdef _WIN32
                    _putenv_s(key.c_str(), value.c_str());
                #else
                    setenv(key.c_str(), value.c_str(), 1);
                #endif
            }
        }
    }

    std::string Utils::saveBase64Image(const std::string &base64Data, int postId)
    {
        std::string data = base64Data;
        std::string extension = ".jpg"; 
        size_t comma = data.find(",");
        if (comma != std::string::npos)
        {
            std::string header = data.substr(0, comma);
            if (header.find("video") != std::string::npos)
                extension = ".mp4";
            else if (header.find("png") != std::string::npos)
                extension = ".png";

            data = data.substr(comma + 1);
        }

        if (data.size() > 70000000)
        {
            return ""; // Retorna vazio = Erro (Muito grande)
        }

        int in_len = data.size();
        int i = 0, j = 0, in_ = 0;
        unsigned char char_array_4[4], char_array_3[3];
        std::vector<unsigned char> ret;

        while (in_len-- && (data[in_] != '=') && is_base64(data[in_]))
        {
            char_array_4[i++] = data[in_];
            in_++;
            if (i == 4)
            {
                for (i = 0; i < 4; i++)
                    char_array_4[i] = base64_chars.find(char_array_4[i]);
                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
                for (i = 0; (i < 3); i++)
                    ret.push_back(char_array_3[i]);
                i = 0;
            }
        }
        if (i)
        {
            for (j = i; j < 4; j++)
                char_array_4[j] = 0;
            for (j = 0; j < 4; j++)
                char_array_4[j] = base64_chars.find(char_array_4[j]);
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            for (j = 0; (j < i - 1); j++)
                ret.push_back(char_array_3[j]);
        }

        std::string filename = "uploads/post_" + std::to_string(postId) + extension;
        std::ofstream file(filename, std::ios::binary);
        file.write(reinterpret_cast<const char *>(&ret[0]), ret.size());
        file.close();

        return filename;
    }
    std::string Utils::sanitize(const std::string& input) {
        std::string output;
        output.reserve(input.size()); 
        for (char c : input) {
            if (c == '\'') {
                output += "''"; 
            } else {
                output += c;
            }
        }
        return output;
    }

}