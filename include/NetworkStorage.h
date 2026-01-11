#ifndef NETWORKSTORAGE_H
#define NETWORKSTORAGE_H

#include "SocialNetwork.h"
#include <iostream>
#include <string>
#include <stdexcept>

class NetworkStorage {
private:
    std::string file;

public:
    NetworkStorage(std::string file);
    virtual ~NetworkStorage();

    void save(SocialNetwork* r);
    SocialNetwork* load();

};

#endif