#include <iostream>
#include "opener.h"

Song::Song(std::unordered_map<std::string, std::string> metadata,
           std::string contents)
        : artist(metadata["artist"])
        , title(metadata["title"])
        , contents(std::move(contents))
        {}

void Song::play() const noexcept {
    std::cout << "Song [" << artist << ", " << title << "]: ";
    std::cout << contents << "\n";
};

std::string Movie::decipher(std::string line) const {
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

Movie::Movie(std::unordered_map<std::string, std::string> metadata,
             std::string contents)
        : title(metadata["title"])
        , year(metadata["year"])
        , contents(decipher(std::move(contents)))
        {}

void Movie::play() const noexcept {
    std::cout << "Movie [" << title << ", " << year << "]: ";
    std::cout << contents << "\n";
};

std::shared_ptr<Piece> SongOpener::open (
        std::unordered_map<std::string, std::string> metadata,
        std::string contents) const
{
    std::unordered_map<std::string, std::string>::const_iterator it;

    if (metadata.find("artist") == metadata.end())
        throw CorruptFileException();
    if (metadata.find("title") == metadata.end())
        throw CorruptFileException();

    return std::make_shared<Song>(
            Song(std::move(metadata), std::move(contents)));
}

void MovieOpener::checkIsNumber(std::string line) const {
    for (char c : line) {
        if (c < '0' || c > '9')
            throw CorruptContentException();
    }
}

std::shared_ptr<Piece> MovieOpener::open(
        std::unordered_map<std::string, std::string> metadata,
        std::string contents) const
{
    std::unordered_map<std::string, std::string>::const_iterator it;

    if (metadata.find("title") == metadata.end())
        throw CorruptFileException();
    if (metadata.find("year") == metadata.end())
        throw CorruptFileException();
    checkIsNumber(metadata["year"]);

    return std::make_shared<Movie>(
            Movie(std::move(metadata), std::move(contents)));
}
