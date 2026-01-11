#ifndef PROFILENOTFOUND_H
#define PROFILENOTFOUND_H

#include "Profile.h"
#include <iostream>
#include <string>
#include <stdexcept>

class ProfileNotFound : public std::logic_error {
public:
  ProfileNotFound ();
};

#endif