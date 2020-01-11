#ifndef _PLAYLIST_H
#define _PLAYLIST_H

#include <list>
#include <string>
#include <cstddef>
#include <memory>
#include <unordered_set>
#include "player_exception.h"
#include "player_mode.h"

class File {
    const std::string file_description;
public:
    const std::string& getDescription() const {
        return this->file_description;
    }

    File(const std::string& description) :
            file_description(description)
    {}
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

class playlist : Playable {
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
    std::shared_ptr< std::unordered_set<playlist> > child_playlists;
    PlayMode mode;

public:
    void add(Playable element);
    void add(Playable element, size_t position);
    void remove();
    void remove(size_t position);
    void setMode(PlayMode mode);
    void play() const override;

    playlist() :
            tracks(std::make_shared<tracklist_t>()),
            child_playlists(std::make_shared< std::unordered_set<playlist> >()),
            mode(createSequenceMode())
    {}

    playlist(const playlist& other) :
            tracks(other.tracks),
            child_playlists(other.child_playlists),
            mode(other.mode)
    {}

    playlist(playlist&& other) :
            tracks(other.tracks),
            child_playlists(other.child_playlists),
            mode(other.mode)
    {
        other.tracks = std::make_shared< tracklist_t >();
        other.child_playlists = std::make_shared< std::unordered_set<playlist> >();
        other.mode = createSequenceMode();
    }
};

class Player {
    std::list<playlist> playlists; // do czego nam to właściwie potrzebne?
    // Czy może chodzi o to, żeby playlisty nie znikały?
    // Ale skoro zwracamy całe obiekty, to i tak chyba nie mamy nad tym kontroli?
public:
    virtual Piece openFile(const File& file) const;
    virtual playlist createPlaylist(const std::string& name) const;
};
#endif