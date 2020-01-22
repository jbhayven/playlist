#ifndef _PLAYLIST_H
#define _PLAYLIST_H

#include <iostream>
#include <boost/algorithm/string.hpp>
#include "player_exception.h"
#include "playable_exception.h"
#include "player_mode.h"
#include "playable.h"
#include "opener.h"

class File {
    std::string type;
    std::unordered_map<std::string, std::string> metadata;
    std::string contents;

    size_t findColon(const std::string &line) const noexcept;

    void censorship(const std::string &line) const;

public:
    explicit File(std::string description);

    const std::string &getType() const noexcept;

    const std::unordered_map<std::string, std::string> &
    getMetadata() const noexcept;

    const std::string &getContents() const noexcept;
};

class Playlist : public CompositePlayable {
    using playmode_ptr = std::shared_ptr<PlayMode>;

    playmode_ptr mode;
    const std::string name;

public:
    explicit Playlist(std::string name) :
            mode(createSequenceMode()),
            name(std::move(name)) {}

    void setMode(const playmode_ptr &mode) noexcept;

    void play() const override;
};

class Player {
private:
    std::unordered_map<std::string, std::shared_ptr<Opener>> openers;

public:
    Player();

    explicit Player(std::unordered_map<std::string,
            std::shared_ptr<Opener>> otherOpeners);

    std::shared_ptr<Piece> openFile(const File &file);

    std::shared_ptr<Playlist> createPlaylist(const std::string &name) const;
};

#endif