#ifndef SOCIALNETWORK_H
#define SOCIALNETWORK_H

#include "Profile.h"
#include "User.h"
#include <vector>
#include <memory>

class SocialNetwork {
private:
  std::vector<std::unique_ptr<Profile>> profiles;

public:
  SocialNetwork();
  ~SocialNetwork();

  Profile* login(std::string name, std::string password);
  std::vector<Profile*> searchProfiles(std::string term);
  void verifyProfile(int userId, std::string newEmail);
  bool add(Profile* profile);
  void print();
  void printStatistics();

  const std::vector<std::unique_ptr<Profile>>& getProfiles() const;
  Profile* getProfile(int id); 
  int getProfilesAmount();
  std::vector<Post*> getTimeline(User* user);

};


#endif