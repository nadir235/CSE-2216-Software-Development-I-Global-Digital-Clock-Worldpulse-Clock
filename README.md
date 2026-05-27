# CSE-2216-Software-Development-I-Global-Digital-Clock
# 🌍 WorldPulse Clock v1.0
> **Global Time Zone Tracker** — A console-based C++ application to track your local time and any country's time across the globe.

---

## Features

- **Local Time Display** — Automatically detects your system's timezone and shows your current local time
- **Country Time Lookup** — Type any country name to instantly see its current time
- **12-Hour AM/PM Format** — All times shown in 12-hour clock with AM/PM indicator
- **24-Hour Format** — Also displays the 24-hour equivalent side by side
- **Time Difference Calculator** — Shows exactly how many hours/minutes ahead or behind a country is from your local time
- **UTC Offset Display** — Every clock shows its UTC offset (e.g. UTC+6, UTC+5:30, UTC-5)
- **Pin System** — Pin up to 5 countries to the Live Dashboard for simultaneous monitoring
- **Live Auto-Refresh Dashboard** — Refreshes every second in real time
- **100+ Countries Supported** — Includes half-hour and quarter-hour timezone offsets (India UTC+5:30, Nepal UTC+5:45, Iran UTC+3:30, Myanmar UTC+6:30, etc.)

---

## Supported Platforms

| Platform | Compiler | Status |
|----------|----------|--------|
| Windows  | MinGW / CodeBlocks g++ | Supported |
| Linux    | g++ | Supported |
| macOS    | clang++ / g++ | Supported |

---

## Requirements

- C++17 or later
- Any standard C++ compiler (g++, clang++, MSVC)

---

## How to Compile

### CodeBlocks (Windows)
1. Open CodeBlocks
2. Go to **File → New → Project → Console Application → C++**
3. Replace the contents of `main.cpp` with `WorldPulseClock.cpp`
4. Press **F9** (Build & Run)

### Command Line (Linux / macOS)
```bash
g++ -std=c++17 -O2 -o WorldPulseClock WorldPulseClock.cpp
./WorldPulseClock
```

### Command Line (Windows MinGW)
```bash
g++ -std=c++17 -O2 -o WorldPulseClock.exe WorldPulseClock.cpp
WorldPulseClock.exe
```

---

## How to Use

When you launch the program, you will see the main menu:

```
  [1] View Live Clock Dashboard
  [2] Look up a Country's Time
  [3] Pin a Country to Dashboard
  [4] Remove a Pinned Country
  [5] List All Supported Countries
  [0] Exit
```

### Option 1 — Live Clock Dashboard
Displays your local time and all pinned countries, auto-refreshing every second.
Press **ENTER** at any time to return to the menu.

### Option 2 — Country Time Lookup
Enter a country name (case-insensitive) to see:
- Your current local time
- That country's current time (12hr + 24hr)
- The time difference between you and that country

**Example inputs:** `japan`, `Bangladesh`, `USA`, `saudi arabia`, `UAE`

### Option 3 — Pin a Country
Pins a country to the Live Dashboard. Up to **5 countries** can be pinned at once.

### Option 4 — Remove a Pinned Country
Shows your pinned list and lets you remove one by number.

### Option 5 — List All Countries
Displays all 100+ supported countries with their UTC offsets.

---

## Example Output

```
  *** WORLDPULSE CLOCK v1.0 ***
  Global Time Zone Tracker

----------------------------------------------------------------
| [PC]  Your Local Time           Local System   [ YOUR LOCAL TIME ]  |
  08:45:30  AM   UTC+6                                              |
  Thursday, May 28, 2025                                            |
  24-hour: 08:45:30                                                 |
----------------------------------------------------------------

----------------------------------------------------------------
| [JP]  Japan                     Tokyo          [  WORLD TIME     ]  |
  11:45:30  AM   UTC+9                                              |
  Thursday, May 28, 2025                                            |
  24-hour: 11:45:30                                                 |
----------------------------------------------------------------

  >> Time Difference: Japan is 3 hour(s) AHEAD of your local time
```

---

## Country Reference (Selected)

| Country | Capital | UTC Offset |
|---------|---------|------------|
| Bangladesh | Dhaka | UTC+6 |
| India | New Delhi | UTC+5:30 |
| Nepal | Kathmandu | UTC+5:45 |
| Pakistan | Islamabad | UTC+5 |
| Japan | Tokyo | UTC+9 |
| China | Beijing | UTC+8 |
| UAE | Abu Dhabi | UTC+4 |
| Saudi Arabia | Riyadh | UTC+3 |
| United Kingdom | London | UTC+0 |
| USA | Washington D.C. | UTC-5 |
| Brazil | Brasilia | UTC-3 |
| Iran | Tehran | UTC+3:30 |
| Myanmar | Naypyidaw | UTC+6:30 |
| Australia | Canberra | UTC+10 |
| New Zealand | Wellington | UTC+12 |

> Use option **[5]** in the app to see the full list of 100+ countries.

---

## Known Notes

- **Windows console colors:** ANSI colors require Windows 10 version 1511 or later. The app enables Virtual Terminal Processing automatically.
- **Emoji/Unicode:** If your terminal does not support Unicode, country flags are shown as `[BD]`, `[JP]`, etc. instead.
- **DST (Daylight Saving Time):** The app uses fixed UTC offsets and does not automatically adjust for DST changes.
- **Live clock input:** The live dashboard (Option 1) refreshes every second. Press ENTER to exit it.

---

## Project Structure

```
WorldPulseClock/
│
└── WorldPulseClock.cpp     # Full source code (single file)
└── README.md               # This file
```

---

## License

This project is free to use, modify, and distribute for personal and educational purposes.

---

*Built with C++17 — WorldPulse Clock v1.0*
