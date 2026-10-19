# 🔐 delta-crypt

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.14+-orange.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Security](https://img.shields.io/badge/Security-libsodium-brightgreen.svg)](https://doc.libsodium.org/)

> *"Military-grade cryptographic protection for your most sensitive files."*

**delta-crypt** is a high-performance, secure command-line utility for file encryption and decryption. Built in modern C++17, it leverages the audited [libsodium](https://doc.libsodium.org/) library to provide authenticated encryption, brute-force resistant key derivation, and advanced memory hardening.

Designed to protect confidential investigation archives, personal data, and sensitive documents with zero compromise on performance or security.

---

## 📑 Table of Contents
- [✨ Key Features](#-key-features)
- [📋 Prerequisites](#-prerequisites)
- [📁 Project Structure](#-project-structure)
- [🔨 Building from Source](#-building-from-source)
- [🚀 Usage](#-usage)
- [🧠 Cryptographic Architecture](#-cryptographic-architecture)
- [🛡️ Memory Hardening & Security](#️-memory-hardening--security)
- [⚠️ Important Disclaimers](#️-important-disclaimers)

---

## ✨ Key Features

- 🔒 **Authenticated Encryption**: Utilizes `XChaCha20-Poly1305` for fast, secure, and tamper-evident data protection.
- 🔑 **Brute-Force Resistance**: Employs `Argon2id` for secure key derivation, making GPU/ASIC attacks computationally infeasible.
- 📦 **Memory-Efficient Streaming**: Processes files in 4KB chunks. Even multi-gigabyte files are encrypted without loading the entire payload into RAM.
- 🛡️ **Advanced Memory Hardening**: Actively prevents sensitive data (passwords, keys) from leaking to disk via swap, core dumps, or external debuggers.
- 📥 **Zero-Dependency Setup**: CMake `FetchContent` automatically downloads and builds `libsodium` and `CLI11`. The end-user installs *nothing* manually.
- 🧂 **Unique Per-File Salt**: Guarantees that encrypting the same file with the same password yields a completely different ciphertext every time.

---

## 📋 Prerequisites

To build `delta-crypt`, you only need a standard development environment:
- **CMake** `3.14` or higher
- **C++17 Compiler** (GCC 8+, Clang 7+, or MSVC 2019+)
- **Git** (required for fetching dependencies)
- **Active Internet Connection** *(only during the initial CMake configuration)*

> 💡 **Note:** You do **not** need to manually install `libsodium` or `CLI11`. The build system handles everything automatically via CMake's `FetchContent` module.

---

## 📁 Project Structure
    delta-crypt/
    ├── CMakeLists.txt # Build configuration and dependency management
    ├── README.md # This file
    └── src/
    └── main.cpp # Main application source code

---

## 🔨 Building from Source

```bash
# 1. Clone the repository
git clone https://github.com/your-username/delta-crypt.git
cd delta-crypt

cmake -B build ; cmake --build build ; sudo cmake --install build
```

## 🚀 Usage
Command-Line Options
    Flag: -e, --encrypt
    Description: Encrypt the specified file.
    
    Flag: -d, --decrypt
    Description: Decrypt the specified file.
    
    Flag: -f, --file <TEXT>
    Description: (Required) Path to the input file.
    
    Flag: -p, --password <TEXT>
    Description: (Required) Password for key derivation.

    Flag: -o, --output <TEXT>
    Description: (Optional) Path for the output file. Defaults to <input>.enc or <input>.dec.

    Flag: -h, --help
    Description: Display the help message and exit.

## 💡 Practical Examples

🔒 Encrypt a file:
``` bash
    delta-crypt -e -f confidential_report.pdf -p "SuperSecretPassword123!"
    # Output: confidential_report.pdf.enc
```

🔓 Decrypt a file:
``` bash
    delta-crypt -d -f confidential_report.pdf.enc -p "SuperSecretPassword123!" -o restored_report.pdf
    # Output: restored_report.pdf
```

❌ Handling Incorrect Passwords:
``` bash
    delta-crypt -d -f confidential_report.pdf.enc -p "WrongPassword"
    # Output: ❌ decryption failed (Authentication tag mismatch)
```
> The Poly1305 MAC ensures that any tampering or incorrect password is detected instantly.

## 🧠 Cryptographic Architecture
    ┌──────────────────────────────────────────────────────────────┐
    │ 1. SALT (16 bytes)                                           │
    │    └─ Randomly generated per file. Prevents rainbow tables.  │
    ├──────────────────────────────────────────────────────────────┤
    │ 2. HEADER (24 bytes)                                         │
    │    └─ Contains the initial nonce for the XChaCha20 stream.   │
    ├──────────────────────────────────────────────────────────────┤
    │ 3. ENCRYPTED CHUNKS (Variable length)                        │
    │    └─ Data is processed in 4096-byte blocks.                 │
    │    └─ Each block is appended with a 17-byte Poly1305 MAC.    │
    │    └─ The final block is marked with a TAG_FINAL.            │
    └──────────────────────────────────────────────────────────────┘
## ⚠️ Important Disclaimers
No Password Recovery: The encryption is mathematically strong. If you lose your password, your data is permanently unrecoverable. There is no backdoor.
Unique Ciphertexts: Encrypting the same file twice with the same password will produce two entirely different output files due to the random salt and nonce generation. This is a feature, not a bug.
Educational & Personal Use: While built with production-grade cryptographic primitives, this tool is provided "as is". Always verify your backups before deleting original files.

##🙏 Acknowledgments
libsodium by Frank Denis: For providing the most robust and easy-to-use cryptographic library available.
CLI11: For the best command-line parsing experience in modern C++.

