/*
  =====================================
   WorldPulse Clock  v1.0
   Global Time Zone Tracker
  =====================================
*/

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <cmath>

// ── Platform Setup ──────────────────────────────────────────────────────────
#ifdef _WIN32
    #include <windows.h>
    #define CLEAR "cls"
    // Safe VT enable: define the constant manually if the SDK is old
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
    void enableVirtualTerminal() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return;
        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode)) return;
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        // Also set UTF-8 output code page
        SetConsoleOutputCP(65001);
    }
#else
    #define CLEAR "clear"
    void enableVirtualTerminal() {}
#endif

// ── ANSI Color Codes ─────────────────────────────────────────────────────────
const std::string RST      = "\033[0m";
const std::string BOLD     = "\033[1m";
const std::string DIM      = "\033[2m";

const std::string FG_RED    = "\033[31m";
const std::string FG_GREEN  = "\033[32m";
const std::string FG_YELLOW = "\033[33m";
const std::string FG_BLUE   = "\033[34m";
const std::string FG_CYAN   = "\033[36m";
const std::string FG_WHITE  = "\033[37m";

const std::string FG_BBLACK  = "\033[90m";
const std::string FG_BRED    = "\033[91m";
const std::string FG_BGREEN  = "\033[92m";
const std::string FG_BYELLOW = "\033[93m";
const std::string FG_BBLUE   = "\033[94m";
const std::string FG_BMAGENTA= "\033[95m";
const std::string FG_BCYAN   = "\033[96m";
const std::string FG_BWHITE  = "\033[97m";

const std::string BG_BLACK  = "\033[40m";
const std::string BG_BLUE   = "\033[44m";
const std::string BG_CYAN   = "\033[46m";

// ── Country Data ──────────────────────────────────────────────────────────────
struct CountryInfo {
    std::string name;
    std::string capital;
    float       utcOffset;
    std::string timezone;
    std::string flag;   // text flag like [BD]
};

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

std::map<std::string, CountryInfo> buildCountryDB() {
    std::map<std::string, CountryInfo> db;

    db["afghanistan"]          = {"Afghanistan",           "Kabul",           4.5f,   "UTC+4:30",  "[AF]"};
    db["albania"]              = {"Albania",               "Tirana",          1.0f,   "UTC+1",     "[AL]"};
    db["algeria"]              = {"Algeria",               "Algiers",         1.0f,   "UTC+1",     "[DZ]"};
    db["argentina"]            = {"Argentina",             "Buenos Aires",   -3.0f,   "UTC-3",     "[AR]"};
    db["armenia"]              = {"Armenia",               "Yerevan",         4.0f,   "UTC+4",     "[AM]"};
    db["australia"]            = {"Australia",             "Canberra",       10.0f,   "UTC+10",    "[AU]"};
    db["australia/sydney"]     = {"Australia (Sydney)",    "Sydney",         10.0f,   "UTC+10",    "[AU]"};
    db["australia/perth"]      = {"Australia (Perth)",     "Perth",           8.0f,   "UTC+8",     "[AU]"};
    db["austria"]              = {"Austria",               "Vienna",          1.0f,   "UTC+1",     "[AT]"};
    db["azerbaijan"]           = {"Azerbaijan",            "Baku",            4.0f,   "UTC+4",     "[AZ]"};
    db["bahrain"]              = {"Bahrain",               "Manama",          3.0f,   "UTC+3",     "[BH]"};
    db["bangladesh"]           = {"Bangladesh",            "Dhaka",           6.0f,   "UTC+6",     "[BD]"};
    db["belarus"]              = {"Belarus",               "Minsk",           3.0f,   "UTC+3",     "[BY]"};
    db["belgium"]              = {"Belgium",               "Brussels",        1.0f,   "UTC+1",     "[BE]"};
    db["bolivia"]              = {"Bolivia",               "Sucre",          -4.0f,   "UTC-4",     "[BO]"};
    db["brazil"]               = {"Brazil",                "Brasilia",       -3.0f,   "UTC-3",     "[BR]"};
    db["bulgaria"]             = {"Bulgaria",              "Sofia",           2.0f,   "UTC+2",     "[BG]"};
    db["cambodia"]             = {"Cambodia",              "Phnom Penh",      7.0f,   "UTC+7",     "[KH]"};
    db["cameroon"]             = {"Cameroon",              "Yaounde",         1.0f,   "UTC+1",     "[CM]"};
    db["canada"]               = {"Canada",                "Ottawa",         -5.0f,   "UTC-5",     "[CA]"};
    db["chile"]                = {"Chile",                 "Santiago",       -3.0f,   "UTC-3",     "[CL]"};
    db["china"]                = {"China",                 "Beijing",         8.0f,   "UTC+8",     "[CN]"};
    db["colombia"]             = {"Colombia",              "Bogota",         -5.0f,   "UTC-5",     "[CO]"};
    db["croatia"]              = {"Croatia",               "Zagreb",          1.0f,   "UTC+1",     "[HR]"};
    db["cuba"]                 = {"Cuba",                  "Havana",         -5.0f,   "UTC-5",     "[CU]"};
    db["czechia"]              = {"Czechia",               "Prague",          1.0f,   "UTC+1",     "[CZ]"};
    db["czech republic"]       = {"Czech Republic",        "Prague",          1.0f,   "UTC+1",     "[CZ]"};
    db["denmark"]              = {"Denmark",               "Copenhagen",      1.0f,   "UTC+1",     "[DK]"};
    db["ecuador"]              = {"Ecuador",               "Quito",          -5.0f,   "UTC-5",     "[EC]"};
    db["egypt"]                = {"Egypt",                 "Cairo",           2.0f,   "UTC+2",     "[EG]"};
    db["ethiopia"]             = {"Ethiopia",              "Addis Ababa",     3.0f,   "UTC+3",     "[ET]"};
    db["finland"]              = {"Finland",               "Helsinki",        2.0f,   "UTC+2",     "[FI]"};
    db["france"]               = {"France",                "Paris",           1.0f,   "UTC+1",     "[FR]"};
    db["georgia"]              = {"Georgia",               "Tbilisi",         4.0f,   "UTC+4",     "[GE]"};
    db["germany"]              = {"Germany",               "Berlin",          1.0f,   "UTC+1",     "[DE]"};
    db["ghana"]                = {"Ghana",                 "Accra",           0.0f,   "UTC+0",     "[GH]"};
    db["greece"]               = {"Greece",                "Athens",          2.0f,   "UTC+2",     "[GR]"};
    db["hungary"]              = {"Hungary",               "Budapest",        1.0f,   "UTC+1",     "[HU]"};
    db["iceland"]              = {"Iceland",               "Reykjavik",       0.0f,   "UTC+0",     "[IS]"};
    db["india"]                = {"India",                 "New Delhi",       5.5f,   "UTC+5:30",  "[IN]"};
    db["indonesia"]            = {"Indonesia",             "Jakarta",         7.0f,   "UTC+7",     "[ID]"};
    db["iran"]                 = {"Iran",                  "Tehran",          3.5f,   "UTC+3:30",  "[IR]"};
    db["iraq"]                 = {"Iraq",                  "Baghdad",         3.0f,   "UTC+3",     "[IQ]"};
    db["ireland"]              = {"Ireland",               "Dublin",          0.0f,   "UTC+0",     "[IE]"};
    db["israel"]               = {"Israel",                "Jerusalem",       2.0f,   "UTC+2",     "[IL]"};
    db["italy"]                = {"Italy",                 "Rome",            1.0f,   "UTC+1",     "[IT]"};
    db["jamaica"]              = {"Jamaica",               "Kingston",       -5.0f,   "UTC-5",     "[JM]"};
    db["japan"]                = {"Japan",                 "Tokyo",           9.0f,   "UTC+9",     "[JP]"};
    db["jordan"]               = {"Jordan",                "Amman",           3.0f,   "UTC+3",     "[JO]"};
    db["kazakhstan"]           = {"Kazakhstan",            "Astana",          5.0f,   "UTC+5",     "[KZ]"};
    db["kenya"]                = {"Kenya",                 "Nairobi",         3.0f,   "UTC+3",     "[KE]"};
    db["north korea"]          = {"North Korea",           "Pyongyang",       9.0f,   "UTC+9",     "[KP]"};
    db["south korea"]          = {"South Korea",           "Seoul",           9.0f,   "UTC+9",     "[KR]"};
    db["korea"]                = {"South Korea",           "Seoul",           9.0f,   "UTC+9",     "[KR]"};
    db["kuwait"]               = {"Kuwait",                "Kuwait City",     3.0f,   "UTC+3",     "[KW]"};
    db["kyrgyzstan"]           = {"Kyrgyzstan",            "Bishkek",         6.0f,   "UTC+6",     "[KG]"};
    db["laos"]                 = {"Laos",                  "Vientiane",       7.0f,   "UTC+7",     "[LA]"};
    db["latvia"]               = {"Latvia",                "Riga",            2.0f,   "UTC+2",     "[LV]"};
    db["lebanon"]              = {"Lebanon",               "Beirut",          2.0f,   "UTC+2",     "[LB]"};
    db["libya"]                = {"Libya",                 "Tripoli",         2.0f,   "UTC+2",     "[LY]"};
    db["lithuania"]            = {"Lithuania",             "Vilnius",         2.0f,   "UTC+2",     "[LT]"};
    db["luxembourg"]           = {"Luxembourg",            "Luxembourg",      1.0f,   "UTC+1",     "[LU]"};
    db["malaysia"]             = {"Malaysia",              "Kuala Lumpur",    8.0f,   "UTC+8",     "[MY]"};
    db["maldives"]             = {"Maldives",              "Male",            5.0f,   "UTC+5",     "[MV]"};
    db["mexico"]               = {"Mexico",                "Mexico City",    -6.0f,   "UTC-6",     "[MX]"};
    db["moldova"]              = {"Moldova",               "Chisinau",        2.0f,   "UTC+2",     "[MD]"};
    db["mongolia"]             = {"Mongolia",              "Ulaanbaatar",     8.0f,   "UTC+8",     "[MN]"};
    db["morocco"]              = {"Morocco",               "Rabat",           1.0f,   "UTC+1",     "[MA]"};
    db["mozambique"]           = {"Mozambique",            "Maputo",          2.0f,   "UTC+2",     "[MZ]"};
    db["myanmar"]              = {"Myanmar",               "Naypyidaw",       6.5f,   "UTC+6:30",  "[MM]"};
    db["nepal"]                = {"Nepal",                 "Kathmandu",       5.75f,  "UTC+5:45",  "[NP]"};
    db["netherlands"]          = {"Netherlands",           "Amsterdam",       1.0f,   "UTC+1",     "[NL]"};
    db["new zealand"]          = {"New Zealand",           "Wellington",     12.0f,   "UTC+12",    "[NZ]"};
    db["nigeria"]              = {"Nigeria",               "Abuja",           1.0f,   "UTC+1",     "[NG]"};
    db["norway"]               = {"Norway",                "Oslo",            1.0f,   "UTC+1",     "[NO]"};
    db["oman"]                 = {"Oman",                  "Muscat",          4.0f,   "UTC+4",     "[OM]"};
    db["pakistan"]             = {"Pakistan",              "Islamabad",       5.0f,   "UTC+5",     "[PK]"};
    db["palestine"]            = {"Palestine",             "Ramallah",        2.0f,   "UTC+2",     "[PS]"};
    db["panama"]               = {"Panama",                "Panama City",    -5.0f,   "UTC-5",     "[PA]"};
    db["paraguay"]             = {"Paraguay",              "Asuncion",       -4.0f,   "UTC-4",     "[PY]"};
    db["peru"]                 = {"Peru",                  "Lima",           -5.0f,   "UTC-5",     "[PE]"};
    db["philippines"]          = {"Philippines",           "Manila",          8.0f,   "UTC+8",     "[PH]"};
    db["poland"]               = {"Poland",                "Warsaw",          1.0f,   "UTC+1",     "[PL]"};
    db["portugal"]             = {"Portugal",              "Lisbon",          0.0f,   "UTC+0",     "[PT]"};
    db["qatar"]                = {"Qatar",                 "Doha",            3.0f,   "UTC+3",     "[QA]"};
    db["romania"]              = {"Romania",               "Bucharest",       2.0f,   "UTC+2",     "[RO]"};
    db["russia"]               = {"Russia",                "Moscow",          3.0f,   "UTC+3",     "[RU]"};
    db["saudi arabia"]         = {"Saudi Arabia",          "Riyadh",          3.0f,   "UTC+3",     "[SA]"};
    db["senegal"]              = {"Senegal",               "Dakar",           0.0f,   "UTC+0",     "[SN]"};
    db["serbia"]               = {"Serbia",                "Belgrade",        1.0f,   "UTC+1",     "[RS]"};
    db["singapore"]            = {"Singapore",             "Singapore",       8.0f,   "UTC+8",     "[SG]"};
    db["slovakia"]             = {"Slovakia",              "Bratislava",      1.0f,   "UTC+1",     "[SK]"};
    db["slovenia"]             = {"Slovenia",              "Ljubljana",       1.0f,   "UTC+1",     "[SI]"};
    db["somalia"]              = {"Somalia",               "Mogadishu",       3.0f,   "UTC+3",     "[SO]"};
    db["south africa"]         = {"South Africa",          "Pretoria",        2.0f,   "UTC+2",     "[ZA]"};
    db["spain"]                = {"Spain",                 "Madrid",          1.0f,   "UTC+1",     "[ES]"};
    db["sri lanka"]            = {"Sri Lanka",             "Colombo",         5.5f,   "UTC+5:30",  "[LK]"};
    db["sudan"]                = {"Sudan",                 "Khartoum",        3.0f,   "UTC+3",     "[SD]"};
    db["sweden"]               = {"Sweden",                "Stockholm",       1.0f,   "UTC+1",     "[SE]"};
    db["switzerland"]          = {"Switzerland",           "Bern",            1.0f,   "UTC+1",     "[CH]"};
    db["syria"]                = {"Syria",                 "Damascus",        3.0f,   "UTC+3",     "[SY]"};
    db["taiwan"]               = {"Taiwan",                "Taipei",          8.0f,   "UTC+8",     "[TW]"};
    db["tajikistan"]           = {"Tajikistan",            "Dushanbe",        5.0f,   "UTC+5",     "[TJ]"};
    db["tanzania"]             = {"Tanzania",              "Dodoma",          3.0f,   "UTC+3",     "[TZ]"};
    db["thailand"]             = {"Thailand",              "Bangkok",         7.0f,   "UTC+7",     "[TH]"};
    db["tunisia"]              = {"Tunisia",               "Tunis",           1.0f,   "UTC+1",     "[TN]"};
    db["turkey"]               = {"Turkey",                "Ankara",          3.0f,   "UTC+3",     "[TR]"};
    db["turkmenistan"]         = {"Turkmenistan",          "Ashgabat",        5.0f,   "UTC+5",     "[TM]"};
    db["uganda"]               = {"Uganda",                "Kampala",         3.0f,   "UTC+3",     "[UG]"};
    db["ukraine"]              = {"Ukraine",               "Kyiv",            2.0f,   "UTC+2",     "[UA]"};
    db["uae"]                  = {"UAE",                   "Abu Dhabi",       4.0f,   "UTC+4",     "[AE]"};
    db["united arab emirates"] = {"United Arab Emirates",  "Abu Dhabi",       4.0f,   "UTC+4",     "[AE]"};
    db["uk"]                   = {"United Kingdom",        "London",          0.0f,   "UTC+0",     "[GB]"};
    db["united kingdom"]       = {"United Kingdom",        "London",          0.0f,   "UTC+0",     "[GB]"};
    db["england"]              = {"England",               "London",          0.0f,   "UTC+0",     "[GB]"};
    db["usa"]                  = {"USA",                   "Washington D.C.", -5.0f,  "UTC-5",     "[US]"};
    db["united states"]        = {"United States",         "Washington D.C.", -5.0f,  "UTC-5",     "[US]"};
    db["america"]              = {"United States",         "Washington D.C.", -5.0f,  "UTC-5",     "[US]"};
    db["uruguay"]              = {"Uruguay",               "Montevideo",     -3.0f,   "UTC-3",     "[UY]"};
    db["uzbekistan"]           = {"Uzbekistan",            "Tashkent",        5.0f,   "UTC+5",     "[UZ]"};
    db["venezuela"]            = {"Venezuela",             "Caracas",        -4.0f,   "UTC-4",     "[VE]"};
    db["vietnam"]              = {"Vietnam",               "Hanoi",           7.0f,   "UTC+7",     "[VN]"};
    db["yemen"]                = {"Yemen",                 "Sana'a",          3.0f,   "UTC+3",     "[YE]"};
    db["zambia"]               = {"Zambia",                "Lusaka",          2.0f,   "UTC+2",     "[ZM]"};
    db["zimbabwe"]             = {"Zimbabwe",              "Harare",          2.0f,   "UTC+2",     "[ZW]"};

    return db;
}

// ── Time Utilities ────────────────────────────────────────────────────────────
struct ClockTime {
    int hour12, hour24, minute, second;
    std::string amPm;
    std::string dayName;
    std::string monthName;
    int day, year;
};

std::string getDayName(int wday) {
    const char* days[] = {"Sunday","Monday","Tuesday","Wednesday",
                          "Thursday","Friday","Saturday"};
    return days[wday % 7];
}

std::string getMonthName(int mon) {
    const char* months[] = {"January","February","March","April","May","June",
                            "July","August","September","October","November","December"};
    return months[mon % 12];
}

time_t getUtcTime() { return std::time(nullptr); }

ClockTime computeClockTime(time_t utcNow, float utcOffset) {
    int offsetSec = static_cast<int>(utcOffset * 3600.0f);
    time_t localTime = utcNow + offsetSec;
    struct tm tmBuf;
#ifdef _WIN32
    gmtime_s(&tmBuf, &localTime);
#else
    gmtime_r(&localTime, &tmBuf);
#endif
    ClockTime ct;
    ct.hour24    = tmBuf.tm_hour;
    ct.minute    = tmBuf.tm_min;
    ct.second    = tmBuf.tm_sec;
    ct.day       = tmBuf.tm_mday;
    ct.year      = tmBuf.tm_year + 1900;
    ct.dayName   = getDayName(tmBuf.tm_wday);
    ct.monthName = getMonthName(tmBuf.tm_mon);
    ct.amPm      = (ct.hour24 < 12) ? "AM" : "PM";
    ct.hour12    = ct.hour24 % 12;
    if (ct.hour12 == 0) ct.hour12 = 12;
    return ct;
}

std::string twoDigit(int n) {
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << n;
    return oss.str();
}

float getLocalUtcOffset() {
    time_t now = std::time(nullptr);
    struct tm gmt, local_tm;
#ifdef _WIN32
    gmtime_s(&gmt, &now);
    localtime_s(&local_tm, &now);
#else
    gmtime_r(&now, &gmt);
    localtime_r(&now, &local_tm);
#endif
    time_t gmt_t   = std::mktime(&gmt);
    time_t local_t = std::mktime(&local_tm);
    return static_cast<float>(std::difftime(local_t, gmt_t) / 3600.0);
}

// Build a readable UTC offset string like "UTC+6" or "UTC-3" or "UTC+5:30"
std::string buildOffsetStr(float offset) {
    std::string sign = (offset >= 0) ? "+" : "-";
    float abs_off = std::fabs(offset);
    int hours = static_cast<int>(abs_off);
    int mins  = static_cast<int>((abs_off - hours) * 60 + 0.5f);
    std::string result = "UTC" + sign + std::to_string(hours);
    if (mins > 0) result += ":" + twoDigit(mins);
    return result;
}

// ── Display Helpers ───────────────────────────────────────────────────────────
void printSeparator(int width = 64) {
    std::cout << FG_BBLACK << std::string(width, '-') << RST << "\n";
}

void printDoubleSep(int width = 64) {
    std::cout << FG_CYAN << std::string(width, '=') << RST << "\n";
}

void printBanner() {
    std::cout << "\n";
    std::cout << FG_BCYAN << BOLD
              << "  *** WORLDPULSE CLOCK v1.0 ***\n"
              << "  Global Time Zone Tracker\n"
              << RST;
    std::cout << FG_BBLACK
              << "  Track Your Local Time + Any Country's Time Across the Globe\n"
              << RST << "\n";
}

void printClockBox(const std::string& label,
                   const std::string& capital,
                   const ClockTime& ct,
                   const std::string& tz,
                   const std::string& flag,
                   bool isLocal)
{
    std::string lineColor = isLocal ? FG_BGREEN : FG_BMAGENTA;
    std::string timeColor = isLocal ? FG_BYELLOW : FG_BCYAN;
    std::string tag       = isLocal ? "[ YOUR LOCAL TIME ]" : "[  WORLD TIME     ]";

    std::cout << lineColor << std::string(64, '-') << RST << "\n";

    // Line 1: flag + name + capital + tag
    std::cout << lineColor << "|" << RST << " "
              << FG_BWHITE << BOLD << flag << "  "
              << std::left << std::setw(26) << label << RST
              << FG_BBLACK << std::setw(14) << capital << RST
              << FG_BBLUE << BOLD << tag << RST
              << lineColor << " |" << RST << "\n";

    // Line 2: big time display
    std::string timeStr = twoDigit(ct.hour12) + ":" + twoDigit(ct.minute) + ":" + twoDigit(ct.second);
    std::cout << lineColor << "|" << RST << "  "
              << timeColor << BOLD << std::setw(10) << timeStr << RST
              << "  "
              << FG_BYELLOW << BOLD << ct.amPm << RST
              << "   "
              << FG_BBLACK << std::left << std::setw(12) << tz << RST
              << "                       "
              << lineColor << "|" << RST << "\n";

    // Line 3: date
    std::string dateStr = ct.dayName + ", " + ct.monthName + " "
                        + std::to_string(ct.day) + ", " + std::to_string(ct.year);
    std::cout << lineColor << "|" << RST << "  "
              << FG_BBLACK << std::left << std::setw(58) << dateStr << RST
              << lineColor << "|" << RST << "\n";

    // Line 4: 24hr
    std::string h24 = "24-hour: " + twoDigit(ct.hour24) + ":" + twoDigit(ct.minute) + ":" + twoDigit(ct.second);
    std::cout << lineColor << "|" << RST << "  "
              << DIM << FG_BBLACK << std::left << std::setw(58) << h24 << RST
              << lineColor << "|" << RST << "\n";

    std::cout << lineColor << std::string(64, '-') << RST << "\n";
}

void printTimeDiff(float localOff, float worldOff, const std::string& country) {
    float diff = worldOff - localOff;
    int hours = static_cast<int>(std::fabs(diff));
    int mins  = static_cast<int>((std::fabs(diff) - hours) * 60 + 0.5f);

    std::cout << "\n  >> Time Difference: ";
    if (diff == 0.0f) {
        std::cout << FG_BGREEN << BOLD << country << " is in the SAME timezone as you!" << RST;
    } else {
        std::string direction = (diff > 0) ? "AHEAD of" : "BEHIND";
        std::cout << FG_BYELLOW << BOLD << country << RST
                  << FG_BBLACK << " is " << RST
                  << FG_BCYAN << BOLD << hours;
        if (mins > 0) std::cout << "h " << mins << "m";
        else          std::cout << " hour(s)";
        std::cout << RST
                  << FG_BBLACK << " " << direction << " your local time" << RST;
    }
    std::cout << "\n";
}

// ── Pinned list ───────────────────────────────────────────────────────────────
struct PinnedCountry {
    std::string key;
    CountryInfo info;
};

// ── Main ──────────────────────────────────────────────────────────────────────
int main() {
    enableVirtualTerminal();

    auto countryDB = buildCountryDB();
    float localOffset = getLocalUtcOffset();

    auto showMenu = [&]() {
        printDoubleSep(64);
        std::cout << FG_BCYAN  << "  [1]" << RST << FG_BWHITE << " View Live Clock Dashboard\n"      << RST
                  << FG_BCYAN  << "  [2]" << RST << FG_BWHITE << " Look up a Country's Time\n"       << RST
                  << FG_BCYAN  << "  [3]" << RST << FG_BWHITE << " Pin a Country to Dashboard\n"     << RST
                  << FG_BCYAN  << "  [4]" << RST << FG_BWHITE << " Remove a Pinned Country\n"        << RST
                  << FG_BCYAN  << "  [5]" << RST << FG_BWHITE << " List All Supported Countries\n"   << RST
                  << FG_BRED   << "  [0]" << RST << FG_BWHITE << " Exit\n"                           << RST
                  << "\n";
    };

    system(CLEAR);
    printBanner();
    showMenu();

    std::vector<PinnedCountry> pinnedList;
    bool running = true;

    while (running) {
        std::cout << FG_BYELLOW << BOLD << "  WorldPulse > " << RST;
        std::string choice;
        std::getline(std::cin, choice);
        choice = toLower(choice);

        // ─ [1] Live Dashboard ─────────────────────────────────────────────────
        if (choice == "1") {
            std::cout << "\n  " << FG_BBLACK
                      << "(Live clock running. Press ENTER to stop)\n" << RST << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(400));

            int ticks = 0;
            while (ticks < 3600) {
                system(CLEAR);
                printBanner();

                time_t utcNow   = getUtcTime();
                ClockTime local = computeClockTime(utcNow, localOffset);

                printDoubleSep(64);
                std::cout << FG_BWHITE << BOLD << "  LIVE CLOCK DASHBOARD\n" << RST;
                printDoubleSep(64);
                std::cout << "\n";

                printClockBox("Your Local Time", "Local System",
                              local, buildOffsetStr(localOffset),
                              "[PC]", true);

                if (!pinnedList.empty()) {
                    std::cout << "\n  " << FG_BBLUE << BOLD
                              << "---- Pinned Countries -----------------------------------"
                              << RST << "\n\n";
                    for (auto& pc : pinnedList) {
                        ClockTime wct = computeClockTime(utcNow, pc.info.utcOffset);
                        printClockBox(pc.info.name, pc.info.capital,
                                      wct, pc.info.timezone,
                                      pc.info.flag, false);
                        printTimeDiff(localOffset, pc.info.utcOffset, pc.info.name);
                        std::cout << "\n";
                    }
                }

                std::cout << "\n  " << FG_BBLACK
                          << "  Auto-refreshing... Press ENTER to return to menu."
                          << RST << "\n";

                std::this_thread::sleep_for(std::chrono::seconds(1));
                ticks++;
            }
            std::cin.ignore(10000, '\n');

        // ─ [2] Country Lookup ─────────────────────────────────────────────────
        } else if (choice == "2") {
            system(CLEAR);
            printBanner();
            std::cout << FG_BCYAN << BOLD
                      << "  ---- Country Time Lookup --------------------------------\n\n"
                      << RST;
            std::cout << FG_BWHITE
                      << "  Enter country name (e.g. Japan, India, USA, France): "
                      << RST;
            std::string input;
            std::getline(std::cin, input);
            std::string key = toLower(input);

            auto it = countryDB.find(key);
            if (it != countryDB.end()) {
                time_t utcNow   = getUtcTime();
                ClockTime local = computeClockTime(utcNow, localOffset);
                ClockTime wct   = computeClockTime(utcNow, it->second.utcOffset);

                std::cout << "\n";
                printClockBox("Your Local Time", "Local System",
                              local, buildOffsetStr(localOffset),
                              "[PC]", true);
                std::cout << "\n";
                printClockBox(it->second.name, it->second.capital,
                              wct, it->second.timezone,
                              it->second.flag, false);
                printTimeDiff(localOffset, it->second.utcOffset, it->second.name);
            } else {
                std::cout << "\n  " << FG_BRED
                          << "  [X] Country not found: \"" << input << "\"\n" << RST
                          << "  " << FG_BBLACK
                          << "  Try option [5] to see all supported countries.\n" << RST;
            }
            std::cout << "\n  " << FG_BBLACK << "[Press ENTER to continue]" << RST;
            std::cin.ignore(10000, '\n');
            system(CLEAR);
            printBanner();
            showMenu();

        // ─ [3] Pin a country ──────────────────────────────────────────────────
        } else if (choice == "3") {
            std::cout << "\n  " << FG_BWHITE << "Enter country to pin: " << RST;
            std::string input;
            std::getline(std::cin, input);
            std::string key = toLower(input);
            auto it = countryDB.find(key);
            if (it != countryDB.end()) {
                bool already = false;
                for (auto& pc : pinnedList)
                    if (pc.key == key) { already = true; break; }
                if (already) {
                    std::cout << "  " << FG_BYELLOW
                              << "  [!] " << it->second.name << " is already pinned.\n" << RST;
                } else if (pinnedList.size() >= 5) {
                    std::cout << "  " << FG_BRED
                              << "  [X] Max 5 pinned countries. Remove one first.\n" << RST;
                } else {
                    pinnedList.push_back({key, it->second});
                    std::cout << "  " << FG_BGREEN
                              << "  [+] Pinned: " << it->second.flag
                              << " " << it->second.name << "\n" << RST;
                }
            } else {
                std::cout << "  " << FG_BRED
                          << "  [X] Not found: \"" << input << "\"\n" << RST;
            }
            std::cout << "\n";

        // ─ [4] Remove pinned ──────────────────────────────────────────────────
        } else if (choice == "4") {
            if (pinnedList.empty()) {
                std::cout << "\n  " << FG_BYELLOW << "  No pinned countries yet.\n\n" << RST;
            } else {
                std::cout << "\n  " << FG_BWHITE << "Currently pinned:\n" << RST;
                for (int i = 0; i < (int)pinnedList.size(); i++) {
                    std::cout << "  " << FG_BCYAN << "  [" << (i+1) << "]" << RST
                              << " " << pinnedList[i].info.flag
                              << " " << pinnedList[i].info.name << "\n";
                }
                std::cout << "\n  " << FG_BWHITE
                          << "Enter number to remove (0 to cancel): " << RST;
                std::string numStr;
                std::getline(std::cin, numStr);
                try {
                    int idx = std::stoi(numStr);
                    if (idx >= 1 && idx <= (int)pinnedList.size()) {
                        std::cout << "  " << FG_BGREEN << "  [-] Removed: "
                                  << pinnedList[idx-1].info.name << "\n\n" << RST;
                        pinnedList.erase(pinnedList.begin() + (idx - 1));
                    } else if (idx != 0) {
                        std::cout << "  " << FG_BRED << "  [X] Invalid number.\n\n" << RST;
                    }
                } catch (...) {
                    std::cout << "  " << FG_BRED << "  [X] Invalid input.\n\n" << RST;
                }
            }

        // ─ [5] List countries ─────────────────────────────────────────────────
        } else if (choice == "5") {
            system(CLEAR);
            printBanner();
            std::cout << FG_BCYAN << BOLD
                      << "  ---- Supported Countries (" << countryDB.size()
                      << " entries) ----------------\n\n" << RST;
            int col = 0;
            for (auto& kv : countryDB) {
                std::cout << "  " << kv.second.flag << " "
                          << FG_BWHITE << std::left << std::setw(26) << kv.second.name << RST
                          << FG_BBLACK << std::setw(10) << kv.second.timezone << RST;
                col++;
                if (col % 2 == 0) std::cout << "\n";
            }
            if (col % 2 != 0) std::cout << "\n";
            std::cout << "\n  " << FG_BBLACK << "[Press ENTER to continue]" << RST;
            std::cin.ignore(10000, '\n');
            system(CLEAR);
            printBanner();
            showMenu();

        // ─ [0] Exit ───────────────────────────────────────────────────────────
        } else if (choice == "0" || choice == "exit" || choice == "quit") {
            system(CLEAR);
            printBanner();
            std::cout << FG_BCYAN << BOLD
                      << "  Thank you for using WorldPulse Clock!\n" << RST
                      << FG_BBLACK
                      << "  The world keeps ticking -- see you next time.\n\n" << RST;
            running = false;

        } else {
            std::cout << "  " << FG_BRED
                      << "  [X] Unknown option. Enter 0-5.\n\n" << RST;
        }
    }
    return 0;
}
