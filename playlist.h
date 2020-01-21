#ifndef _PLAYLIST_H
#define _PLAYLIST_H

#include <iostream>
#include <boost/algorithm/string.hpp>
#include "player_exception.h"
#include "playable_exception.h"
#include "player_mode.h"
#include "playable.h"

class File {
    std::string type;
    std::unordered_map<std::string, std::string> metadata;
    std::string contents;

    size_t findColon(const std::string &line) const;

    void censorship(const std::string &line) const;

public:
    explicit File(std::string description);

    const std::string &getType() const;

    const std::unordered_map<std::string, std::string> &getMetadata() const;

    const std::string &getContents() const;
};

class Song : public Piece {
    const std::string artist;
    const std::string title;
    const std::string contents;

public:
    Song(std::unordered_map<std::string, std::string> metadata,
         std::string contents)
            : artist(metadata["artist"]), title(metadata["title"]),
              contents(std::move(contents)) {}

    void play() const override;
};

class Movie : public Piece {
    const std::string title;
    const std::string year;
    const std::string contents;

    std::string decipher(std::string line);

public:
    Movie(std::unordered_map<std::string, std::string> metadata,
          std::string contents)
            : title(metadata["title"]), year(metadata["year"]),
              contents(decipher(std::move(contents))) {}

    void play() const override;
};

class Playlist : public CompositePlayable {
    using playmode_ptr = std::shared_ptr<PlayMode>;

    playmode_ptr mode;
    std::string name;

public:
    explicit Playlist(std::string name) :
            mode(createSequenceMode()),
            name(std::move(name)) {}

    void setMode(const playmode_ptr &mode);

    void play() const override;
};

class Player {
private:
    typedef std::shared_ptr<Piece> (*pfunc)(
            std::unordered_map<std::string, std::string>, std::string);

    std::unordered_map<std::string, pfunc> openers;

public:
    Player();

    std::shared_ptr<Piece> openFile(const File &file);

    std::shared_ptr<Playlist> createPlaylist(const std::string &name) const;
};

#endif
