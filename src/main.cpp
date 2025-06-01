#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <chrono>
using namespace std::chrono_literals;
#include <thread>
#include <csignal>
#include <cstdlib>
#include <ctime>

// Headers required for terminal management
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>

#include "audioengine.hpp"

struct termios original_termios;
bool should_exit = false;
std::vector<std::filesystem::path> track_list;
size_t selected_track_index = 0;
size_t playing_track_index = 65535;
unsigned short width;
unsigned short height;

AudioEngine audioEngine;

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
    if (track_index <= track_list.size() - 1) {
        std::filesystem::path track = track_list[track_index];
        audioEngine.playTrack(std::filesystem::absolute(track).c_str(), false);

        playing_track_index = track_index;
        selected_track_index = track_index;
    }
}

void playNextTrack() {
    size_t next_track = playing_track_index;
    // todo implement shuffle/repeat
    // Pick random song
    // -2 to remove the current active track from the pool
    size_t track_list_len = track_list.size();
    next_track = (size_t)std::rand() % (track_list_len - 2);
    // If we get the same track again, increase by 1 to get the next one in line
    if (next_track == playing_track_index) {
        next_track++;
    }

    playTrack(next_track);
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
                        // todo decide between seek and skip
                        break;
                    case 'D':
                        // Left
                        // todo decide between seek and skip
                        break;
                }
            }
        } else {
            switch (c) {
                case '\n':
                    if (selected_track_index != playing_track_index) {
                        playTrack(selected_track_index);
                    }
                    break;
                case ' ':
                    audioEngine.playOrPause();
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

    // use current time as seed
    std::srand(std::time({}));

    get_window_size(&width, &height);

    signal(SIGINT, interruptSignalHandler);

    enableRawMode();

    std::string path = argv[1];
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path, std::filesystem::directory_options::follow_directory_symlink | std::filesystem::directory_options::skip_permission_denied)) {
            // Check that it's not a directory and a regular file and has the mp3 file extension
            if (!entry.is_directory() && entry.is_regular_file() && entry.path().extension() == ".mp3") {
                track_list.push_back(entry.path());
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
        // Get updates from audio engine
        if (audioEngine.isTrackFinished()) {
            playNextTrack();
        }

        // Start rendering
        clearScreen();

        for (size_t i = 0; i < track_list.size(); ++i) {
            std::filesystem::path track = track_list[i];

            std::string* color_style = &normal_color_style;
            if (i == selected_track_index) {
                color_style = &selected_color_style;
            }

            std::cout << *color_style << track.filename() << "\x1b[0m\x1b[K" << std::endl;
        }
        std::this_thread::sleep_for(100ms);
    }

    input_thread.join();
    
    // Show cursor
    std::cout << "\x1b[?25h";

    clearScreen();

    return 0;
}