# Social Engine (C++ Core) - Beta v1.3.5

> **High-Performance Social Network Backend written in C++**

**Social Engine** is a robust, high-performance RESTful API developed in modern C++ (C++17/20), utilizing the Crow microframework and SQLite. This project serves as the central engine for a complete social network, handling authentication, complex relationship graphs, community management, and media distribution.

---

## Project Status

- **Backend (C++):** STABLE (v1.3.5) - Implements RBAC, Privacy Logic, and Content Hierarchies.
- **Frontend (Vue.js):** BETA - Single File Component (`index.html`) fully integrated with the API.

---

## Version 1.3.5 Changelog

The latest release introduces significant logic improvements:

- **Hybrid Authentication:** Login endpoint now accepts a generic `identifier` field, automatically resolving to either Username or Email.
- **Privacy Hardening:** SQL logic updated to strictly enforce private profile rules. Posts from locked accounts are now invisible to non-friends in the global feed.
- **Community Workflows:** Implemented **Direct Join** vs **Request to Join** logic based on community privacy settings. Added Admin approval endpoints.
- **Media Handling:** Full Base64 support for User Avatars, Cover Photos, and Post Media.

---

## Engine Features

### Core Architecture & Security

- **HTTP Server:** Powered by Crow (Asynchronous C++ Microframework).
- **Authentication:** JWT (JSON Web Tokens) system with Bearer Auth.
- **Hybrid Login:** Smart resolution of credentials (Email/Username).
- **Database:** Integrated SQLite with enforced Foreign Keys and `ON DELETE CASCADE` for data integrity.
- **Localization (i18n):** Native internationalization system (PT-BR/EN-US) embedded in the binary.

---

### User Management & Social Graph

#### Profiles

Profiles support:

- Bio
- Birth Date
- Avatar (Base64)
- Cover Photo (Base64)

#### Privacy Logic

Two visibility modes exist:

- **Public**
  - Visible to all users.

- **Private**
  - Content visible only to accepted friends.

#### Friendship System

Bi-directional relationships with status states:

- `Pending`
- `Accepted`
- `Blocked`

#### Search

User discovery implemented through SQL queries using `LIKE`.

Example behavior:


/users/search?q=john


---

### Communities (Groups)

#### Access Control

Communities support different access types:

- **Public Groups**
  - Immediate entry upon join.

- **Private Groups**
  - Users must request entry.
  - Admins must approve via the API.

#### Role Management

Hierarchical role system:


Master

↓

Admin

↓

Member


Capabilities:

- **Master:** Full control, can promote or demote members.
- **Admin:** Moderation privileges.
- **Member:** Standard participation.

#### Content Isolation

Community posts:

- Appear in the **global feed**
- Are **tagged with the community**
- Remain within the **community context**

---

### Content & Interaction

#### Algorithmic Feed

Chronological timeline aggregating:

1. Self posts
2. Friend posts (respecting privacy)
3. Community posts
4. Public content

#### Media Support

Handles large Base64 payloads for:

- Post images
- Avatars
- Cover photos

#### Comments

Supports **recursive / nested comment structures** for threaded discussions.

#### Notifications

Database-backed notification system for:

- Likes
- Comments
- Community approvals

#### Input Validation

Server-side validation includes character limits:

- **Posts:** 300 characters
- **Comments:** 200 characters

---

## How to Build & Run (Windows Setup Guide)

This guide explains how to configure the development environment and run **Social Engine** on Windows using:

- **Visual Studio Code**
- **MSYS2**
- **MinGW (GCC)**
- **OpenSSL**
- **CMake**
- **Ngrok**

---

## 1. Install Visual Studio Code

Download and install **Visual Studio Code**:

https://code.visualstudio.com/

After installing VS Code, install the following extensions:

- **C/C++ (Microsoft)**
- **CMake Tools (Microsoft)**

These extensions provide:

- C++ IntelliSense
- Build integration with CMake
- Debugging support

---

## 2. Install MSYS2

Since this project depends on **native C++ libraries**, we use **MSYS2** to install compilers and dependencies.

Download MSYS2:

https://www.msys2.org/

Install it normally.

After installation, open:

```
MSYS2 MinGW 64-bit
```

from the Windows Start Menu.

---

## 3. Install the Compiler and Dependencies

Inside the **MSYS2 MinGW 64-bit terminal**, run the following commands:

```bash
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-make
pacman -S mingw-w64-x86_64-openssl
```

This installs:

| Package | Purpose |
|-------|------|
| GCC | C++ Compiler |
| CMake | Build system generator |
| Make | Build automation tool |
| OpenSSL | Cryptographic library used by the backend |

---

## 4. Configure Environment Variables

To allow Windows and VS Code to locate the compiler, add the following path to your **System PATH**:

```
C:\msys64\mingw64\bin
```

Steps:

1. Open the **Start Menu**
2. Search for **Environment Variables**
3. Click **Edit the system environment variables**
4. Click **Environment Variables**
5. Find the **Path** variable
6. Add the path above

Restart **VS Code** after completing this step.

---

## 5. Open the Project in VS Code

Open the project folder in **Visual Studio Code**.

At the bottom blue status bar, click:

```
No Kit Selected
```

Then select:

```
GCC x86_64-w64-mingw32
```

This tells **CMake Tools** to use the **MinGW GCC compiler**.

---

## 6. Build the Project

Open the integrated terminal in VS Code and run:

```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```

Explanation:

| Command | Purpose |
|------|------|
| `mkdir build` | Creates a build directory |
| `cmake ..` | Generates the build configuration |
| `mingw32-make` | Compiles the project |

---

## 7. Run the Server

After compilation finishes, run the generated executable:

```
SocialEngine.exe
```

The API server should start at:

```
http://localhost:8085
```

You can now test the API using tools like:

- **Postman**
- **Insomnia**
- **curl**
- Your **frontend application**

---

## 8. Exposing the API to the Internet (Ngrok)

If you want to access the API from outside your local machine (for example from a remote frontend), you can expose the server using **Ngrok**.

### Install Ngrok

Download Ngrok:

https://ngrok.com/download

Extract the `ngrok.exe` file.

---

### Authenticate Ngrok

Run this command once:

```powershell
.\ngrok config add-authtoken <YOUR_AUTH_TOKEN>
```

You can find your token in the **Ngrok Dashboard**.

---

### Create a Public Tunnel

Expose the API port:

```powershell
.\ngrok http 8085
```

Ngrok will generate a public URL similar to:

```
https://abcd-1234.ngrok-free.app
```

All requests sent to this URL will be forwarded to:

```
http://localhost:8085
```

You can now use this URL as the **base URL for your API requests**.

### Created by:

```bash

Jeff Industries
2026
