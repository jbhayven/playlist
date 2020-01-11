#ifndef _PLAYLIST_H
#define _PLAYLIST_H

#include <list>
#include <string>
#include <cstddef>
#include <memory>
#include <unordered_set>
#include "player_exception.h"

class File {
    std::string description;
public:
    std::string getDescription() {
        return this->description;
    }
};

class Playable {
public:
    virtual void play() const;
};

class Piece : Playable { };

// Może zamiast std::string użylibyśmy std::shared_ptr<std::string> na potrzeby
// kopiowania, z naszymi własnymi konstruktorami kopiującymi, bo kopii pewnie będzie dużo,
// a Song i Movie nigdy nie zmieniają swoich właściwości?
// Może contents wyrzucilibyśmy do klasy Piece i tam wrzucili częściową
// implementację play()?
class Song : Piece {
    const std::string performer;
    const std::string title;

    const std::string contents;

public:
    void play() const override;
};

class Movie : Piece {
    const std::string title;
    const std::string year;

    const std::string contents;

public:
    void play() const override;
};
//

// Jednak koncepcja jest taka, że PlayMode nie powinien znać sposobu przechowywania
// danych przez playlistę – to jej odpowiedzialnością będzie przetłumaczenie go
// na std::list<Playable>, które jest typem znanym przez PlayMode.
// Może PlayMode wypadałoby wyrzucić do osobnego pliku nagłówkowego?
// Nie jestem też pewien sygnatur funkcji order_tracks; może powinny być statyczne?
// Ale bazowa nie może być jednocześnie statyczna i wirtualna...
class PlayMode {
public:
    virtual std::list<Playable> order_tracks(const std::list<Playable>& tracks) const;
};

class SequenceMode : public PlayMode {
public:
    std::list<Playable> order_tracks(
        const std::list<Playable>& tracks) const override {

        return tracks;
    }
};

class ShuffleMode : public PlayMode {
public:
    std::list<Playable> order_tracks(const std::list<Playable>& tracks) const override;
};

class OddEvenMode : public PlayMode {
public:
    std::list<Playable> order_tracks(const std::list<Playable>& tracks) const override;
};

PlayMode createSequenceMode() {
    return SequenceMode();
}

PlayMode createShuffleMode() {
    return ShuffleMode();
}

PlayMode createOddEvenMode() {
    return OddEvenMode();
}

class Playlist : Playable {
    // Tutaj nie jestem pewien, czy robić to tutaj przez using, czy może
    // przez jakąś zewnętrzną klasę.
    using tracklist_t = std::list<Playable>;

    // Szczerze mówiąc, nie jestem pewien, jak patrzeć na tutejszą sytuację
    // – bo czy kopia listy jest tą samą listą? Jak należy reagować na zmiany
    // w kopiach? Czy kopie współdzielą zmiany?

    // Jeśli tak, to shared_ptr jest najlepszą opcją, ale wtedy sprawdzanie
    // na obecność cykli trzeba by wykonywać przy każdym dodawaniu bo trudno byłoby
    // na bieżąco utrzymywać dla każdej playlisty zbiór wszystkich wyższych
    // leksykograficznie list.
    // No, albo to, albo jakieś dzikie algosy, których nie znam xD

    // Jeśli nie, to można pomyśleć o copy-on-write albo po prostu chamsko kopiować,
    // ale znowu – druga opcja jest czasochłonna. Z drugiej strony, możliwe byłoby
    // jako takie utrzymywanie struktury i w miarę szybkie sprawdzanie na obecność cykli.
    std::shared_ptr<tracklist_t> tracks;
    std::shared_ptr< std::unordered_set<Playlist> > child_playlists;
    PlayMode mode;

public:
    void add(Playable element);
    void add(Playable element, size_t position);
    void remove();
    void remove(size_t position);
    void setMode(PlayMode mode);
    void play() const override;

    Playlist() :
        tracks(std::make_shared<tracklist_t>()),
        child_playlists(std::make_shared< std::unordered_set<Playlist> >()),
        mode(createSequenceMode())
    {}

    Playlist(const Playlist& other) :
        tracks(other.tracks),
        child_playlists(other.child_playlists),
        mode(other.mode)
    {}

    Playlist(Playlist&& other) :
        tracks(other.tracks),
        child_playlists(other.child_playlists),
        mode(other.mode)
    {
        other.tracks = std::make_shared< tracklist_t >();
        other.child_playlists = std::make_shared< std::unordered_set<Playlist> >();
        other.mode = createSequenceMode();
    }
};

class Player {
    std::list<Playlist> playlists; // do czego nam to właściwie potrzebne?
        // Czy może chodzi o to, żeby playlisty nie znikały?
        // Ale skoro zwracamy całe obiekty, to i tak chyba nie mamy nad tym kontroli?
public:
    virtual Piece openFile(const File& file) const;
    virtual Playlist createPlaylist(const std::string& name) const;
};
#endif
