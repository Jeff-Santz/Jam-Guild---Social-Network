#include "SocialNetwork.h"
#include "Profile.h"
#include "User.h"
#include "Page.h"
#include "ProfileNotFound.h"
#include "Utils.h"
#include <fstream>
#include <map>
#include <algorithm>
#include <vector>

using namespace std;

//Construtores e Destrutores
SocialNetwork::SocialNetwork (){};

SocialNetwork::~SocialNetwork() {
  cout << "SocialNetwork destructor: " << profiles.size() << " profiles" << endl;
  cout << "SocialNetwork deleted " << endl;
}

Profile* SocialNetwork::login(string name, string passwordAttempt) {
  string hashedAttempt = Utils::hashPassword(passwordAttempt);

    for (auto& u_ptr : profiles) {
        if (u_ptr->getName() == name && u_ptr->checkPassword(hashedAttempt)) {
            return u_ptr.get(); 
        }
    }
    return nullptr; 
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

void SocialNetwork::verifyProfile(int userId, std::string newEmail) {
  Profile* p = getProfile(userId);
  
  if (p == nullptr) {
      cout << "Profile not found." << endl;
      return;
  }


  if (User* u = dynamic_cast<User*>(p)) {
      u->verify(newEmail);
      cout << "Success! User " << u->getName() << " is now Verified." << endl;
      
  } else {
      cout << "Error: Pages cannot be verified." << endl;
  }
}

int calculateDistance(const std::string& s1, const std::string& s2) {
    const size_t m = s1.size();
    const size_t n = s2.size();
    if (m == 0) return n;
    if (n == 0) return m;

    std::vector<std::vector<int>> d(m + 1, std::vector<int>(n + 1));

    for (int i = 0; i <= m; i++) d[i][0] = i;
    for (int j = 0; j <= n; j++) d[0][j] = j;

    for (int j = 1; j <= n; j++) {
        for (int i = 1; i <= m; i++) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            d[i][j] = std::min({ d[i - 1][j] + 1,       // deleção
                                 d[i][j - 1] + 1,       // inserção
                                 d[i - 1][j - 1] + cost // substituição
                               });
        }
    }
    return d[m][n];
}

std::vector<Profile*> SocialNetwork::searchProfiles(std::string term) {
    std::vector<Profile*> results;
    std::string termLower = term;
    std::transform(termLower.begin(), termLower.end(), termLower.begin(), ::tolower);

    for (auto& u_ptr : profiles) {
        std::string nameLower = u_ptr->getName();
        std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

        // 1. Busca exata ou por substring (o que já tínhamos)
        if (nameLower.find(termLower) != std::string::npos) {
            results.push_back(u_ptr.get());
        } 
        // 2. Busca por proximidade (Fuzzy)
        // Se a distância for pequena (ex: 1 ou 2 letras de diferença)
        else if (calculateDistance(termLower, nameLower) <= 2) { 
            results.push_back(u_ptr.get());
        }
    }
    return results;
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







