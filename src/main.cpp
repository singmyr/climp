#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <chrono>
using namespace std::chrono_literals;
#include <thread>

// Headers required for terminal management
// #include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>

struct termios original_termios;

void die(const char* msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(disableRawMode);

    struct termios raw = original_termios;

    tcgetattr(STDIN_FILENO, &raw);

    // ECHO = turn offechoing keypresses
    // ICANON = read byte-by-byte instead of line-by-line
    raw.c_lflag &= ~(ECHO | ICANON);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(int argc, char** argv) {
    // Handle command-line arguments
    /*
        todo handle --directory argument
        todo handle --playlist argument
        todo handle --file argument (play single file only)
    */
    if (argc != 2) {
        die("Please input path to the music as argument");
    }

    enableRawMode();

    std::vector<std::string> track_list;

    std::string path = argv[1];
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path, std::filesystem::directory_options::follow_directory_symlink | std::filesystem::directory_options::skip_permission_denied)) {
            track_list.push_back(entry.path().string());
        }
    } catch (std::filesystem::filesystem_error e) {
        die(e.what());
    } catch (...) {
        die("caught unknown exception");
    }

    // Hide cursor
    std::cout << "\x1b[?25l";
    std::cout << "\x1b[2J\x1b[H";

    // Start rendering
    std::cout << "\x1b[H";
    for (const auto& track : track_list) {
        std::cout << track << "\x1b[K" << std::endl;
    }
    std::this_thread::sleep_for(1000ms);
    
    // Show cursor
    std::cout << "\x1b[?25h";
    std::cout << "\x1b[H";

    return 0;
}