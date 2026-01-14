#ifndef CONTACT_HANDLERS_H
#define CONTACT_HANDLERS_H

#include "SocialNetwork.h"
#include "NetworkStorage.h"
#include "User.h"

namespace ContactHandlers {
    void acceptFriendRequest(Profile* user, Profile* requester, SocialNetwork& sn, NetworkStorage& storage);
    void refuseFriendRequest(Profile* user, Profile* requester, SocialNetwork& sn, NetworkStorage& storage);

    void handleAddContact(Profile* currentUser, SocialNetwork& sn, NetworkStorage& storage);
    void handleRemoveContact(Profile* currentUser, SocialNetwork& sn, NetworkStorage& storage);
    void handleInspectFriend(Profile* currentUser, SocialNetwork& sn);
    void handleListContacts(Profile* currentUser);
    void handleSendRequest(Profile* currentUser, SocialNetwork& sn, NetworkStorage& storage);
    void coreSendFriendRequest(Profile* sender, Profile* target, SocialNetwork& sn, NetworkStorage& storage);
    void handleManageRequests(Profile* currentUser, SocialNetwork& sn, NetworkStorage& storage);
}

#endif
