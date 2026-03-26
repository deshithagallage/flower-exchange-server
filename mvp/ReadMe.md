# Flower Exchange MVP - Running Guide

## Quick Start

### Prerequisites
- Windows: No prerequisites needed (executable included)
- Linux/macOS: `g++` or `clang`, `cmake`

---

## Running the Application

### On Windows (from `server/mvp` folder)

**Option 1: Command Prompt or PowerShell**
```bash
.\flower-exchange.exe input.csv output.csv
```

**Option 2: With Sample Data**
```bash
.\flower-exchange.exe sample_orders.csv results.csv
```

---

### On Linux/macOS

**Step 1: Build (if not already built)**
```bash
mkdir -p build
cd build
cmake ..
make
cd ..
```

**Step 2: Run**
```bash
./build/flower-exchange input.csv output.csv
```

---

## Input Format

Create a CSV file with 6 columns:

```csv
CLIENT_ID,ORDER_ID,INSTRUMENT,SIDE,PRICE,QUANTITY
CLIENT_001,ORD_001,ROSE,BUY,50.25,100
CLIENT_001,ORD_002,TULIP,SELL,45.50,50
CLIENT_002,ORD_003,LILIES,BUY,100.00,200
```

**Column Details:**
- `CLIENT_ID`: Any string identifier
- `ORDER_ID`: Unique order reference
- `INSTRUMENT`: One of: `ROSE`, `TULIP`, `LILIES`, `SUNFLOWER`, `DAISY`
- `SIDE`: `BUY` or `SELL`
- `PRICE`: Decimal number > 0
- `QUANTITY`: 10-1000, multiple of 10

---

## Output Format

The application generates a report CSV with execution results:

```csv
EXCHANGE_ORDER_ID,INSTRUMENT,SIDE,STATUS,FILLED_QUANTITY,EXECUTION_PRICE,REASON
EXO_1000,ROSE,BUY,QUEUED,0,0.00,Added to order book
EXO_1001,ROSE,SELL,FILLED,50,50.25,
EXO_1002,TULIP,BUY,QUEUED,0,0.00,Added to order book
EXO_1003,TULIP,SELL,FILLED,80,45.50,
```

**Status Codes:**
- `QUEUED` - Added to order book, awaiting match
- `FILLED` - Order fully matched
- `REJECTED` - Order validation failed

---

## Example Workflow

**Step 1: Create input file** (`test_orders.csv`)
```csv
CLIENT_ID,ORDER_ID,INSTRUMENT,SIDE,PRICE,QUANTITY
TRADER_A,ORD_1,ROSE,BUY,50.00,100
TRADER_B,ORD_2,ROSE,SELL,50.00,80
TRADER_C,ORD_3,TULIP,BUY,45.00,200
```

**Step 2: Run the exchange**
```powershell
.\flower-exchange.exe test_orders.csv execution_report.csv
```

**Step 3: View results**
```powershell
cat execution_report.csv
```

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| File not found | Use full path: `.\flower-exchange.exe C:\data\input.csv C:\data\output.csv` |
| Invalid quantity | Ensure quantity is 10-1000 and multiple of 10 |
| Unknown instrument | Use exact names: ROSE, TULIP, LILIES, SUNFLOWER, DAISY |
| Price must be positive | Ensure PRICE column > 0 |
| CSV format error | Check column order and data types match specification |

---

## Performance

- **1,000 orders:** < 50 ms
- **10,000 orders:** < 500 ms
- **100,000 orders:** < 5 seconds

---

## Docker (Optional)

To run in Docker:

```bash
docker build -t flower-exchange .
docker run -v C:\data:/data flower-exchange /data/input.csv /data/output.csv
```

---

For detailed architecture and API information, see `README_COMPREHENSIVE.md`
