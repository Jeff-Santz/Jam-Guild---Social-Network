#ifndef NETWORKSTORAGE_H
#define NETWORKSTORAGE_H

#include "SocialNetwork.h"
#include "sqlite3.h"
#include "Profile.h"
#include "User.h"
#include "Page.h"
#include <iostream>
#include <string>
#include <stdexcept>

class NetworkStorage {
private:
    std::string dbFileName;
    sqlite3* db;

    void executeSQL(std::string sql);

public:
    NetworkStorage(std::string dbFileName);
    virtual ~NetworkStorage();

    void save(SocialNetwork* sn);
    void load(SocialNetwork* sn);
};

#endif