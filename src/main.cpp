#include <string>
#include <iostream>
#include <filesystem>

int main(int argc, char** argv) {
    // Handle command-line arguments
    /*
        todo handle --directory argument
        todo handle --playlist argument
        todo handle --file argument (play single file only)
    */
    if (argc != 2) {
        std::cerr << "Please input path to the music as argument" << std::endl;
        exit(1);
    }

    std::string path = argv[1];
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::cout << entry.path() << std::endl;
    }

    return 0;
}