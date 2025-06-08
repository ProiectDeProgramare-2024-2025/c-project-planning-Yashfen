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

bool validateTime(int h, int min) {
    return h >= 0 && h < 24 && min >= 0 && min < 60;
}

void saveNotesToFile() {
    std::ofstream file("notes.txt");
    if (!file) {
        std::cout << COLOR_RED << "Error saving notes!\n" << COLOR_RESET;
        pressEnter();
        return;
    }
    for (const auto& n : notes) {
        file << std::setfill('0') << std::setw(2) << n.day    << ' '
             << std::setw(2) << n.month  << ' '
             << std::setw(4) << n.year   << ' '
             << std::setw(2) << n.hour   << ' '
             << std::setw(2) << n.minute << '|' << n.text << "\n";
    }
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

void addNote() {
    clearConsole();
    if (notes.size() >= NOTES_MAX) {
        std::cout << COLOR_RED << "Note storage full!\n" << COLOR_RESET;
        pressEnter();
        return;
    }

    Note n;
    std::cout << COLOR_CYAN << "Enter date and time (DD MM YYYY HH MM): " << COLOR_RESET;
    
    std::string line;
    std::getline(std::cin, line);
    std::istringstream iss(line);

    int d, m, y, h, min;
    std::string extra;
    if (!(iss >> d >> m >> y >> h >> min) || (iss >> extra)) {
        std::cout << COLOR_RED << "Invalid input format! Please enter exactly 5 numbers.\n" << COLOR_RESET;
        pressEnter();
        return;
    }

    if (!validateDate(d, m, y) || !validateTime(h, min)) {
        std::cout << COLOR_RED << "Invalid date or time.\n" << COLOR_RESET;
        pressEnter();
        return;
    }

    n.day = d;
    n.month = m;
    n.year = y;
    n.hour = h;
    n.minute = min;

    std::cout << COLOR_CYAN << "Enter note: " << COLOR_RESET;
    std::getline(std::cin, n.text);
    if (n.text.size() > NOTE_LENGTH - 1) n.text.resize(NOTE_LENGTH - 1);

    notes.push_back(std::move(n));
    saveNotesToFile();

    std::cout << COLOR_GREEN << "Note added successfully!\n" << COLOR_RESET;
    pressEnter();
}

void editNote(int d, int m, int y) {
    clearConsole();
    if (!validateDate(d, m, y)) {
        std::cout << COLOR_RED << "Invalid date!\n" << COLOR_RESET;
        pressEnter();
        return;
    }

    for (auto& n : notes) {
        if (n.day == d && n.month == m && n.year == y) {
            std::cout << COLOR_MAGENTA << "Old Note: " << COLOR_RESET << n.text <<std::endl;
            std::cout << COLOR_CYAN << "Enter new note: " << COLOR_RESET;
            std::getline(std::cin, n.text);
            if (n.text.size() > NOTE_LENGTH - 1) n.text.resize(NOTE_LENGTH - 1);
            saveNotesToFile();
            std::cout << COLOR_GREEN << "Note updated!\n" << COLOR_RESET;
            pressEnter();
            return;
        }
    }

    std::cout << COLOR_YELLOW << "No note found for given date.\n" << COLOR_RESET;
    pressEnter();
}

void deleteNote(int d, int m, int y) {
    clearConsole();
    if (!validateDate(d, m, y)) {
        std::cout << COLOR_RED << "Invalid date!\n" << COLOR_RESET;
        pressEnter();
        return;
    }

    for (size_t i = 0; i < notes.size(); ++i) {
        if (notes[i].day == d && notes[i].month == m && notes[i].year == y) {
            notes.erase(notes.begin() + i);
            saveNotesToFile();
            std::cout << COLOR_GREEN << "Note deleted.\n" << COLOR_RESET;
            pressEnter();
            return;
        }
    }

    std::cout << COLOR_YELLOW << "Note not found.\n" << COLOR_RESET;
    pressEnter();
}

int main(int argc, char* argv[]) {
    loadNotesFromFile();

    if (argc < 2) {
        std::cout << COLOR_RED << "Usage:\n"
                  << "  ./notes_app add\n"
                  << "  ./notes_app edit DD MM YYYY\n"
                  << "  ./notes_app delete DD MM YYYY\n"
                  << COLOR_RESET;
        return 1;
    }

    std::string command = argv[1];

    if (command == "add") {
        if (argc != 2) {
            std::cout << COLOR_RED << "Usage: ./notes_app add\n" << COLOR_RESET;
            return 1;
        }
        addNote();

    } else if (command == "edit") {
        if (argc != 5) {
            std::cout << COLOR_RED << "Usage: ./notes_app edit DD MM YYYY\n" << COLOR_RESET;
            return 1;
        }
        int d = std::atoi(argv[2]);
        int m = std::atoi(argv[3]);
        int y = std::atoi(argv[4]);
        editNote(d, m, y);

    } else if (command == "delete") {
        if (argc != 5) {
            std::cout << COLOR_RED << "Usage: ./notes_app delete DD MM YYYY\n" << COLOR_RESET;
            return 1;
        }
        int d = std::atoi(argv[2]);
        int m = std::atoi(argv[3]);
        int y = std::atoi(argv[4]);
        deleteNote(d, m, y);

    } else {
        std::cout << COLOR_RED << "Unknown command.\n" << COLOR_RESET;
        return 1;
    }

    return 0;
}
