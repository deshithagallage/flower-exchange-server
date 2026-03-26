# Flower Exchange MVP

A command-line order matching engine for flower trading with FIFO order book matching.

---

## Object Details

### Core Components

**Order**
- Unique order identifier
- Client order reference
- Instrument (Rose, Tulip, Lilies, Sunflower, Daisy)
- Side (1=BUY, 2=SELL)
- Quantity (10-1000, multiples of 10)
- Price (decimal > 0)

**OrderBook**
- Separate order books per instrument
- Buy side (ascending price)
- Sell side (ascending price)
- FIFO matching (price-time priority)

**Exchange**
- Manages 5 order books (one per instrument)
- Processes orders sequentially
- Generates execution reports

**ExecutionReport**
- Order ID (auto-generated)
- Client Order ID (from input)
- Execution Status (New, Fill, Rejected)
- Filled Quantity
- Execution Price

---

## Input Format

**File:** `orders.csv`

```csv
Client Order ID,Instrument,Side,Quantity,Price
CLIENT_001,Rose,1,100,55.00
CLIENT_002,Rose,2,100,55.00
CLIENT_003,Tulip,1,200,45.50
```

**Columns:**
- **Client Order ID**: String (any value)
- **Instrument**: Rose, Tulip, Lilies, Sunflower, Daisy (case-insensitive)
- **Side**: 1 (BUY) or 2 (SELL)
- **Quantity**: 10-1000 (must be multiple of 10)
- **Price**: Decimal > 0

---

## Output Format

**File:** `execution_report.csv`

```csv
Order ID,Client Order ID,Instrument,Side,Exec Status,Quantity,Price
ord1,CLIENT_001,Rose,1,Fill,100,55.00
ord2,CLIENT_002,Rose,2,Fill,100,55.00
ord3,CLIENT_003,Tulip,1,New,0,0.00
```

**Columns:**
- **Order ID**: ord1, ord2, ... (auto-generated)
- **Client Order ID**: From input
- **Instrument**: From input
- **Side**: 1 or 2
- **Exec Status**: New (queued), Fill (matched), Rejected (validation failed)
- **Quantity**: Filled quantity
- **Price**: Execution price

---

## How to Run

### Build

```bash
cd server/mvp
g++ -o flower-exchange main.cpp engine/*.cpp models/*.cpp services/*.cpp utils/*.cpp
```

### Execute

```bash
# Windows
.\flower-exchange.exe input.csv output.csv

# Linux/macOS
./flower-exchange input.csv output.csv
```

**Sample Input Files:** Sample input CSV files (`orders_1.csv` through `orders_7.csv`) are available in the `sample_inputs/` folder for testing.

### Example

```bash
.\flower-exchange.exe orders.csv execution_report.csv
```

The application reads `orders.csv`, processes all orders through the matching engine, and writes results to `execution_report.csv`.

---

## Validation Rules

- Instrument must be one of: Rose, Tulip, Lilies, Sunflower, Daisy
- Side must be 1 (BUY) or 2 (SELL)
- Quantity must be 10-1000 and a multiple of 10
- Price must be > 0

Invalid orders are rejected with status "Rejected".

---

## Example Workflow

**Input:** `orders.csv`
```csv
Client Order ID,Instrument,Side,Quantity,Price
TRADER_A,Rose,1,100,55.00
TRADER_B,Rose,2,100,55.00
TRADER_C,Tulip,1,200,45.50
```

**Command:**
```bash
.\flower-exchange.exe orders.csv report.csv
```

**Output:** `report.csv`
```csv
Order ID,Client Order ID,Instrument,Side,Exec Status,Quantity,Price
ord1,TRADER_A,Rose,1,Fill,100,55.00
ord2,TRADER_B,Rose,2,Fill,100,55.00
ord3,TRADER_C,Tulip,1,New,0,0.00
```
