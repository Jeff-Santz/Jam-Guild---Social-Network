#include "SocialNetwork.h"
#include "Profile.h"
#include "User.h"
#include "VerifiedUser.h"
#include "Page.h"
#include "ProfileNotFound.h"
#include <fstream>
#include <map>

using namespace std;

//Construtores e Destrutores
SocialNetwork::SocialNetwork (){};

SocialNetwork::~SocialNetwork() {
  cout << "SocialNetwork destructor: " << profiles.size() << " profiles" << endl;
  cout << "SocialNetwork deleted " << endl;
}

bool SocialNetwork::add (Profile* profile) {
  if(profile == nullptr) return false;

  profiles.push_back(std::unique_ptr<Profile>(profile));
  return true;
}


void SocialNetwork::print () {
  cout << "==================================" << endl;
  cout << "Social Network: " << profiles.size() << " profiles" << endl;
  cout << "==================================" << endl;
  if (profiles.size() == 0){
    cout << "No profiles" << endl;
    cout << "==================================" << endl;
  } else {
    for (auto& u_ptr : profiles){
      u_ptr->print();
      cout << "==================================" << endl;
    }
  }
  cout << endl;
}


void SocialNetwork::printStatistics() {
  std::map<string, int> stats;

  for (auto& u_ptr : profiles) {
      stats[u_ptr->getRole()]++;
  }

  cout << "=== Statistics ===" << endl;
  for (auto const& [role, count] : stats) {
      cout << role << ": " << count << endl;
  }
  cout << "Total Profiles: " << profiles.size() << endl;
}

int SocialNetwork::getProfilesAmount() {
  return this->profiles.size();
}

Profile* SocialNetwork::getProfile(int id) {
  for (auto& u_ptr : profiles) {
    if (u_ptr->getId() == id) {
        return u_ptr.get(); 
    }
      }
  throw ProfileNotFound();
}

const std::vector<std::unique_ptr<Profile>>& SocialNetwork::getProfiles() const {
    return this->profiles;
}







