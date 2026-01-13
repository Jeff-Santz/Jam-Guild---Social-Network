#include "Utils.h"
#include <iostream>
#include <limits>
#include <sstream>
#include <sstream>
#include <iomanip>
#include <regex>
#include <ctime>
#include <stdexcept>


std::string Utils::hashPassword(const std::string& password) {
    unsigned long hash = 5381;
    
    for (char c : password) {
        hash = ((hash << 5) + hash) + c;
    }

    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return ss.str();
}

std::string Utils::validateAndFixDate(std::string date) {
    // Lógica para tratar 01012005 -> 01/01/2005
    if (date.length() == 8 && date.find('/') == std::string::npos) {
        date = date.substr(0, 2) + "/" + date.substr(2, 2) + "/" + date.substr(4, 4);
    }

    // Regex para validar formato DD/MM/YYYY
    std::regex datePattern("^([0-9]{2})/([0-9]{2})/([0-9]{4})$");
    std::smatch matches;

    if (!std::regex_match(date, matches, datePattern)) {
        throw std::invalid_argument("Invalid Format! Use DD/MM/AAAA or DDMMAAAA.");
    }

    int day = std::stoi(matches[1]);
    int month = std::stoi(matches[2]);
    int year = std::stoi(matches[3]);

    // Validação de limites de ano (Programação Defensiva)
    time_t t = time(0);
    tm* now = localtime(&t);
    int currentYear = now->tm_year + 1900;

    if (year < 1900 || year > currentYear) {
        throw std::out_of_range("Year outside the permitted range. (1900 - today).");
    }

    // Validação de dias por mês (considerando bissextos)
    int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) daysInMonth[2] = 29;

    if (month < 1 || month > 12 || day < 1 || day > daysInMonth[month]) {
        throw std::logic_error("Calender date is invalid.");
    }

    return date;
}

void Utils::cleanBuffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void Utils::showHeader(const std::string& title) {
    std::cout << "\n=======================================" << std::endl;
    std::cout << "   " << title << std::endl;
    std::cout << "=======================================" << std::endl;
}

std::string Utils::getCurrentDate() {
    std::time_t t = std::time(nullptr);
    char buffer[80];
    struct tm* timeinfo = std::localtime(&t);
    std::strftime(buffer, 80, "%d/%m/%Y", timeinfo);
    return std::string(buffer);
}
