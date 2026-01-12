# 🛡️ Jam Guild - Social Network Core

> **A high-performance C++ backend engine featuring a persistent social graph architecture.**
> *Evolution from legacy logic to Modern C++ (C++17) with Relational Persistence.*

![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B)
![SQLite](https://img.shields.io/badge/Database-SQLite3-lightgrey.svg?style=flat&logo=sqlite)
![CMake](https://img.shields.io/badge/Build-CMake-orange.svg?style=flat&logo=cmake)
![Status](https://img.shields.io/badge/Status-Advanced-green)

## The Evolution

**Jam Guild** has evolved from a simple memory-based simulation to a robust backend engine. This project represents a deep dive into **Software Architecture**, migrating from manual memory management to **Smart Pointers** and integrating a **Relational Database** for long-term data persistence.

## Key Engineering Features

* **Modern Memory Management:** * Full migration to `std::unique_ptr` and `std::shared_ptr`.
    * Elimination of manual `new/delete` calls to ensure exception safety and zero memory leaks.
* **Relational Persistence (SQLite3):** * Custom-built `NetworkStorage` layer to map complex C++ objects to SQL tables.
    * Full persistence for Profiles, Connections, Posts, and Notifications.
* **Smart Social Logic:**
    * **Auto-Match System:** Intelligent friend request handling that detects mutual interest and creates connections instantly.
    * **Persistent Notification Engine:** An object-oriented notification system that survives application restarts.
* **Security & Identity:**
    * **Password Hashing:** Implementation of the **djb2** algorithm for non-plain-text credential storage.
    * **Unique ID Login:** Shift from name-based login to Unique Global IDs to handle name collisions professionally.

## Polymorphic Profile System

The system leverages inheritance and polymorphism to manage different types of entities:

* **Standard User:** Core profile with social interaction capabilities.
* **Verified User:** Identity-validated profile with special badges and extended permissions.
* **Page:** Institutional profiles linked to an `Owner` (User), enabling a Creator-to-Page management relationship.



## Project Structure

The project follows a strict separation of concerns:

```text
Jam-Guild/
├── include/        # Header files (.h) - Smart Pointer interfaces & Contracts
├── src/            # Source files (.cpp) - Business logic & SQL mapping
├── build/          # Compiled binaries and SQLite database file
├── CMakeLists.txt  # Multi-platform build configuration
└── README.md       # Project Documentation
