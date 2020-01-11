#ifndef PLAYLIST_PLAYERMODE_H
#define PLAYLIST_PLAYERMODE_H

#include <list>

class Playable {
public:
    virtual void play() const;
};

// Jednak koncepcja jest taka, że PlayMode nie powinien znać sposobu przechowywania
// danych przez playlistę – to jej odpowiedzialnością będzie przetłumaczenie go
// na std::list<Playable>, które jest typem znanym przez PlayMode.
// Może PlayMode wypadałoby wyrzucić do osobnego pliku nagłówkowego?
// Nie jestem też pewien sygnatur funkcji order_tracks; może powinny być statyczne?
// Ale bazowa nie może być jednocześnie statyczna i wirtualna...
class PlayMode {
public:
    virtual std::list<Playable> orderTracks(const std::list<Playable>& tracks) const;
};

class SequenceMode : public PlayMode {
public:
    std::list<Playable> orderTracks(
            const std::list<Playable>& tracks) const override {

        return tracks;
    }
};

class ShuffleMode : public PlayMode {
    int shuffle_seed;

public:
    std::list<Playable> orderTracks(const std::list<Playable>& tracks) const override;

    ShuffleMode(int seed) :
            shuffle_seed(seed)
    {}
};

class OddEvenMode : public PlayMode {
public:
    std::list<Playable> orderTracks(const std::list<Playable>& tracks) const override;
};

PlayMode createSequenceMode() {
    return SequenceMode();
}

PlayMode createShuffleMode(int seed) {
    return ShuffleMode(seed);
}

PlayMode createOddEvenMode() {
    return OddEvenMode();
}


#endif
