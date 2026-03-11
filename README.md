# flower-exchange-server
Server codebase for the Flower Exchange implementing the order matching engine, order validation, and instrument order book management in C++.

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
