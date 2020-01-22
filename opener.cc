#include <iostream>
#include "opener.h"

void Song::play() const noexcept {
    std::cout << "Song [" << artist << ", " << title << "]: ";
    std::cout << contents << "\n";
};

std::string Movie::decipher(std::string line) {

    char answer[line.size() + 1];
    int help;

    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] >= 'a' && line[i] <= 'z') {
            help = line[i] - 'a';
            help = (help + 13) % 26 + 'a';
            answer[i] = static_cast<char>(help);
        } else if (line[i] >= 'A' && line[i] <= 'Z') {
            help = line[i] - 'A';
            help = (help + 13) % 26 + 'A';
            answer[i] = static_cast<char>(help);
        } else {
            answer[i] = line[i];
        }
    }

    answer[line.size()] = '\0';
    return answer;
}

void Movie::play() const noexcept {
    std::cout << "Movie [" << title << ", " << year << "]: ";
    std::cout << contents << "\n";
};

std::shared_ptr<Piece>
SongOpener::open(std::unordered_map<std::string, std::string> metadata,
                 std::string contents) {
    return std::make_shared<Song>(
            Song(std::move(metadata), std::move(contents)));
}

std::shared_ptr<Piece>
MovieOpener::open(std::unordered_map<std::string, std::string> metadata,
                  std::string contents) {
    return std::make_shared<Movie>(
            Movie(std::move(metadata), std::move(contents)));
}
