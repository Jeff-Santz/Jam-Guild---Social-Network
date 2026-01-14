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
                // --- SISTEMA & BANCO ---
                messages["SYS_START"]      = ">> Iniciando Social Engine v1.0...";
                messages["DB_SUCCESS"]     = ">> Banco de dados conectado com sucesso.";
                messages["DB_ERROR"]       = "[ERRO FATAL] Nao foi possivel abrir o banco de dados";
                messages["TABLE_CREATED"]  = ">> Tabela verificada/criada com sucesso.";
                messages["SQL_ERROR"]      = "[ERRO SQL] Falha ao executar query";
                
                // --- SERVIDOR & LOGS ---
                messages["SERVER_ONLINE"]  = "   SOCIAL ENGINE V1 - SERVER ONLINE";
                messages["SERVER_URL"]     = "   Acesse: http://localhost:8085/health";

                // --- API & RESPOSTAS JSON ---
                messages["API_HEALTH_MSG"]    = "Social Engine Backend Operacional";
                messages["ERR_JSON"]          = "JSON Invalido ou mal formatado";
                messages["ERR_MISSING"]       = "Dados obrigatorios faltando ou invalidos";
                messages["MSG_CREATED"]       = "Usuario Criado com Sucesso";
                messages["ERR_CONFLICT"]      = "Erro: Email ou Username ja existem";
                messages["MSG_LOGIN_OK"]      = "Login Aprovado";
                messages["ERR_WRONG_PASS"]    = "Senha Incorreta";
                messages["ERR_USER_NOT_FOUND"]= "Usuario nao encontrado";
                
                // --- DATAS & VALIDAÇÃO ---
                messages["DATE_FORMAT"]         = "%d/%m/%Y"; 
                messages["ERR_DATE_FORMAT"]     = "Formato invalido. Use AAAA-MM-DD ou DD/MM/AAAA.";
                messages["ERR_DATE_RANGE"]      = "Ano fora do limite permitido (1900 - Hoje).";
                messages["ERR_DATE_INVALID"]    = "Data inexistente no calendario.";
                messages["ERR_PREFIX_DATE"]     = "Erro na Data: ";

                // --- FRIEND REQUESTS ---
                messages["MSG_REQ_SENT"]      = "Solicitacao enviada com sucesso!";
                messages["ERR_REQ_EXIST"]     = "Erro: Ja existe solicitacao ou amizade.";
                messages["MSG_REQ_PROCESSED"] = "Solicitacao processada com sucesso!";
                messages["ERR_REQ_PROCESS"]   = "Erro ao processar solicitacao.";
                messages["MSG_FRIEND_ADDED"]  = "Agora voces sao amigos!";
                messages["ERR_FRIEND_EXIST"]  = "Ja sao amigos ou erro";

                // --- INTERAÇÕES  ---
                messages["MSG_COMMENT_ADDED"] = "Comentario publicado com sucesso!";
                messages["ERR_COMMENT_EMPTY"] = "O comentario nao pode estar vazio.";
                messages["MSG_LIKE_ADDED"]    = "Post curtido!";
                messages["MSG_LIKE_REMOVED"]  = "Curtida removida do post!";

                // --- NOTIFICAÇÕES & LOGS ---
                messages["NOTIF_LIKE"]        = "curtiu seu post.";
                messages["NOTIF_COMMENT"]     = "comentou no seu post.";
                messages["NOTIF_FRIEND_REQ"]  = "enviou uma solicitacao de amizade.";
                messages["NOTIF_FRIEND_POST"] = "postou algo novo.";
                messages["LOG_BIO_UPDATED"]   = "atualizou a bio.";
                messages["LOG_UNK_ACT"]       = "realizou uma acao.";

                // --- NOVAS CHAVES ---
                messages["MSG_PROFILE_UPDATED"] = "Perfil Atualizado!";
                messages["MSG_POST_CREATED"]    = "Post Publicado!";
                
                break;

            case Language::EN_US:
            default:
                // --- SYSTEM & DATABASE ---
                messages["SYS_START"]      = ">> Starting Social Engine v1.0...";
                messages["DB_SUCCESS"]     = ">> Database connected successfully.";
                messages["DB_ERROR"]       = "[FATAL ERROR] Could not open database";
                messages["TABLE_CREATED"]  = ">> Table verified/created successfully.";
                messages["SQL_ERROR"]      = "[SQL ERROR] Failed to execute query";

                // --- SERVER & LOGS ---
                messages["SERVER_ONLINE"]  = "   SOCIAL ENGINE V1 - SERVER ONLINE";
                messages["SERVER_URL"]     = "   Access: http://localhost:8085/health";

                // --- API & JSON RESPONSES ---
                messages["API_HEALTH_MSG"]    = "Social Engine Backend Operational";
                messages["ERR_JSON"]          = "Invalid or malformed JSON";
                messages["ERR_MISSING"]       = "Missing or invalid required data";
                messages["MSG_CREATED"]       = "User Created Successfully";
                messages["ERR_CONFLICT"]      = "Error: Email or Username already exists";
                messages["MSG_LOGIN_OK"]      = "Login Approved";
                messages["ERR_WRONG_PASS"]    = "Incorrect Password";
                messages["ERR_USER_NOT_FOUND"]= "User not found";

                // --- DATES & VALIDATION ---
                messages["DATE_FORMAT"]      = "%m/%d/%Y"; 
                messages["ERR_DATE_FORMAT"]  = "Invalid format. Use YYYY-MM-DD or DD/MM/YYYY.";
                messages["ERR_DATE_RANGE"]   = "Year outside permitted range (1900 - Today).";
                messages["ERR_DATE_INVALID"] = "Non-existent calendar date.";
                messages["ERR_PREFIX_DATE"]  = "Date Error: ";

                // --- FRIEND REQUESTS ---
                messages["MSG_REQ_SENT"]      = "Friend request sent successfully!";
                messages["ERR_REQ_EXIST"]     = "Error: Request already exists or already friends.";
                messages["MSG_REQ_PROCESSED"] = "Request processed successfully!";
                messages["ERR_REQ_PROCESS"]   = "Error processing request.";
                messages["MSG_FRIEND_ADDED"]  = "You are now friends!";
                messages["ERR_FRIEND_EXIST"]  = "Already friends or error";

                // --- INTERACTIONS ---
                messages["MSG_COMMENT_ADDED"] = "Comment posted successfully!";
                messages["ERR_COMMENT_EMPTY"] = "Comment cannot be empty.";
                messages["MSG_LIKE_ADDED"]    = "Post liked!";
                messages["MSG_LIKE_REMOVED"]  = "Like removed from post!";
                
                // --- NOTIFICATIONS & LOGS ---
                messages["NOTIF_LIKE"]        = "liked your post.";
                messages["NOTIF_COMMENT"]     = "commented on your post.";
                messages["NOTIF_FRIEND_REQ"]  = "sent you a friend request.";
                messages["NOTIF_FRIEND_POST"] = "posted something new.";
                messages["LOG_BIO_UPDATED"]   = "updated the bio.";
                messages["LOG_UNK_ACT"]       = "performed an action.";

                // --- NEW KEYS ---
                messages["MSG_PROFILE_UPDATED"] = "Profile Updated!";
                messages["MSG_POST_CREATED"]    = "Post Published!";
                
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