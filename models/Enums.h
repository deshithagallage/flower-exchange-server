#pragma once

#include <string>

namespace flower_exchange {

enum class Instrument {
    ROSE,
    TULIP,
    LILIES,
    SUNFLOWER,
    DAISY,
    UNKNOWN
};

enum class Side {
    BUY,
    SELL
};

enum class OrderStatus {
    NEW,
    PARTIALLY_FILLED,
    FILLED,
    CANCELLED,
    REJECTED
};

enum class ExecutionStatus {
    REJECTED,
    NEW,
    FILL,
    PFILL
};

} // namespace flower_exchange

