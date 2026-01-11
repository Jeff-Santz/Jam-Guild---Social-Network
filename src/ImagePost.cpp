#include "ImagePost.h"
#include "Profile.h"
#include <iostream>

using namespace std;

ImagePost::ImagePost(string text, Profile* owner, string imagePath) : Post(text, owner) {
    this->imagePath = imagePath;
}

ImagePost::ImagePost(string text, time_t date, Profile* owner, string imagePath) : Post(text, date, owner) {
    this->imagePath = imagePath;
}

void ImagePost::print() {
    cout << "[" << getFormattedDate() << "] " 
         << getOwner()->getName() << " postou uma FOTO: " << endl;
    cout << "   Legenda: " << getText() << endl;
    cout << "   [IMAGEM]: " << imagePath << endl; 
}