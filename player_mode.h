#ifndef PLAYLIST_PLAYERMODE_H
#define PLAYLIST_PLAYERMODE_H

#include <algorithm>
#include <random>
#include <vector>
#include <memory>
#include "playable.h"

class PlayMode {
protected:
    using collection_t = std::vector<std::shared_ptr<Playable>>;

public:
    virtual collection_t orderTracks(const collection_t &tracks) = 0;

    virtual ~PlayMode() = default;
};

class SequenceMode : public PlayMode {
public:
    collection_t orderTracks(const collection_t &tracks) override;
};

class ShuffleMode : public PlayMode {
    std::default_random_engine engine;

public:
    collection_t orderTracks(const collection_t &tracks) override;

    ShuffleMode(unsigned seed)
        : engine(seed)
    {}
};

class OddEvenMode : public PlayMode {
public:
    collection_t orderTracks(const collection_t &tracks) override;
};


std::shared_ptr<PlayMode> createSequenceMode();

std::shared_ptr<PlayMode> createShuffleMode(unsigned seed);

std::shared_ptr<PlayMode> createOddEvenMode();

#endif