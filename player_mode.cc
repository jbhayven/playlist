#include "player_mode.h"

using collection_t = std::vector<std::shared_ptr<Playable>>;

collection_t SequenceMode::orderTracks(const collection_t &tracks) {
    return collection_t(tracks);
}

collection_t ShuffleMode::orderTracks(const collection_t &tracks) {
    collection_t result(tracks);

    std::shuffle(result.begin(), result.end(), engine);

    return result;
}

collection_t OddEvenMode::orderTracks(const collection_t &tracks) {
    collection_t result;

    for (size_t i = 1; i < tracks.size(); i += 2)
        result.push_back(tracks.at(i));

    for (size_t i = 0; i < tracks.size(); i += 2)
        result.push_back(tracks.at(i));

    return result;
}

std::shared_ptr<PlayMode> createSequenceMode() {
    return std::make_shared<SequenceMode>();
}

std::shared_ptr<PlayMode> createShuffleMode(unsigned seed) {
    return std::make_shared<ShuffleMode>(seed);
}

std::shared_ptr<PlayMode> createOddEvenMode() {
    return std::make_shared<OddEvenMode>();
}