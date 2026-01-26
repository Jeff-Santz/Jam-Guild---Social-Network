#include "Content/Comment.h"
#include "Content/Notification.h"
#include "Core/Database.h"
#include "Core/Utils.h"
#include <iostream>

namespace Content {

    int Comment::save() {
        auto* db = Core::Database::getInstance();
        this->creationDate = Core::Utils::getCurrentDateTime();

        std::string parentVal = (this->parentId == -1) ? "-1" : std::to_string(this->parentId);
        
        // --- PROTEÇÃO ---
        std::string safeContent = Core::Database::escape(this->content);
        std::string safeMedia = Core::Database::escape(this->mediaUrl);
        // ----------------

        std::string sql = "INSERT INTO comments (post_id, author_id, parent_id, content, media_url, media_type, creation_date) VALUES (" 
                        + std::to_string(this->postId) + ", " 
                        + std::to_string(this->authorId) + ", " 
                        + parentVal + ", '" 
                        + safeContent + "', '"  // AQUI
                        + safeMedia + "', '" 
                        + this->mediaType + "', '"
                        + this->creationDate + "');";

        bool success = db->execute(sql);

        if (success) {
            int newId = db->getLastInsertId(); 
            this->id = newId;
            
            if (this->parentId != -1) {
                // É UMA RESPOSTA: Notificar o autor do comentário PAI
                int parentAuthorId = -1;
                auto cb = [&](int, char** argv, char**) { parentAuthorId = std::stoi(argv[0]); return 0; };
                // Busca quem escreveu o comentário que estamos respondendo
                db->query("SELECT author_id FROM comments WHERE id=" + std::to_string(this->parentId), cb);
                
                // Se achou e não for eu mesmo respondendo a mim mesmo
                if (parentAuthorId != -1 && parentAuthorId != this->authorId) {
                    Notification::create(parentAuthorId, this->authorId, Notification::COMMENT, this->postId, "NOTIF_REPLY");
                }
            } else {
                // É UM COMENTÁRIO RAIZ: Notificar o dono do POST
                int postOwnerId = -1;
                auto cb = [&](int, char** argv, char**) { postOwnerId = std::stoi(argv[0]); return 0; };
                db->query("SELECT author_id FROM posts WHERE id=" + std::to_string(this->postId), cb);

                if (postOwnerId != -1 && postOwnerId != this->authorId) {
                    Notification::create(postOwnerId, this->authorId, Notification::COMMENT, this->postId, "NOTIF_COMMENT");
                }
            }
            return newId;
        }
        return -1;
    }

    std::vector<Comment> Comment::getCommentsByPostId(int postId, int currentUserId) {
        std::vector<Comment> list;
        auto* db = Core::Database::getInstance();
        
        std::string sql = 
            "SELECT c.id, c.author_id, c.parent_id, c.content, c.media_url, c.media_type, c.creation_date, "
            "u.username, u.avatar_url, "
            "(SELECT COUNT(*) FROM comment_likes cl WHERE cl.comment_id = c.id) as like_count, "
            "(SELECT COUNT(*) FROM comment_likes cl2 WHERE cl2.comment_id = c.id AND cl2.user_id = " + std::to_string(currentUserId) + ") as liked_by_me "
            "FROM comments c "
            "LEFT JOIN users u ON c.author_id = u.id "
            "WHERE c.post_id = " + std::to_string(postId) + " "
            "ORDER BY like_count DESC, c.creation_date ASC;";

        auto callback = [&](int argc, char** argv, char** colNames) -> int {
            Comment c;
            c.setPostId(postId);
            c.setId(std::stoi(argv[0]));
            c.setAuthorId(std::stoi(argv[1]));
            c.setParentId(argv[2] ? std::stoi(argv[2]) : -1);
            c.setContent(argv[3] ? argv[3] : "");
            c.setMediaUrl(argv[4] ? argv[4] : "");
            c.setMediaType(argv[5] ? argv[5] : "");
            c.setCreationDate(argv[6] ? argv[6] : "");
            c.setAuthorName(argv[7] ? argv[7] : "Unknown");
            c.setAuthorAvatar(argv[8] ? argv[8] : "");
            c.setLikesCount(argv[9] ? std::stoi(argv[9]) : 0);
            c.setLikedByMe(argv[10] && std::string(argv[10]) == "1"); // Se for "1", é true

            list.push_back(std::move(c)); // std::move para otimizar ;)
            return 0;
        };

        db->query(sql, callback);
        return list;
    }

    bool Comment::toggleLike(int userId, int commentId) {
        auto* db = Core::Database::getInstance();
        bool exists = false;
        db->query("SELECT 1 FROM comment_likes WHERE user_id=" + std::to_string(userId) + " AND comment_id=" + std::to_string(commentId), 
            [&](int, char**, char**){ exists = true; return 0; });

        if (exists) {
            // REMOVE LIKE
            db->execute("DELETE FROM comment_likes WHERE user_id=" + std::to_string(userId) + " AND comment_id=" + std::to_string(commentId));
            return false; 
        } else {
            // ADICIONA LIKE
            db->execute("INSERT INTO comment_likes (user_id, comment_id, date) VALUES (" + 
                        std::to_string(userId) + ", " + std::to_string(commentId) + ", CURRENT_TIMESTAMP)");
            return true; 
        }
    }

    // Adicione no final do arquivo Comment.cpp

    bool Comment::deleteComment(int commentId, int requesterId, bool isAdmin) {
        auto* db = Core::Database::getInstance();

        // 1. VERIFICAÇÃO DE SEGURANÇA
        // Precisamos saber quem é o dono do comentário para ver se quem pediu pode deletar
        int authorId = -1;
        int postId = -1;
        
        // Lambda para pegar os dados antes de deletar
        auto cb = [&](int, char** argv, char**) { 
            authorId = std::stoi(argv[0]); 
            postId = std::stoi(argv[1]);
            return 0; 
        };
        db->query("SELECT author_id, post_id FROM comments WHERE id=" + std::to_string(commentId), cb);

        if (authorId == -1) return false; // Comentário nem existe

        // Se não for o dono E não for Admin, bloqueia.
        if (authorId != requesterId && !isAdmin) {
            return false;
        }

        // 2. FAXINA DE LIKES (Remove todos os likes desse comentário)
        db->execute("DELETE FROM comment_likes WHERE comment_id = " + std::to_string(commentId));

        // 3. FAXINA DE NOTIFICAÇÕES 
        // Remove avisos do tipo "Jeff comentou..." gerados por esse comentário
        // Assumindo que o notification.post_id aponta pro Post, mas precisamos filtrar pelo remetente e tipo
        // Tipo 2 = Comment, Tipo 4 = Reply (Verifique seus Enums no Notification.h)
        // Como o sistema de notificação é meio genérico, vamos tentar limpar o rastro principal:
        
        std::string sqlNotif = "DELETE FROM notifications WHERE sender_id = " + std::to_string(authorId) + 
                               " AND post_id = " + std::to_string(postId) + 
                               " AND (type = 2 OR type = 4);"; // Limpa Comentários e Respostas desse user nesse post
        db->execute(sqlNotif);


        // 4. FAXINA DE FILHOS (RESPOSTAS)
        // Se alguém respondeu a esse comentário, as respostas também devem sumir (Cascade)
        // Atenção: Isso é um delete simples de 1 nível. Se tiver resposta da resposta, precisaria ser recursivo.
        // Para simplificar agora, deletamos os filhos diretos.
        
        // Primeiro, pegamos os IDs dos filhos para deletar os likes DELES também
        std::vector<std::string> childrenIds;
        db->query("SELECT id FROM comments WHERE parent_id = " + std::to_string(commentId), 
                 [&](int, char** argv, char**){ childrenIds.push_back(argv[0]); return 0; });
        
        for(const auto& childId : childrenIds) {
             // Deleta likes dos filhos
             db->execute("DELETE FROM comment_likes WHERE comment_id = " + childId);
             // Deleta notificações dos filhos (Opcional, mas bom)
        }
        
        // Agora deleta os comentários filhos
        db->execute("DELETE FROM comments WHERE parent_id = " + std::to_string(commentId));


        // 5. O GRAND FINALE (Deleta o comentário em si)
        return db->execute("DELETE FROM comments WHERE id = " + std::to_string(commentId));
    }
}