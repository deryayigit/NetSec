# Port and Service Scanner with OS Detection

## 1. Basic Information

### 1.1 Problem Definition and Purpose
A C++ program that:
- Detects the operating system of a remote machine given its IP address
- Lists open TCP and UDP ports
- Identifies the services running on these ports
- Saves all results to both a file and the console with timestamps

**Project Requirements:**
- Estimate the OS from service banner data
- Perform simultaneous TCP and UDP port scanning
- Detect service names on open ports
- Log results with timestamps to a file and display on screen

---

### 1.2 Project Sections
- **Input Unit:** Accept and resolve IP/domain name
- **TCP Port Scanning:** Connect to each TCP port and retrieve service information
- **UDP Port Scanning:** Send a basic test message via UDP and await a response
- **Banner Analysis:** Extract banner data from TCP connections
- **OS Detection:** Estimate the possible OS based on banner content
- **Data Logging:** Save results to `sonuclar_windows.txt`
- **Concurrency:** Use threads for parallel scanning

---

### 1.3 Technologies and Tools
- **Programming Language:** C++
- **IDE:** Microsoft Visual Studio (Windows)
- **Libraries:**
  - `winsock2.h` & `ws2tcpip.h` (network programming)
  - `thread`, `mutex` (multithreading)
  - `fstream`, `map`, `ctime` (data processing & file logging)

---

### 1.4 Hardware Requirements
- **Processor:** Any modern x86_64 CPU
- **Memory:** Minimum 8 GB RAM
- **Storage:** Minimum 8 GB free disk space
- **Operating System:** Windows 10/11 (development environment), optional Linux testing via virtual machines

---

## 2. Technical Requirements

### 2.1 Project Algorithm
1. Receive IP or domain from the user and resolve it
2. Connect to specific TCP ports and retrieve service banners
3. Analyze banner data for OS estimation
4. Perform TCP and UDP scanning in parallel
5. Log results to file and display them in the console

### 2.2 Critical Components
- **Winsock Initialization (`WSAStartup`):** Required for network functions in Windows
- **Banner Retrieval:** Critical for OS detection
- **Parallel Port Scanning:** Uses threads to reduce execution time

---

## 3. Overview
This project demonstrates the core techniques behind tools like **Nmap**.  
With features like simultaneous TCP/UDP scanning, banner retrieval, and OS estimation, it is designed as a practical, real-world solution rather than a purely theoretical exercise.

---

## 4. Example Output

