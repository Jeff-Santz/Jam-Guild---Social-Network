#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <string>
#include <map>
#include <string>

namespace Core {

    enum class Language {
        EN_US,
        PT_BR
    };

    class Translation {
    private:
        static Translation* instance;
        std::map<std::string, std::string> messages;
        Language currentLang;
        Translation(); 
        void loadTranslations();

    public:
        static Translation* getInstance();
        void setLanguage(Language lang);
        std::string get(const std::string& key);
    };

}

#endif