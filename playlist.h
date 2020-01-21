#ifndef _PLAYLIST_H
#define _PLAYLIST_H

#include <iostream>
#include <vector>
#include <string>
#include <cstddef>
#include <memory>
#include <unordered_set>
#include <utility>
#include <assert.h>
#include <boost/algorithm/string.hpp>
#include "player_exception.h"
#include "playable_exception.h"
#include "player_mode.h"
#include "playable.h"

class File {
    std::string type;
    std::unordered_map<std::string, std::string> metadata;
    std::string contents;

    size_t findColon(const std::string &line);

    void censorship(const std::string &line);

public:
    File(std::string description);

    const std::string &getType() const;

    const std::unordered_map<std::string, std::string> &getMetadata() const;

    const std::string &getContents() const;
};


class Piece : public Playable {};

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
              contents(std::move(decipher(std::move(contents)))) {}

    void play() const override;
};

class CompositePlayable : public Playable {
protected:
    using playable_ptr = std::shared_ptr<Playable>;
    using composite_ptr = std::shared_ptr<CompositePlayable>;
    using piece_ptr = std::shared_ptr<Piece>;

    std::vector<playable_ptr> child_components;
    std::vector<CompositePlayable *> child_composites;

    size_t size();

    bool reachable(CompositePlayable *looked_up);

public:
    virtual void add(piece_ptr elem, size_t position);

    virtual void add(piece_ptr elem);

    virtual void add(composite_ptr elem, size_t position);

    virtual void add(composite_ptr elem);

    virtual void remove(size_t position);

    virtual void remove();
};

class Playlist : public CompositePlayable {
    using playmode_ptr = std::shared_ptr<PlayMode>;

    playmode_ptr mode;
    std::string name;

public:
    Playlist(std::string name) :
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