# CLI Cashier & Inventory System (C++17)

A terminal-based Point of Sale (POS) and inventory management application built with modern C++ (C++17). This repository is structured iteratively by version to document the evolution of the software architecture and feature set over time.

---

## 🎯 Project Objectives

Developed continuously as part of a software engineering portfolio, this project demonstrates clean, structured, and defensive C++ programming practices:

1. **Modular Architecture:** Separating data models and function contracts (`.h`) from business logic implementations (`.cpp`) and the main execution flow (`main.cpp`).
2. **Memory & Container Management:** Utilizing STL containers (`std::vector`), pass-by-reference (`&`), and const-reference (`const &`) to minimize unnecessary data copying.
3. **Defensive Programming:** Preventing runtime crashes and infinite loops caused by invalid user input on `std::cin` through stream validation and buffer clearing.
4. **File I/O & System Operations:** Implementing persistent data storage using external files (`<fstream>`), directory management (`<filesystem>`), and real-time timestamps (`<ctime>`).

---

## ✨ Core Features

* **Shopping Cart Management:** Dynamically add, view, update quantities, and remove items from active transactions.
* **Automated Transaction Calculation:** Automatically computes subtotals, applies conditional discounts, and calculates cash change.
* **Persistent Storage & Receipt Export:** Maintains product catalogs and sales logs across sessions while exporting timestamped transaction receipts to `.txt` files.

---

## 📂 Repository Structure

Each development milestone is organized into its own directory (`Cashier_V1`, `Cashier_V2`, etc.) following a standardized layout:

```text
Cashier_Vx/
├── CMakeLists.txt     # Cross-platform build configuration
├── include/           # Header files (.h) for data models and class declarations
└── src/               # Source files (.cpp) for core logic and entry point
