#ifndef _PLAYLIST_H
#define _PLAYLIST_H

#include <iostream>
#include <vector>
#include <string>
#include <cstddef>
#include <memory>
#include <unordered_set>
#include <utility>
#include "player_exception.h"
#include "player_mode.h"
#include "playable.h"

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

class Playlist : Playable {
    using elem_t = std::shared_ptr<Playable>;
    using playmode_t = PlayMode;
    using collection_t = std::vector<elem_t>;

    std::shared_ptr<collection_t> tracklist;
    std::shared_ptr<playmode_t> mode;
    std::shared_ptr<std::string> name;

    size_t size() {
        return tracklist->size();
    }

    bool reachable(Playlist* p, Playlist* checked) {
        if(p == checked) return true;

        for(auto elem : *p->tracklist)
            if(reachable(elem.get(), checked))
                return true;

        return false;
    }

    bool reachable([[maybe_unused]] Playable* p, [[maybe_unused]] Playlist* checked) {
        return false;
    }

public:
    void add(elem_t element) {
        if(reachable(element.get(), this)) throw LoopingPlaylistsException();

        tracklist->push_back(element);
    }

    void add(elem_t element, size_t position) {
        if(position > size()) throw OutOfBoundsException();
        if(reachable(element.get(), this)) throw LoopingPlaylistsException();

        tracklist->insert(tracklist->begin() + position, element);
    }

    void remove() {
        tracklist->pop_back();
    }

    void remove(size_t position) {
        if(position >= size()) throw OutOfBoundsException();

        tracklist->erase(tracklist->begin() + position);
    }

    void setMode(PlayMode mode) {
        *this->mode = mode;
    }

    void play() const override {
        std::cout << "Playlist [" << name << "]" << std::endl;

        collection_t ordered_tracks = mode->orderTracks(*tracklist);

        for(auto element : ordered_tracks)
            element->play();
    }

    Playlist(const std::string& name) :
        tracklist(std::make_shared<collection_t>()),
        mode(std::make_shared<playmode_t>(createSequenceMode())),
        name(std::make_shared<std::string>(name))
    {}

    Playlist(const Playlist& other) :
        tracklist(other.tracklist),
        mode(other.mode),
        name(other.name)
    {}

    Playlist(Playlist&& other) :
        tracklist(std::move(other.tracklist)),
        mode(std::move(other.mode)),
        name(std::move(other.name))
    {
        other.tracklist = std::make_shared<collection_t>();
        other.mode = std::make_shared<playmode_t>(createSequenceMode());
        other.name = std::make_shared<std::string>();
    }
};

class Player {
public:
    virtual std::shared_ptr<Piece> openFile(const File& file) const;

    std::shared_ptr<Playlist> createPlaylist(const std::string& name) const {
        return std::make_shared<Playlist>(name);
    }
};
#endif
