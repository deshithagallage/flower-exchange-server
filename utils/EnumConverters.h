#pragma once

#include "../models/Enums.h"
#include <string>

namespace flower_exchange {

// Side conversions
inline Side strToSide(const std::string& s) {
    return (s == "BUY") ? Side::BUY : Side::SELL;
}

inline std::string sideStr(Side s) {
    return (s == Side::BUY) ? "BUY" : "SELL";
}

// Instrument conversions
inline Instrument strToInstrument(const std::string& s) {
    if (s == "ROSE") return Instrument::ROSE;
    if (s == "TULIP") return Instrument::TULIP;
    if (s == "LILIES") return Instrument::LILIES;
    if (s == "SUNFLOWER") return Instrument::SUNFLOWER;
    if (s == "DAISY") return Instrument::DAISY;
    return Instrument::UNKNOWN;
}

inline std::string instrumentStr(Instrument i) {
    switch (i) {
        case Instrument::ROSE: return "ROSE";
        case Instrument::TULIP: return "TULIP";
        case Instrument::LILIES: return "LILIES";
        case Instrument::SUNFLOWER: return "SUNFLOWER";
        case Instrument::DAISY: return "DAISY";
        default: return "UNKNOWN";
    }
}

// ExecutionStatus conversions
inline std::string executionStatusStr(ExecutionStatus s) {
    switch (s) {
        case ExecutionStatus::REJECTED: return "Rejected";
        case ExecutionStatus::NEW: return "New";
        case ExecutionStatus::FILL: return "Fill";
        case ExecutionStatus::PFILL: return "PFill";
        default: return "Unknown";
    }
}

} // namespace flower_exchange
