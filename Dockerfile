# Stage 1: Builder
FROM ubuntu:22.04 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    ca-certificates \
    cmake \
    g++ \
    git \
    pkg-config \
    libssl-dev \
    libjsoncpp-dev \
    uuid-dev \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

# Install Drogon from source
WORKDIR /tmp/drogon_build
RUN git clone https://github.com/drogonframework/drogon.git . && \
    git submodule update --init && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Copy source code
WORKDIR /app
COPY . .

# Build the application
RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# Stage 2: Runtime
FROM ubuntu:22.04

# Install runtime dependencies (only runtime libraries needed)
RUN apt-get update && apt-get install -y \
    libssl3 \
    libjsoncpp25 \
    uuid-runtime \
    zlib1g \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Copy Drogon libraries from builder
COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/local/include /usr/local/include

# Copy compiled application from builder
WORKDIR /app
COPY --from=builder /app/build/flower-exchange-app .

# Update library cache
RUN ldconfig 2>&1 || true

# Expose port (Railway will override dynamically)
EXPOSE 5555

# Health check (optional)
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD timeout 5 bash -c 'exec 3<>/dev/tcp/127.0.0.1/5555' || exit 1

# Run the application
CMD ["./flower-exchange-app"]
