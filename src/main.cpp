#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <chrono>
using namespace std::chrono_literals;
#include <thread>
#include <csignal>

// Headers required for terminal management
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>

struct termios original_termios;
bool should_exit = false;
std::vector<std::string> track_list;
size_t selected_track_index = 0;
unsigned short width;
unsigned short height;

void die(const char* msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

void clearScreen() {
    std::cout << "\x1b[2J\x1b[H";
}

void get_window_size(unsigned short* width, unsigned short* height) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    *width = w.ws_col;
    *height = w.ws_row;
}

void interruptSignalHandler(int sig) {
    clearScreen();

    exit(sig);
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

void playTrack(size_t track_index) {
    // placeholder for playing a track
}

void keyboardListener() {
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == '\x1b') {
            char seq[3];

            if (read(STDIN_FILENO, &seq[0], 1) != 1) {
                continue;
            }

            if (read(STDIN_FILENO, &seq[1], 1) != 1) {
                continue;
            }

            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A':
                        if (selected_track_index > 0) {
                            selected_track_index--;
                        }
                        break;
                    case 'B':
                        if (selected_track_index < track_list.size() - 1) {
                            selected_track_index++;
                        }
                        break;
                    case 'C':
                        // Right
                        break;
                    case 'D':
                        // Left
                        break;
                }
            }
        } else {
            switch (c) {
                case '\n':
                    playTrack(selected_track_index);
                    break;
                case ' ':
                    // todo: play/pause
                    break;
                case 'q':
                    should_exit = true;
                    return;
            }
            // printf("%d ('%c')\n", c, c);
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        die("Please input path to the music as argument");
    }

    get_window_size(&width, &height);

    signal(SIGINT, interruptSignalHandler);

    enableRawMode();

    std::string path = argv[1];
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path, std::filesystem::directory_options::follow_directory_symlink | std::filesystem::directory_options::skip_permission_denied)) {
            // Check that it's not a directory and a regular file and has the mp3 file extension
            if (!entry.is_directory() && entry.is_regular_file() && entry.path().extension() == ".mp3") {
                track_list.push_back(entry.path().filename());
            }
        }
    } catch (std::filesystem::filesystem_error e) {
        die(e.what());
    } catch (...) {
        die("caught unknown exception");
    }

    clearScreen();

    // Hide cursor
    std::cout << "\x1b[?25l";

    std::thread input_thread(keyboardListener);

    std::string normal_color_style = "\x1b[38;2;255;255;255m";
    std::string selected_color_style = "\x1b[48;2;255;255;255m\x1b[38;2;0;0;0m";

    while (!should_exit) {
        // Start rendering
        clearScreen();

        for (size_t i = 0; i < track_list.size(); ++i) {
            std::string track = track_list[i];

            std::string* color_style = &normal_color_style;
            if (i == selected_track_index) {
                color_style = &selected_color_style;
            }

            std::cout << *color_style << track << "\x1b[0m\x1b[K" << std::endl;
        }
        std::this_thread::sleep_for(10ms);
    }

    input_thread.join();
    
    // Show cursor
    std::cout << "\x1b[?25h";

    clearScreen();

    return 0;
}