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
                messages["ERR_AUTH_FAILED"]   = "Senha ou email incorretos";
                messages["ERR_RMV_VERIFIED_EMAIL"]   = "Usuários verificados não podem remover seu email";
                messages["ERR_INVALID_EMAIL"]   = "Erro: Email inválido!";
                messages["MSG_TOKEN_ISSUED"] = "Autenticação gerada com sucesso";

                // --- NOVAS CHAVES DE MÍDIA E ARQUIVOS (FALTAVA ISSO) ---
                messages["ERR_FILE_TOO_LARGE"]   = "Arquivo muito grande. Limite maximo: 50MB.";
                messages["MSG_MEDIA_UPLOADED"]   = "Midia enviada com sucesso.";
                messages["ERR_INVALID_LOCATION"] = "Localizacao invalida. Consulte /api/locations para ver as cidades suportadas.";

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
                messages["MSG_POST_CREATED"]  = "Post Publicado!";

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
                messages["MSG_COMM_JOINED"]         = "Você entrou na comunidade!";
                messages["ERR_NOT_VERIFIED"]        = "Voce precisa verificar seu email para criar comunidades.";
                messages["MSG_EMAIL_VERIFIED"]      = "Seu email foi verificado com sucesso";
                messages["ERR_PRIVATE_COMMUNITY"] = "Essa pagina tem seu perfil privado";

                // --- REPORTE & GOVERNANÇA ---
                messages["MSG_REPORT_CREATED"]   = "Denuncia enviada com sucesso. Nossa equipe vai analisar.";
                messages["ERR_REPORT_MISSING"]   = "Faltam dados para a denuncia (target_id, type ou reason).";
                messages["MSG_REPORT_RESOLVED"]  = "Denuncia marcada como resolvida.";
                messages["ERR_ADMIN_ONLY"]       = "Apenas administradores do sistema podem ver isso.";

                // --- PERFIL ---
                messages["MSG_PROFILE_UPDATED"] = "Perfil Atualizado!";
                messages["MSG_USER_DELETED"] = "Sua conta e todos os seus dados foram excluidos permanentemente.";
                messages["ERR_DELETE_MASTER"] = "Voce nao pode excluir sua conta sendo dono de uma comunidade. Transfira a posse ou exclua a comunidade primeiro.";
                messages["ERR_PRIVATE_PROFILE"]   = "Esse usuario tem o perfil privado";

                // --- PRIVACIDADE & PERFIL ---
                messages["LBL_PRIVATE_BIO"]     = "🔒 Este perfil é privado.";
                messages["LBL_RESTRICTED"]      = "Restrito";
                messages["LBL_UNKNOWN_LOC"]     = "Localização Desconhecida";
                messages["LBL_GLOBAL_LOC"]      = "Global";
                messages["BTN_ADD_FRIEND"]      = "Adicionar";
                messages["BTN_PENDING"]         = "Pendente";
                messages["BTN_FRIENDS"]         = "Amigos";

                // --- INTERFACE GRÁFICA (UI) ---
                // Auth
                messages["UI_TITLE_LOGIN"]      = "Acesso ao Terminal";
                messages["UI_BTN_LOGIN"]        = "Acessar Sistema";
                messages["UI_BTN_SIGNUP"]       = "Criar Identidade";
                messages["UI_LBL_ID"]           = "Identificador (ID/Email)";
                messages["UI_LBL_PASS"]         = "Senha de Acesso";
                messages["UI_LBL_CITY"]         = "Cidade";
                messages["UI_LBL_STATE"]        = "Estado";
                
                // Navegação
                messages["UI_NAV_FEED"]         = "Feed Principal";
                messages["UI_NAV_COMMUNITIES"]  = "Comunidades";
                messages["UI_NAV_FRIENDS"]      = "Rede de Contatos";
                messages["UI_NAV_NOTIFICATIONS"]= "Notificações";
                messages["UI_NAV_SETTINGS"]     = "Configurações";
                messages["UI_NAV_LOGOUT"]       = "Desconectar";

                // Feed
                messages["UI_BTN_POST"]         = "Transmitir";
                messages["UI_PLACEHOLDER_POST"] = "Atualize o status da rede...";
                messages["UI_BTN_MEDIA"]        = "Anexar Mídia";
                messages["UI_BTN_COMMENTS"]     = "Comentários";
                messages["UI_LBL_LIKES"]        = "Curtidas";

                // Perfil
                messages["UI_LBL_BIO"]          = "Biografia";
                messages["UI_LBL_JOINED"]       = "Membro desde";
                messages["UI_BTN_ADD"]          = "Conectar";
                messages["UI_BTN_ACCEPT"]       = "Aceitar";
                messages["UI_BTN_REJECT"]       = "Recusar";
                messages["UI_LBL_PRIVATE"]      = "Perfil Privado";
                messages["UI_LBL_LOCKED"]       = "Acesso Restrito";
                
                // Comunidades
                messages["UI_TITLE_COMM"]       = "Diretório de Comunidades";
                messages["UI_BTN_CREATE_COMM"]  = "Fundar Comunidade";
                messages["UI_BTN_JOIN"]         = "Juntar-se";
                messages["UI_BTN_LEAVE"]        = "Abandonar";

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
                messages["ERR_AUTH_FAILED"]   = "Incorrect email or password";
                messages["ERR_RMV_VERIFIED_EMAIL"]   = "Verified users cannot remove their email.";
                messages["ERR_INVALID_EMAIL"]   = "Error: Invalid email";
                messages["MSG_TOKEN_ISSUED"] = "Auth token issued successfully";

                // --- MEDIA & FILES (NEW KEYS) ---
                messages["ERR_FILE_TOO_LARGE"]   = "File too large. Max limit: 50MB.";
                messages["MSG_MEDIA_UPLOADED"]   = "Media uploaded successfully.";
                messages["ERR_INVALID_LOCATION"] = "Invalid location. Check /api/locations for supported cities.";

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
                messages["MSG_POST_CREATED"]  = "Post Published!";
                
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
                messages["MSG_COMM_JOINED"]         = "You have joined the community!";
                messages["ERR_NOT_VERIFIED"]        = "You need to verify your email to create communities";
                messages["MSG_EMAIL_VERIFIED"]      = "Your email have been sucessfully verified";
                messages["ERR_PRIVATE_COMMUNITY"] = "This page has its profile set to private";
                
                // --- REPORT & GOVERNANCE ---
                messages["MSG_REPORT_CREATED"]   = "Report submitted successfully. Our team will review it.";
                messages["ERR_REPORT_MISSING"]   = "Missing report data (target_id, type or reason).";
                messages["MSG_REPORT_RESOLVED"]  = "Report marked as resolved.";
                messages["ERR_ADMIN_ONLY"]       = "Only system administrators can access this.";

                // --- PROFILE ---
                messages["MSG_PROFILE_UPDATED"] = "Profile Updated!";
                messages["MSG_USER_DELETED"] = "Your account and all its data have been permanently deleted.";
                messages["ERR_DELETE_MASTER"] = "You cannot delete your account while owning a community. Transfer ownership or delete the community first.";
                messages["ERR_PRIVATE_PROFILE"]   = "This user has a private profile";

                // --- PRIVACY & PROFILE ---
                messages["LBL_PRIVATE_BIO"]     = "🔒 This profile is private.";
                messages["LBL_RESTRICTED"]      = "Restricted";
                messages["LBL_UNKNOWN_LOC"]     = "Unknown Location";
                messages["LBL_GLOBAL_LOC"]      = "Global";
                messages["BTN_ADD_FRIEND"]      = "Add Friend";
                messages["BTN_PENDING"]         = "Pending";
                messages["BTN_FRIENDS"]         = "Friends";

                // --- GRAPHICAL INTERFACE (UI) ---
                // Auth
                messages["UI_TITLE_LOGIN"]      = "Terminal Access";
                messages["UI_BTN_LOGIN"]        = "Access System";
                messages["UI_BTN_SIGNUP"]       = "Create Identity";
                messages["UI_LBL_ID"]           = "Identifier (ID/Email)";
                messages["UI_LBL_PASS"]         = "Access Password";
                messages["UI_LBL_CITY"]         = "City";
                messages["UI_LBL_STATE"]        = "State";

                // Navigation
                messages["UI_NAV_FEED"]         = "Main Feed";
                messages["UI_NAV_COMMUNITIES"]  = "Communities";
                messages["UI_NAV_FRIENDS"]      = "Contact Network";
                messages["UI_NAV_NOTIFICATIONS"]= "Notifications";
                messages["UI_NAV_SETTINGS"]     = "Settings";
                messages["UI_NAV_LOGOUT"]       = "Disconnect";

                // Feed
                messages["UI_BTN_POST"]         = "Broadcast";
                messages["UI_PLACEHOLDER_POST"] = "Update your network status...";
                messages["UI_BTN_MEDIA"]        = "Attach Media";
                messages["UI_BTN_COMMENTS"]     = "Comments";
                messages["UI_LBL_LIKES"]        = "Likes";

                // Profile
                messages["UI_LBL_BIO"]          = "Biography";
                messages["UI_LBL_JOINED"]       = "Member since";
                messages["UI_BTN_ADD"]          = "Connect";
                messages["UI_BTN_ACCEPT"]       = "Accept";
                messages["UI_BTN_REJECT"]       = "Reject";
                messages["UI_LBL_PRIVATE"]      = "Private Profile";
                messages["UI_LBL_LOCKED"]       = "Restricted Access";

                // Communities
                messages["UI_TITLE_COMM"]       = "Community Directory";
                messages["UI_BTN_CREATE_COMM"]  = "Found Community";
                messages["UI_BTN_JOIN"]         = "Join";
                messages["UI_BTN_LEAVE"]        = "Leave";


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