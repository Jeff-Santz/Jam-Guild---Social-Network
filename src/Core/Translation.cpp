#include "Core/Translation.h"

namespace Core {

    Translation* Translation::instance = nullptr;

    Translation::Translation() {
        currentLang = Language::PT_BR; 
        loadTranslations();
    }

    Translation* Translation::getInstance() {
        if (instance == nullptr) {
            instance = new Translation();
        }
        return instance;
    }

    void Translation::setLanguage(Language lang) {
        currentLang = lang;
        loadTranslations();
    }

    void Translation::loadTranslations() {
        messages.clear();

        switch (currentLang) {
            case Language::PT_BR:
                // --- SISTEMA & BANCO DE DADOS ---
                messages["SYS_START"]    = ">> Iniciando Social Engine v1.0...";
                messages["DB_SUCCESS"]   = ">> Banco de dados conectado com sucesso.";
                messages["DB_ERROR"]     = "[ERRO FATAL] Nao foi possivel abrir o banco de dados"; // Sem acento pra evitar bug de encoding por enquanto
                messages["TABLE_CREATED"]= ">> Tabela verificada/criada com sucesso.";
                messages["SQL_ERROR"]    = "[ERRO SQL] Falha ao executar query";

                // --- USUÁRIO ---
                messages["ERR_USER_NOT_FOUND"] = "Usuario nao encontrado.";
                messages["ERR_WRONG_PASS"]     = "Senha incorreta.";
                messages["MSG_WELCOME"]        = "Bem-vindo a Jam Guild!";
                break;

            case Language::EN_US:
            default:
                // --- SYSTEM & DATABASE ---
                messages["SYS_START"]    = ">> Starting Social Engine v1.0...";
                messages["DB_SUCCESS"]   = ">> Database connected successfully.";
                messages["DB_ERROR"]     = "[FATAL ERROR] Could not open database";
                messages["TABLE_CREATED"]= ">> Table verified/created successfully.";
                messages["SQL_ERROR"]    = "[SQL ERROR] Failed to execute query";

                // --- USER ---
                messages["ERR_USER_NOT_FOUND"] = "User not found.";
                messages["ERR_WRONG_PASS"]     = "Incorrect password.";
                messages["MSG_WELCOME"]        = "Welcome to Jam Guild!";
                break;
        }
    }

    std::string Translation::get(const std::string& key) {
        if (messages.find(key) != messages.end()) {
            return messages[key];
        }
        return "[" + key + "]";
    }
}