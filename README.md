# Social Engine (C++ Core) - Beta v0.9.2

> **High-Performance Social Network Backend written in C++**

**Social Engine** is a robust, high-performance RESTful API developed in modern C++ (C++17/20), utilizing the Crow microframework and SQLite. This project serves as the central "engine" for a complete social network, handling authentication, relationships, communities, and media distribution.

## 🚀 Project Status

* **Backend (C++):** ✅ **STABLE / FINISHED** (v0.9.2)
* **Frontend (HTML/JS):** ⚠️ **POC / EXPERIMENTAL** (Non-functional/Legacy)

> *Note: The included frontend (`index.html`) serves only as a Proof of Concept (POC) and does not currently reflect the full capabilities of the API. The focus of this release is the stability of the C++ Engine.*

## 🔥 Engine Features (Backend)

### 🛡️ Core & Security
* **HTTP Server:** Powered by **Crow** (Asynchronous C++ Microframework).
* **Authentication:** JWT (JSON Web Tokens) system with Bearer Auth.
* **Database:** Integrated SQLite with automatic migrations and connection pooling.
* **Localization (i18n):** Native internationalization system (PT-BR/EN-US) embedded in the binary.

### 👤 Users & Social
* **Profiles:** Support for Bio, Birth Date, Avatar, and Cover images (Base64/URL).
* **Privacy:** Robust logic for Locked (Private) vs. Public profiles.
* **Friendship System:** Pending requests, accept/reject logic, and friend lists.
* **Search:** User indexing by name/username.

### 🏘️ Communities (Groups)
* **Role Management:** Role system (Owner, Admin, Member).
* **Privacy:** Public Communities (Direct join) vs. Private (Request approval).
* **Isolated Feed:** Exclusive timeline for community posts.
* **Moderation:** Kick/Ban tools for administrators.

### 📡 Content & Interaction
* **Algorithmic Feed:** Chronological timeline mixing friend and community posts.
* **Media:** Image and Video upload support (50MB+ payloads) via Base64.
* **Interactions:** Atomic Likes and Comments.
* **Notifications:** Intelligent alert system (Likes, Comments, Invites, Friend Requests) with persistence.
* **Reports:** Abuse/Spam reporting system with Admin Dashboard (User ID 1).

## 🛠️ How to Build & Run

### Prerequisites
* C++ Compiler (GCC/MinGW or MSVC)
* CMake
* Libraries: Crow, SQLite3 (included or linked)

### Build Instructions
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release

Developed by Jeff Industries - 2026