#ifndef SOCIALNETWORK_H
#define SOCIALNETWORK_H

#include "Profile.h"
#include <vector>
#include <memory>

class SocialNetwork {
private:
  std::vector<std::unique_ptr<Profile>> profiles;

public:
  SocialNetwork();
  ~SocialNetwork();

  Profile* getProfile(int id); 
  int getProfilesAmount();
  bool add(Profile* profile);
  void print();
  void printStatistics();

};


#endif