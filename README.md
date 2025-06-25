# ⚡ Low-Latency Order Matching Engine

A high-performance, multithreaded C++17 order matching engine simulating exchange-grade matching logic with features like price-time priority, order lifecycle management, and real-time logging.

---

## 🚀 Features

- Low-latency matching engine built in modern C++ (C++17)
- Priority-based order matching (price-time priority)
- Support for order placement and cancellation
- Thread-safe architecture using `std::mutex` and `std::lock_guard`
- Signal handling (e.g., `SIGINT`) for graceful shutdown
- Logs trades and cancellations to a `trades.txt` file
- Modular code split across headers and implementation files

---

## 📁 Project Structure

├── Matchin Engine/  
├── main.cpp  
├── OrderBook.h  
├── OrderBook.cpp  
├── Order.h  
├── Order.cpp  
├── trades.txt  
└── README.md  


## 🛠️ Build & Run

### Prerequisites
- C++17 compatible compiler (e.g., GCC 9+, Clang, MSVC)
- CMake or g++ on Linux/Windows

### Using g++ (example)
```bash
g++ -std=c++17 -pthread main.cpp Order.cpp OrderBook.cpp -o MatchingEngine
./MatchingEngine
```

👨‍💻 Author
Sahil Mourya
LinkedIn - www.linkedin.com/in/sahil-mourya-b34827217
mouryasahil99@gmail.com
