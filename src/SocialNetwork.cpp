#include "SocialNetwork.h"
#include "Profile.h"
#include "User.h"
#include "VerifiedUser.h"
#include "Page.h"
#include "ProfileNotFound.h"
#include <fstream>

using namespace std;

//Construtores e Destrutores
SocialNetwork::SocialNetwork (){
  profiles = new Profile*[capacity];
};

SocialNetwork::~SocialNetwork() {
  cout << "SocialNetwork destructor: " << profilesAmount << " profiles" << endl;
  	for (int i = 0; i < profilesAmount; i++) {
        delete profiles[i];
    }
    delete[] profiles;
  cout << "SocialNetwork deleted " << endl;
}

bool SocialNetwork::add (Profile* profile) {
  if (this->profilesAmount < capacity) {
    profiles[profilesAmount] = profile;
    profilesAmount ++;
    return true;
  }
  return false;
}


void SocialNetwork::print () {
  cout << "==================================" << endl;
  cout << "Social Network: " << profilesAmount << " profiles" << endl;
  cout << "==================================" << endl;
  if (profilesAmount == 0){
    cout << "No profiles" << endl;
    cout << "==================================" << endl;
  } else {
    for (int i = 0; i < profilesAmount; i++){
      profiles[i]->print();
      cout << "==================================" << endl;
    }
  }
  cout << endl;
}


void SocialNetwork::printStatistics() {
  int VerifiedUsersAmount = 0;
  int UsersAmount = 0;
  int PagesAmount = 0;
  int ProfilesAmount = 0;
  for (int i = 0; i < this->profilesAmount; i++) {
    if (dynamic_cast<VerifiedUser*>(profiles[i]) != NULL) {
        VerifiedUsersAmount++;
    }
    else if (dynamic_cast<User*>(profiles[i]) != NULL) {
        UsersAmount++;
    }
    else if (dynamic_cast<Page*>(profiles[i]) != NULL) {
        PagesAmount++;
    }
    else {
        ProfilesAmount++;
    }
  }

  cout << "VerifiedUser(s): " << VerifiedUsersAmount << endl;
  cout << "NotVerifiedUser(s): " << UsersAmount << endl;
  cout << "Page(s): " << PagesAmount << endl;
  cout << "Profiles: " << ProfilesAmount << endl;
}

int SocialNetwork::getProfilesAmount() {
  return this->profilesAmount;
}

Profile** SocialNetwork::getProfiles() {
  return profiles;
}

Profile* SocialNetwork::getProfile(int id) {
  for (int i=0; i < this->profilesAmount; i++) {
    if (this->profiles[i]->getId() == id) {
      return profiles[i];
    }
  }
  throw ProfileNotFound();
}







