#ifndef SOCIALNETWORK_H
#define SOCIALNETWORK_H

#include "Profile.h"
#include "User.h"
#include "Page.h"
#include <vector>
#include <map>
#include <memory>

class SocialNetwork {
private:
  std::vector<std::unique_ptr<Profile>> profiles;
  std::map<std::string, int> usernameCache;

public:
  SocialNetwork();
  ~SocialNetwork();

  Profile* signUp(std::string username, std::string password);
  Profile* signIn(int id, std::string password);
  std::vector<Profile*> searchProfiles(std::string term);
  bool verifyProfile(int userId, std::string newEmail);
  bool add(Profile* profile);
  Profile* createPage(User* creator, std::string pageName);
  
  std::string getSnapshot();
  std::string getStats();
  const std::vector<std::unique_ptr<Profile>>& getProfiles() const;
  std::vector<Page*> getPagesByOwner(int ownerId);
  Profile* getProfile(int id); 
  int getProfilesAmount();
  std::vector<Post*> getTimeline(User* user);
  int getIdByUsername(std::string name);

};


#endif