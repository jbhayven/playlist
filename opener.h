#ifndef PLAYLIST_OPENER_H
#define PLAYLIST_OPENER_H

#include <unordered_map>
#include "playable.h"

class Song : public Piece {
    const std::string artist;
    const std::string title;
    const std::string contents;

public:
    Song(std::unordered_map<std::string, std::string> metadata,
         std::string contents);

    void play() const noexcept override;
};

class Movie : public Piece {
    const std::string title;
    const std::string year;
    const std::string contents;

    std::string decipher(std::string line);

public:
    Movie(std::unordered_map<std::string, std::string> metadata,
          std::string contents);

    void play() const noexcept override;
};

class Opener {
public:
    virtual std::shared_ptr<Piece>
    open(std::unordered_map<std::string, std::string> metadata,
         std::string contents) = 0;
};

class SongOpener : public Opener {
public:
    std::shared_ptr<Piece>
    open(std::unordered_map<std::string, std::string> metadata,
         std::string contents) override;
};

class MovieOpener : public Opener {
private:
    void checkIsNumber(std::string line);

public:
    std::shared_ptr<Piece>
    open(std::unordered_map<std::string, std::string> metadata,
         std::string contents) override;
};

#endif
