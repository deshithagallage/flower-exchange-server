# flower-exchange-server
Server codebase for the Flower Exchange implementing the order matching engine, order validation, and instrument order book management in C++.


## Getting Started with MVP

A basic working prototype application is included in the [mvp](mvp/) folder. This MVP demonstrates core functionality with a command-line order matching engine for flower trading.

**To get started with the MVP application:**
- See the [MVP README](mvp/README.md) for detailed documentation on the order matching engine, input/output formats, and sample data
- The MVP provides a simplified, self-contained version perfect for understanding the order matching logic before exploring the full server implementation

## Building

### Prerequisites
- CMake 3.10+
- C++17 compatible compiler
- Drogon framework

### Build Steps

```bash
mkdir build
cd build
cmake ..
make
```

## Running

```bash
./flower-exchange-app
```

The server will start and listen on `0.0.0.0:5555` by default.
