#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <limits>
#include <cstdlib>
#include <cctype>

constexpr const char* COLOR_RED     = "\x1b[31m";
constexpr const char* COLOR_GREEN   = "\x1b[32m";
constexpr const char* COLOR_YELLOW  = "\x1b[33m";
constexpr const char* COLOR_BLUE    = "\x1b[34m";
constexpr const char* COLOR_MAGENTA = "\x1b[35m";
constexpr const char* COLOR_CYAN    = "\x1b[36m";
constexpr const char* COLOR_RESET   = "\x1b[0m";

constexpr std::size_t NOTES_MAX    = 50;
constexpr std::size_t NOTE_LENGTH  = 500;

struct Note {
    int  day{}, month{}, year{};
    int  hour{}, minute{};
    std::string text;
};

std::vector<Note> notes;

void pressEnter() {
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void clearConsole() {
#ifdef _WIN32
    std::system("cls");
#endif
}

bool validateDate(int d, int m, int y) {
    if (y < 0 || y > 2030)             return false;
    if (m < 1 || m > 12)               return false;
    if (d < 1 || d > 31)               return false;

    if (m == 4 || m == 6 || m == 9 || m == 11)
        return d <= 30;

    if (m == 2) {
        bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
        return d <= 28 + static_cast<int>(leap);
    }
    return true;
}

void loadNotesFromFile() {
    std::ifstream file("notes.txt");
    if (!file) {
        std::cout << COLOR_YELLOW << "No existing notes found. Starting fresh...\n"
                  << COLOR_RESET;
        return;
    }
    notes.clear();

    while (true) {
        Note n;
        char sep;
        if (!(file >> n.day >> n.month >> n.year >> n.hour >> n.minute))
            break;
        file.get(sep);
        std::getline(file, n.text);
        notes.push_back(std::move(n));
        if (notes.size() == NOTES_MAX) break;
    }
}

void viewNotes() {
    clearConsole();
    if (notes.empty()) {
        std::cout << COLOR_YELLOW << "No notes available.\n" << COLOR_RESET;
    } else {
        std::cout << COLOR_MAGENTA << "All Notes:\n" << COLOR_RESET;
        for (const auto& n : notes) {
            std::cout << COLOR_BLUE << std::setfill('0')
                      << std::setw(2) << n.day   << '-'
                      << std::setw(2) << n.month << '-'
                      << std::setw(4) << n.year  << ' '
                      << std::setw(2) << n.hour  << ':'
                      << std::setw(2) << n.minute
                      << COLOR_RESET << " - " << n.text << "\n";
        }
    }
    pressEnter();
}

void viewNotesByDate(int d, int m, int y) {
    clearConsole();
    if (!validateDate(d, m, y)) {
        std::cout << COLOR_RED << "Invalid date!\n" << COLOR_RESET;
        pressEnter();
        return;
    }

    bool found = false;
    std::cout << COLOR_MAGENTA << "Notes for "
              << std::setfill('0') << std::setw(2) << d << '-'
              << std::setw(2) << m << '-' << std::setw(4) << y << ":\n"
              << COLOR_RESET;
    for (const auto& n : notes) {
        if (n.day == d && n.month == m && n.year == y) {
            std::cout << COLOR_BLUE << std::setw(2) << n.hour << ':'
                      << std::setw(2) << n.minute << COLOR_RESET
                      << " - " << n.text << "\n";
            found = true;
        }
    }
    if (!found)
        std::cout << COLOR_YELLOW << "No notes found for this date.\n"
                  << COLOR_RESET;
    pressEnter();
}

int main(int argc, char* argv[]) {
    loadNotesFromFile();

    if (argc < 2) {
        std::cout << COLOR_RED << "Usage:\n"
                  << "  ./note_viewer view\n"
                  << "  ./note_viewer view-by-date DD MM YYYY\n"
                  << COLOR_RESET;
        return 1;
    }

    std::string command = argv[1];

    if (command == "view") {
        if (argc != 2) {
            std::cout << COLOR_RED << "'view' does not take any arguments.\n" << COLOR_RESET;
            return 1;
        }
        viewNotes();
    } else if (command == "view-by-date") {
        if (argc != 5) {
            std::cout << COLOR_RED << "Please enter: DD MM YYYY\n" << COLOR_RESET;
            return 1;
        }

        int day = std::atoi(argv[2]);
        int month = std::atoi(argv[3]);
        int year = std::atoi(argv[4]);

        if (!validateDate(day, month, year)) {
            std::cout << COLOR_RED << "Invalid date format.\n" << COLOR_RESET;
            return 1;
        }

        viewNotesByDate(day, month, year);
    } else {
        std::cout << COLOR_RED << "Unknown command.\n" << COLOR_RESET;
        return 1;
    }

    return 0;
}