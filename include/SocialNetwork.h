#ifndef SOCIALNETWORK_H
#define SOCIALNETWORK_H

#include "Profile.h"
#include "User.h"
#include "Page.h"
#include <vector>
#include <memory>

class SocialNetwork {
private:
  std::vector<std::unique_ptr<Profile>> profiles;

public:
  SocialNetwork();
  ~SocialNetwork();

  Profile* login(int id, std::string password);
  std::vector<Profile*> searchProfiles(std::string term);
  void verifyProfile(int userId, std::string newEmail);
  bool add(Profile* profile);
  void print();
  void printStatistics();

  const std::vector<std::unique_ptr<Profile>>& getProfiles() const;
  std::vector<Page*> getPagesByOwner(int ownerId);
  Profile* getProfile(int id); 
  int getProfilesAmount();
  std::vector<Post*> getTimeline(User* user);

};


#endif