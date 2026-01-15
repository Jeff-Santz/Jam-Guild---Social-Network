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
                messages["ERR_CONFLICT"]      = "Erro: Email ja vinculado a outra conta";
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

                // -- COMUNIDADES ---
                messages["MSG_MEMBER_REMOVED"] = "Membro removido da comunidade.";
                messages["MSG_COMM_CREATED"]    = "Comunidade criada com sucesso!";
                messages["ERR_ALREADY_MEMBER"]  = "Erro: Este usuário já faz parte da comunidade.";
                messages["COMM_REQUEST_JOIN"]   = "solicitou entrar na comunidade.";
                messages["APROVE_COMM_MEMBER"]   = "foi aprovado para entrar na comunidade.";
                messages["ERR_PERMISSION_DENIED"]   = "Erro: Permissão negada para esta ação.";
                messages["MSG_ROLE_UPDATED"]    = "Função atualizada com sucesso!";
                messages["NOTIF_COMMUNITY_NEW_POST"] = "postou na comunidade da qual você é membro.";
                messages["MSG_LEAVE_COMM"]    = "Você saiu da comunidade.";
                messages["ERR_LEAVE_COMM"]    = "Erro: Falha ao processar sua saída.";
                messages["MSG_COMM_DELETED"]        = "Comunidade e todos os seus dados foram excluidos.";
                messages["MSG_TRANSFER_OK"]         = "Posse da comunidade transferida com sucesso.";
                messages["ERR_MASTER_LEAVE"]        = "O dono nao pode sair da comunidade sem transferir a posse.";
                messages["ERR_NOT_MASTER"]          = "Apenas o dono (Master Admin) pode realizar esta acao.";
                messages["MSG_REQ_ACCEPTED"]        = "Solicitacao de entrada aceita!";
                messages["MSG_REQ_REJECTED"]        = "Solicitacao de entrada recusada.";
                messages["ERR_PROCESS_REQ"]         = "Erro ao processar acao no pedido de entrada.";

                // --- NOVAS CHAVES ---
                messages["MSG_PROFILE_UPDATED"] = "Perfil Atualizado!";
                messages["MSG_POST_CREATED"]    = "Post Publicado!";
                messages["MSG_USER_DELETED"] = "Sua conta e todos os seus dados foram excluidos permanentemente.";
                messages["ERR_DELETE_MASTER"] = "Voce nao pode excluir sua conta sendo dono de uma comunidade. Transfira a posse ou exclua a comunidade primeiro.";

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
                messages["ERR_CONFLICT"]      = "Error: Email already linked to another account";
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

                // --- COMMUNITIES ---
                messages["MSG_MEMBER_REMOVED"] = "Member removed from the community.";
                messages["MSG_COMM_CREATED"]    = "Community created successfully!";
                messages["ERR_ALREADY_MEMBER"]  = "Error: This user is already a community member.";
                messages["COMM_REQUEST_JOIN"]   = "requested to join the community.";
                messages["APROVE_COMM_MEMBER"]   = "was approved to join the community.";
                messages["ERR_PERMISSION_DENIED"]   = "Error: Permission denied for this action.";
                messages["MSG_ROLE_UPDATED"]    = "Role updated successfully!";
                messages["NOTIF_COMMUNITY_NEW_POST"] = "posted in the community you are a member of.";
                messages["MSG_LEAVE_COMM"]    = "You have left the community.";
                messages["ERR_LEAVE_COMM"]    = "Error: Failed to process your exit.";
                messages["MSG_COMM_DELETED"]        = "Community and all its data have been deleted.";
                messages["MSG_TRANSFER_OK"]         = "Community ownership transferred successfully.";
                messages["ERR_MASTER_LEAVE"]        = "The owner cannot leave the community without transferring ownership.";
                messages["ERR_NOT_MASTER"]          = "Only the owner (Master Admin) can perform this action.";
                messages["MSG_REQ_ACCEPTED"]        = "Join request accepted!";
                messages["MSG_REQ_REJECTED"]        = "Join request rejected.";
                messages["ERR_PROCESS_REQ"]         = "Error processing action on join request.";

                // --- NEW KEYS ---
                messages["MSG_PROFILE_UPDATED"] = "Profile Updated!";
                messages["MSG_POST_CREATED"]    = "Post Published!";
                messages["MSG_USER_DELETED"] = "Your account and all its data have been permanently deleted.";
                messages["ERR_DELETE_MASTER"] = "You cannot delete your account while owning a community. Transfer ownership or delete the community first.";

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