# Jam Guild - Social Network Engine (C++ API)

> **A high-performance C++ backend featuring a hybrid architecture (CLI & REST API) and a persistent social graph.**
> *Evolution from legacy logic to Modern C++ (C++17) with Relational Persistence and Networking.*

![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B)
![SQLite](https://img.shields.io/badge/Database-SQLite3-lightgrey.svg?style=flat&logo=sqlite)
![CMake](https://img.shields.io/badge/Build-CMake-orange.svg?style=flat&logo=cmake)
![Network](https://img.shields.io/badge/Network-Crow_Asio-red)

## Hybrid Architecture

The project is designed with a **Headless Core**, where the business logic is entirely decoupled from the interface. This allows the same engine to power multiple entry points:

* **JamGuildServer:** A high-concurrency REST API using the Crow framework.
* **JamGuildCLI:** A terminal interface for administrative tasks.
* **JamGuildTester:** A dedicated suite for unit testing and logic validation.

## Key Engineering Features

* **RESTful API Integration:**
    * Implementation of a web server using **Crow** and **Asio** (Standalone).
    * JSON serialization for profile data, timelines, and system status.
* **Modern Memory Management:**
    * Full migration to `std::unique_ptr` and `std::shared_ptr`.
    * Resource acquisition is initialization (RAII) to ensure zero memory leaks.
* **Relational Persistence (SQLite3):**
    * Custom `NetworkStorage` layer mapping polymorphic C++ objects to SQL.
    * Enabled **Write-Ahead Logging (WAL)** mode for improved database concurrency.
* **Security & Identity:**
    * **One-Way Hashing:** Use of the **djb2** algorithm for secure credential storage.
    * **Exception Handling:** Robust `try-catch` blocks protecting the server from runtime crashes and database locks.

## Polymorphic Profile System

The system leverages inheritance to manage distinct entities through a unified interface:

* **Standard User:** Core profile with social interaction capabilities.
* **Verified User:** Identity-validated profile with specialized badges and extended permissions.
* **Page:** Institutional profiles linked to an `Owner` (User), enabling a managed relationship between creators and entities.

## Project Structure

```text
Jam-Guild/
├── include/        # Header files, Crow framework, and Asio networking
├── src/            # Business logic, SQL mapping, and API Controllers
├── build/          # Compiled binaries (.exe) and SQLite database
├── CMakeLists.txt  # Multi-target build configuration (Server, CLI, Tester)
└── README.md       # Project Documentation