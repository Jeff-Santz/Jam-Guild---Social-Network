#ifndef UIHANDLERS_H
#define UIHANDLERS_H 

#include "SocialNetwork.h"
#include "NetworkStorage.h"
#include "User.h"
#include "Page.h"

namespace UIHandlers {
    int showVisitorMenu();
    int showUserMenu(Profile* user);
    int showPageMenu(Page* p);
    void showNotifications(Profile* user);

    void handleLogin(SocialNetwork* sn, Profile*& currentUser);
    void handleCreateAccount(SocialNetwork* sn, NetworkStorage& storage);
    void handleViewProfile(Profile* user);
    void handleCreatePost(Profile* user, NetworkStorage& storage, SocialNetwork& sn);
    void handleCreatePage(Profile* currentUser, SocialNetwork& sn, NetworkStorage& storage);
    void handleManageMyPages(Profile* currentUser, SocialNetwork& sn, Page*& currentPage);
    void handleEditProfile(Profile* user, NetworkStorage& storage, SocialNetwork& sn);
    void handleSearchUsers(Profile* user, SocialNetwork& sn, NetworkStorage& storage);
    void handleVerify(Profile* user, SocialNetwork& sn, NetworkStorage& storage);
    void handleTimeline(Profile* currentUser, SocialNetwork& sn, NetworkStorage& storage);
}
#endif