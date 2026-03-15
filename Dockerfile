# Stage 1: Builder
FROM ubuntu:22.04 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
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

# Install Drogon framework from source
WORKDIR /tmp/drogon_build
RUN git clone https://github.com/drogonframework/drogon.git . && \
    git submodule update --init && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Copy and build application
WORKDIR /build
COPY . .
RUN cmake . -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# Stage 2: Runtime
FROM ubuntu:22.04

# Install only runtime dependencies needed
RUN apt-get update && apt-get install -y --no-install-recommends \
    libssl3 \
    libjsoncpp25 \
    uuid-runtime \
    zlib1g \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Copy Drogon runtime libraries from builder
COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/local/include /usr/local/include

# Create app user for security (don't run as root)
RUN useradd -m -u 1000 appuser

# Setup application directory
WORKDIR /app
COPY --from=builder --chown=appuser:appuser /build/flower-exchange-app .

# Create data directory for persistent CSV reports
RUN mkdir -p /data && chown appuser:appuser /data

# Update library cache
RUN ldconfig

# Switch to non-root user
USER appuser

# Expose default port (Railway will override via PORT env var)
EXPOSE 5555

# Health check that respects PORT environment variable
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD PORT_CHECK=${PORT:-5555} && curl -f http://localhost:${PORT_CHECK}/api/health || exit 1

# Run application
ENTRYPOINT ["./flower-exchange-app"]
