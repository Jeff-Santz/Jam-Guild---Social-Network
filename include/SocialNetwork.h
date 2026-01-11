#ifndef SOCIALNETWORK_H
#define SOCIALNETWORK_H

#include "Profile.h"

class SocialNetwork {
private:
  Profile** profiles;
  const int capacity = 100;
  int profilesAmount = 0;

public:
  SocialNetwork();
  ~SocialNetwork();

  Profile** getProfiles();
  Profile* getProfile(int id); 
  int getProfilesAmount();
  bool add(Profile* profile);
  void print();
  void printStatistics();

};


#endif