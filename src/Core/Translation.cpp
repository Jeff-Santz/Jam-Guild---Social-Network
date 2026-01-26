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

                // --- NOVAS CHAVES DE MÍDIA E ARQUIVOS ---
                messages["ERR_FILE_TOO_LARGE"]   = "Arquivo muito grande. Limite maximo: 50MB.";
                messages["MSG_MEDIA_UPLOADED"]   = "Midia enviada com sucesso.";
                messages["ERR_INVALID_LOCATION"] = "Localizacao invalida. Consulte /api/locations para ver as cidades suportadas.";
                messages["ERR_POST_TOO_LONG"]    = "O post excede o limite de 280 caracteres.";

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
                messages["ERR_COMMENT_TOO_LONG"] = "O comentário é muito longo. Limite: 500 caracteres.";
                messages["ERR_COMMENT_EMPTY"]    = "O comentário não pode estar vazio.";

                // --- NOTIFICAÇÕES & LOGS ---
                messages["NOTIF_LIKE"]        = "curtiu seu post.";
                messages["NOTIF_COMMENT"]     = "comentou no seu post.";
                messages["NOTIF_REPLY"]       = "respondeu ao seu comentário.";
                messages["NOTIF_FRIEND_REQ"]  = "enviou uma solicitacao de amizade.";
                messages["NOTIF_FRIEND_POST"] = "postou algo novo.";
                messages["LOG_BIO_UPDATED"]   = "atualizou a bio.";
                messages["LOG_UNK_ACT"]       = "realizou uma acao.";
                messages["MSG_COMMENT_DELETED"] = "Comentário excluído com sucesso.";
                messages["ERR_PERM_DELETE"]     = "Você não tem permissão para excluir este item.";
                messages["ERR_COMMENT_NOT_FOUND"] = "Comentário não encontrado.";

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
                messages["UI_LBL_USERNAME"]     = "Nome de Usuário";
                messages["UI_LBL_EMAIL"]        = "E-mail";
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

                // --- NOVAS chaves para o Vue.js (Frontend 2.0) ---
                messages["UI_LOADING"]          = "Carregando dados...";
                messages["UI_NO_POSTS"]         = "Nenhum post por enquanto. Seja o primeiro!";
                messages["UI_NO_COMMUNITIES"]   = "Nenhuma comunidade encontrada.";
                messages["UI_NO_NOTIFS"]        = "Você não tem novas notificações.";
                messages["UI_WRITE_PLACEHOLDER"]= "O que está acontecendo na sua rede?";
                
                // Botões Genéricos
                messages["UI_BTN_SEND"]         = "Publicar";
                messages["UI_BTN_CANCEL"]       = "Cancelar";
                messages["UI_BTN_DELETE"]       = "Excluir";
                messages["UI_BTN_OPEN"]         = "Abrir";
                messages["UI_BTN_BACK"]         = "Voltar";
                
                // Status
                messages["STATUS_ONLINE"]       = "Sistema Online";
                messages["STATUS_OFFLINE"]      = "Desconectado";
                messages["STATUS_CONNECTING"]   = "Conectando ao servidor...";
                
                // Abas de Navegação
                messages["TAB_FEED"]            = "Timeline";
                messages["TAB_COMMUNITIES"]     = "Comunidades";
                messages["TAB_NOTIFICATIONS"]   = "Alertas";
                messages["TAB_PROFILE"]         = "Meu Perfil";

                // --- NOVAS CHAVES DE COMENTÁRIOS (Interação) ---
                messages["UI_BTN_REPLY"]         = "Responder";
                messages["UI_LBL_REPLYING_TO"]   = "Respondendo a";
                messages["UI_BTN_CANCEL_REPLY"]  = "Cancelar resposta";
                messages["UI_PLACEHOLDER_REPLY"] = "Escreva sua resposta...";
                messages["UI_PLACEHOLDER_COMMENT"] = "Escreva um comentário...";
                messages["UI_BTN_COMMENT_ACTION"] = "Comentar"; 
                messages["UI_BTN_DEL_SHORT"]     = "Excluir";   

                // --- NOVAS CHAVES DE DENÚNCIA (UI) ---
                messages["UI_BTN_REPORT"]        = "Denunciar";
                messages["UI_TITLE_REPORT"]      = "Denunciar Conteúdo";
                messages["UI_LBL_REASON"]        = "Motivo da denúncia";
                messages["UI_OPT_SPAM"]          = "Spam / Indesejado";
                messages["UI_OPT_HATE"]          = "Discurso de Ódio";
                messages["UI_OPT_FAKE"]          = "Notícia Falsa";
                messages["UI_LANGUAGE_CHANGE"]   = "Idioma alterado";

                // --- NOVAS CHAVES (PT-BR) ---
                messages["UI_LBL_USERNAME"]        = "Nome de Usuário";
                messages["UI_LBL_EMAIL"]           = "E-mail";
                messages["UI_SELECT"]              = "Selecione...";
                
                // --- NAVEGAÇÃO & UI GERAL ---
                messages["TAB_FRIENDS"]            = "Amigos";
                messages["UI_BTN_BACK"]            = "Voltar";
                messages["UI_LOADING"]             = "Carregando...";
                messages["UI_BTN_HIDE"]            = "Ocultar";
                messages["UI_BTN_VIEW"]            = "Ver";
                messages["UI_BTN_CANCEL"]          = "Cancelar";
                messages["UI_BTN_SAVE"]            = "Salvar";
                messages["UI_BTN_OK"]              = "OK";
                messages["UI_BTN_SEARCH"]          = "Buscar";
                messages["UI_BTN_REMOVE"]          = "Remover";
                messages["UI_BTN_EDIT"]            = "Editar";
                
                // --- FEED & POSTS ---
                messages["UI_WRITE_PLACEHOLDER"]   = "O que está acontecendo?";
                messages["UI_BTN_SEND"]            = "Publicar";
                messages["UI_NO_POSTS"]            = "Nenhum post aqui ainda.";
                messages["UI_LBL_LIKES"]           = "Curtidas";
                messages["UI_BTN_COMMENTS"]        = "Comentários";
                messages["UI_NO_COMMENTS"]         = "Nenhum comentário.";
                messages["UI_TITLE_POSTS"]         = "Publicações";

                // --- PERFIL ---
                messages["UI_TITLE_EDIT_PROFILE"]  = "Editar Perfil";
                messages["UI_LBL_BIO"]             = "Biografia";
                messages["UI_LBL_PRIVATE"]         = "Perfil Privado";
                messages["UI_LBL_JOINED"]          = "Entrou em";
                messages["UI_NO_BIO"]              = "Sem biografia.";
                messages["UI_PRIVATE_PROFILE"]     = "Este perfil é privado.";
                messages["MSG_PROFILE_UPDATED"]    = "Perfil atualizado!";

                // --- AMIGOS ---
                messages["UI_TAB_MY_FRIENDS"]      = "Meus Amigos";
                messages["UI_TAB_PENDING"]         = "Pendentes";
                messages["UI_TAB_SEARCH"]          = "Buscar";
                messages["UI_PLACEHOLDER_SEARCH"]  = "Buscar usuário...";
                messages["UI_BTN_ADD_FRIEND"]      = "Adicionar";
                messages["UI_BTN_REMOVE_FRIEND"]   = "Desfazer Amizade";
                messages["UI_BTN_ACCEPT"]          = "Aceitar";
                messages["UI_BTN_REJECT"]          = "Rejeitar";
                messages["UI_LBL_REQUEST_SENT"]    = "Solicitação Enviada";
                messages["UI_NO_FRIENDS"]          = "Você ainda não tem amigos.";
                messages["UI_NO_REQUESTS"]         = "Nenhuma solicitação pendente.";
                messages["UI_NO_RESULTS"]          = "Nenhum resultado encontrado.";
                messages["MSG_REQ_SENT"]           = "Solicitação enviada!";
                messages["MSG_REQ_ACCEPTED"]       = "Solicitação aceita!";
                messages["MSG_REQ_REJECTED"]       = "Solicitação rejeitada.";
                messages["MSG_FRIEND_ADDED"]       = "Agora vocês são amigos!";
                messages["MSG_FRIEND_REMOVED"]     = "Amigo removido.";

                // --- COMUNIDADES ---
                messages["UI_BTN_CREATE_COMM"]     = "Criar Comunidade";
                messages["UI_TITLE_COMM"]          = "Comunidades";
                messages["UI_TITLE_EDIT_COMM"]     = "Editar Comunidade";
                messages["UI_LBL_NAME"]            = "Nome";
                messages["UI_LBL_DESC"]            = "Descrição";
                messages["UI_LBL_MEMBERS"]         = "Membros";
                messages["UI_LBL_REQUESTS"]        = "Solicitações";
                messages["UI_BTN_JOIN"]            = "Entrar";
                messages["UI_BTN_LEAVE"]           = "Sair";
                messages["UI_NO_COMMUNITIES"]      = "Nenhuma comunidade encontrada.";
                messages["UI_NO_DESC"]             = "Sem descrição.";
                messages["UI_NO_MEMBERS"]          = "Nenhum membro.";
                messages["MSG_COMM_CREATED"]       = "Comunidade criada!";
                messages["MSG_COMM_JOINED"]        = "Você entrou na comunidade!";
                messages["MSG_MEMBER_REMOVED"]     = "Membro removido.";
                messages["MSG_ROLE_UPDATED"]       = "Cargo atualizado.";
                messages["UI_CONFIRM_LEAVE"]       = "Sair desta comunidade?";
                messages["UI_CONFIRM_REMOVE"]      = "Remover este membro?";

                // --- ADMIN & DENÚNCIAS ---
                messages["UI_TITLE_ADMIN"]         = "Painel Admin";
                messages["UI_NO_REPORTS"]          = "Nenhuma denúncia pendente.";
                messages["UI_LBL_TYPE"]            = "Tipo";
                messages["UI_LBL_TARGET"]          = "Alvo ID";
                messages["UI_LBL_CATEGORY"]        = "Categoria";
                messages["UI_LBL_REPORTER"]        = "Denunciante";
                messages["UI_BTN_RESOLVE"]         = "Resolver";
                messages["UI_BTN_IGNORE"]          = "Ignorar";
                messages["UI_PLACEHOLDER_REASON"]  = "Descreva o motivo...";
                messages["MSG_REPORT_CREATED"]     = "Denúncia enviada.";
                messages["MSG_REPORT_RESOLVED"]    = "Denúncia resolvida.";

                // --- NOTIFICAÇÕES & OUTROS ---
                messages["UI_BTN_MARK_READ"]       = "Marcar todas como lidas";
                messages["UI_NO_NOTIFS"]           = "Nenhuma notificação nova.";
                messages["MSG_NEW_NOTIF"]          = "Você tem novas notificações!";
                messages["MSG_CREATED"]            = "Conta criada com sucesso!";
                messages["MSG_SAVED"]              = "Salvo com sucesso.";
                messages["UI_LBL_YOUR_ID"]         = "Seu ID de Usuário é";
                messages["UI_MSG_SAVE_ID"]         = "Salve este ID! Você precisará dele para entrar.";
                messages["ERR_AUTH_FAILED"]        = "Falha na autenticação.";
                messages["ERR_SIGNUP"]             = "Erro ao criar conta.";
                messages["UI_CONFIRM_DELETE"]      = "Tem certeza que deseja apagar?";
                messages["UI_CONFIRM_REMOVE_FRIEND"] = "Desfazer amizade com este usuário?";
                messages["UI_BTN_REGISTER_LINK"]    = "Clique Aqui";
                messages["UI_BTN_LOGIN_LINK"]       = "Clique Aqui";

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
                messages["ERR_POST_TOO_LONG"]    = "The Post exceds the characters limit from 200";

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
                messages["ERR_COMMENT_TOO_LONG"] = "Comment is too long. Limit: 200 characters";
                messages["ERR_COMMENT_EMPTY"]    = "The comment cannot be empty.";
                
                // --- NOTIFICATIONS & LOGS ---
                messages["NOTIF_LIKE"]        = "liked your post.";
                messages["NOTIF_COMMENT"]     = "commented on your post.";
                messages["NOTIF_FRIEND_REQ"]  = "sent you a friend request.";
                messages["NOTIF_FRIEND_POST"] = "posted something new.";
                messages["LOG_BIO_UPDATED"]   = "updated the bio.";
                messages["LOG_UNK_ACT"]       = "performed an action.";
                messages["NOTIF_REPLY"]       = "replied to your comment.";
                messages["MSG_COMMENT_DELETED"] = "Comment has been sucessfull deleted.";
                messages["ERR_PERM_DELETE"]     = "You have no permission to delete this item.";
                messages["ERR_COMMENT_NOT_FOUND"] = "Comment not found.";

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
                messages["UI_LBL_USERNAME"]     = "Username";
                messages["UI_LBL_EMAIL"]        = "Email";
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

                // --- NEW keys for Vue.js (Frontend 2.0) ---
                messages["UI_LOADING"]           = "Loading data...";
                messages["UI_NO_POSTS"]          = "No posts yet. Be the first!";
                messages["UI_NO_COMMUNITIES"]    = "No communities found.";
                messages["UI_NO_NOTIFS"]         = "You have no new notifications.";
                messages["UI_WRITE_PLACEHOLDER"] = "What's happening on your network?";

                // Generic Buttons
                messages["UI_BTN_SEND"]          = "Post";
                messages["UI_BTN_CANCEL"]        = "Cancel";
                messages["UI_BTN_DELETE"]        = "Delete";
                messages["UI_BTN_OPEN"]          = "Open";
                messages["UI_BTN_BACK"]          = "Back";

                // Status
                messages["STATUS_ONLINE"]        = "System Online";
                messages["STATUS_OFFLINE"]       = "Offline";
                messages["STATUS_CONNECTING"]    = "Connecting to server...";

                // Navigation Tabs
                messages["TAB_FEED"]             = "Timeline";
                messages["TAB_COMMUNITIES"]      = "Communities";
                messages["TAB_NOTIFICATIONS"]    = "Notifications";
                messages["TAB_PROFILE"]          = "My Profile";

                // --- NEW KEYS FOR COMMENTS (Interaction) ---
                messages["UI_BTN_REPLY"]         = "Reply";
                messages["UI_LBL_REPLYING_TO"]   = "Replying to";
                messages["UI_BTN_CANCEL_REPLY"]  = "Cancel reply";
                messages["UI_PLACEHOLDER_REPLY"] = "Write your reply...";
                messages["UI_PLACEHOLDER_COMMENT"] = "Write a comment...";
                messages["UI_BTN_COMMENT_ACTION"] = "Comment";
                messages["UI_BTN_DEL_SHORT"]     = "Delete";

                // --- NEW KEYS FOR REPORTS (UI) ---
                messages["UI_BTN_REPORT"]        = "Report";
                messages["UI_TITLE_REPORT"]      = "Report Content";
                messages["UI_LBL_REASON"]        = "Reason for reporting";
                messages["UI_OPT_SPAM"]          = "Spam / Unwanted";
                messages["UI_OPT_HATE"]          = "Hate Speech";
                messages["UI_OPT_FAKE"]          = "Fake News";
                messages["UI_LANGUAGE_CHANGE"]   = "Language changed";

                // --- NEW KEYS (EN-US) ---
                messages["UI_LBL_USERNAME"]        = "Username";
                messages["UI_LBL_EMAIL"]           = "Email";
                messages["UI_SELECT"]              = "Select...";
                
                // --- NAVIGATION & UI ---
                messages["TAB_FRIENDS"]            = "Friends";
                messages["UI_BTN_BACK"]            = "Back";
                messages["UI_LOADING"]             = "Loading...";
                messages["UI_BTN_HIDE"]            = "Hide";
                messages["UI_BTN_VIEW"]            = "View";
                messages["UI_BTN_CANCEL"]          = "Cancel";
                messages["UI_BTN_SAVE"]            = "Save";
                messages["UI_BTN_OK"]              = "OK";
                messages["UI_BTN_SEARCH"]          = "Search";
                messages["UI_BTN_REMOVE"]          = "Remove";
                messages["UI_BTN_EDIT"]            = "Edit";
                
                // --- FEED & POSTS ---
                messages["UI_WRITE_PLACEHOLDER"]   = "What is happening?";
                messages["UI_BTN_SEND"]            = "Post";
                messages["UI_NO_POSTS"]            = "No posts yet.";
                messages["UI_LBL_LIKES"]           = "Likes";
                messages["UI_BTN_COMMENTS"]        = "Comments";
                messages["UI_NO_COMMENTS"]         = "No comments yet.";
                messages["UI_TITLE_POSTS"]         = "Posts";

                // --- PROFILE ---
                messages["UI_TITLE_EDIT_PROFILE"]  = "Edit Profile";
                messages["UI_LBL_BIO"]             = "Bio";
                messages["UI_LBL_PRIVATE"]         = "Private Profile";
                messages["UI_LBL_JOINED"]          = "Joined";
                messages["UI_NO_BIO"]              = "No bio.";
                messages["UI_PRIVATE_PROFILE"]     = "This profile is private.";
                messages["MSG_PROFILE_UPDATED"]    = "Profile updated!";

                // --- FRIENDS ---
                messages["UI_TAB_MY_FRIENDS"]      = "My Friends";
                messages["UI_TAB_PENDING"]         = "Pending";
                messages["UI_TAB_SEARCH"]          = "Search";
                messages["UI_PLACEHOLDER_SEARCH"]  = "Search user...";
                messages["UI_BTN_ADD_FRIEND"]      = "Add Friend";
                messages["UI_BTN_REMOVE_FRIEND"]   = "Unfriend";
                messages["UI_BTN_ACCEPT"]          = "Accept";
                messages["UI_BTN_REJECT"]          = "Reject";
                messages["UI_LBL_REQUEST_SENT"]    = "Request Sent";
                messages["UI_NO_FRIENDS"]          = "No friends yet.";
                messages["UI_NO_REQUESTS"]         = "No pending requests.";
                messages["UI_NO_RESULTS"]          = "No results found.";
                messages["MSG_REQ_SENT"]           = "Request sent!";
                messages["MSG_REQ_ACCEPTED"]       = "Request accepted!";
                messages["MSG_REQ_REJECTED"]       = "Request rejected.";
                messages["MSG_FRIEND_ADDED"]       = "Friend added!";
                messages["MSG_FRIEND_REMOVED"]     = "Friend removed.";

                // --- COMMUNITIES ---
                messages["UI_BTN_CREATE_COMM"]     = "Create Community";
                messages["UI_TITLE_COMM"]          = "Communities";
                messages["UI_TITLE_EDIT_COMM"]     = "Edit Community";
                messages["UI_LBL_NAME"]            = "Name";
                messages["UI_LBL_DESC"]            = "Description";
                messages["UI_LBL_MEMBERS"]         = "Members";
                messages["UI_LBL_REQUESTS"]        = "Requests";
                messages["UI_BTN_JOIN"]            = "Join";
                messages["UI_BTN_LEAVE"]           = "Leave";
                messages["UI_NO_COMMUNITIES"]      = "No communities found.";
                messages["UI_NO_DESC"]             = "No description.";
                messages["UI_NO_MEMBERS"]          = "No members.";
                messages["MSG_COMM_CREATED"]       = "Community created!";
                messages["MSG_COMM_JOINED"]        = "Joined community!";
                messages["MSG_MEMBER_REMOVED"]     = "Member removed.";
                messages["MSG_ROLE_UPDATED"]       = "Role updated.";
                messages["UI_CONFIRM_LEAVE"]       = "Leave this community?";
                messages["UI_CONFIRM_REMOVE"]      = "Remove this member?";

                // --- ADMIN & REPORTS ---
                messages["UI_TITLE_ADMIN"]         = "Admin Panel";
                messages["UI_NO_REPORTS"]          = "No pending reports.";
                messages["UI_LBL_TYPE"]            = "Type";
                messages["UI_LBL_TARGET"]          = "Target ID";
                messages["UI_LBL_CATEGORY"]        = "Category";
                messages["UI_LBL_REPORTER"]        = "Reporter";
                messages["UI_BTN_RESOLVE"]         = "Resolve";
                messages["UI_BTN_IGNORE"]          = "Ignore";
                messages["UI_PLACEHOLDER_REASON"]  = "Describe reason...";
                messages["MSG_REPORT_CREATED"]     = "Report sent.";
                messages["MSG_REPORT_RESOLVED"]    = "Report resolved.";

                // --- NOTIFICATIONS & MISC ---
                messages["UI_BTN_MARK_READ"]       = "Mark all as read";
                messages["UI_NO_NOTIFS"]           = "No new notifications.";
                messages["MSG_NEW_NOTIF"]          = "You have new notifications!";
                messages["MSG_CREATED"]            = "Account created successfully!";
                messages["MSG_SAVED"]              = "Saved successfully.";
                messages["UI_LBL_YOUR_ID"]         = "Your User ID is";
                messages["UI_MSG_SAVE_ID"]         = "Save this ID! You will need it to login.";
                messages["ERR_AUTH_FAILED"]        = "Authentication failed.";
                messages["ERR_SIGNUP"]             = "Signup failed.";
                messages["UI_CONFIRM_DELETE"]      = "Are you sure you want to delete?";
                messages["UI_CONFIRM_REMOVE_FRIEND"] = "Remove this friend?";
                messages["UI_BTN_REGISTER_LINK"]    = "Click Here";
                messages["UI_BTN_LOGIN_LINK"]       = "Click Here";

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