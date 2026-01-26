# Social Engine (C++ Core) - Beta v1.3.5

> **High-Performance Social Network Backend written in C++**

**Social Engine** is a robust, high-performance RESTful API developed in modern C++ (C++17/20), utilizing the Crow microframework and SQLite. This project serves as the central engine for a complete social network, handling authentication, complex relationship graphs, community management, and media distribution.

## Project Status

* **Backend (C++):** STABLE (v1.3.5) - Implements RBAC, Privacy Logic, and Content Hierarchies.
* **Frontend (Vue.js):** BETA - Single File Component (index.html) fully integrated with the API.

## Version 1.3.5 Changelog

The latest release introduces significant logic improvements:

* **Hybrid Authentication:** Login endpoint now accepts a generic `identifier` field, automatically resolving to either Username or Email.
* **Privacy Hardening:** SQL logic updated to strictly enforce private profile rules. Posts from locked accounts are now invisible to non-friends in the global feed.
* **Community Workflows:** Implemented "Direct Join" vs. "Request to Join" logic based on community privacy settings. Added Admin approval endpoints.
* **Media Handling:** Full Base64 support for User Avatars, Cover Photos, and Post Media.

## Engine Features

### Core Architecture & Security
* **HTTP Server:** Powered by Crow (Asynchronous C++ Microframework).
* **Authentication:** JWT (JSON Web Tokens) system with Bearer Auth.
* **Hybrid Login:** Smart resolution of credentials (Email/Username).
* **Database:** Integrated SQLite with enforced Foreign Keys and ON DELETE CASCADE for data integrity.
* **Localization (i18n):** Native internationalization system (PT-BR/EN-US) embedded in the binary.

### User Management & Social Graph
* **Profiles:** Support for Bio, Birth Date, and Base64 Media (Avatar/Cover).
* **Privacy Logic:**
    * **Public:** Visible to all users.
    * **Private:** Content visible only to accepted friends.
* **Friendship System:** Bi-directional relationships with status states (Pending, Accepted, Blocked).
* **Search:** SQL `LIKE` queries for user discovery.

### Communities (Groups)
* **Access Control:**
    * **Public Groups:** Immediate entry upon join.
    * **Private Groups:** Users must request entry; Admins must approve via the API.
* **Role Management:** Hierarchical system (Master, Admin, Member). Masters can promote/demote members.
* **Content Isolation:** Community posts appear in the global feed with distinct tagging but belong to the community context.

### Content & Interaction
* **Algorithmic Feed:** Chronological timeline aggregating:
    1. Self posts.
    2. Friend posts (respecting privacy).
    3. Community posts.
    4. Public content.
* **Media Support:** Handling of large Base64 payloads for images.
* **Comments:** Recursive/Nested comment structure (Threaded view).
* **Notifications:** Database-backed system for interactions (Likes, Comments, Approvals).
* **Input Validation:** Server-side character limits (300 for posts, 200 for comments).

## How to Build & Run

### Prerequisites
* C++ Compiler (GCC, MinGW, or MSVC) supporting C++17 standard.
* CMake (3.10 or higher).
* Dependencies: Crow, SQLite3 (included or linked).

### Build Instructions

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release

Developed by Jeff Industries - 2026