#ifndef PLAYLIST_PLAYERMODE_H
#define PLAYLIST_PLAYERMODE_H

#include <algorithm>
#include <random>
#include <vector>
#include <memory>
#include "playable.h"

class PlayMode {
protected:
    using collection_t = std::vector< std::shared_ptr<Playable> >;
public:
    virtual collection_t orderTracks(const collection_t& tracks) const = 0;
    virtual ~PlayMode() = default;
};

class SequenceMode : public PlayMode {
public:
    collection_t orderTracks(const collection_t& tracks) const override {
        return collection_t(tracks);
    }
};

class ShuffleMode : public PlayMode {
    std::default_random_engine random_engine;

public:
    collection_t orderTracks(const collection_t& tracks) const override {
        collection_t result(tracks);

        std::shuffle(result.begin(), result.end(), random_engine);

        return result;
    }

    ShuffleMode(unsigned seed) :
            random_engine(seed)
    {}
};

class OddEvenMode : public PlayMode {
public:
    collection_t orderTracks(const collection_t& tracks) const override {
        collection_t result;

        for(size_t i = 1; i < tracks.size(); i+=2)
            result.push_back(tracks.at(i));

        for(size_t i = 0; i < tracks.size(); i+=2)
            result.push_back(tracks.at(i));

        return result;
    }
};


std::shared_ptr<PlayMode> createSequenceMode() {
    return std::make_shared<SequenceMode>();
}

std::shared_ptr<PlayMode> createShuffleMode(unsigned seed) {
    return std::make_shared<ShuffleMode>(seed);
}

std::shared_ptr<PlayMode> createOddEvenMode() {
    return std::make_shared<OddEvenMode>();
}

#endif
