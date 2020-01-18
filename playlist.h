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
#include "player_mode.h"
#include "playable.h"

class File {
    const std::string file_description;
public:
    const std::string& getDescription() const {
        return this->file_description;
    }

    File(std::string description) :
            file_description(std::move(description))
    {}
};


class Piece : public Playable { };

class Song : public Piece {
    const std::string artist;
    const std::string title;
    const std::string contents;

public:
    void play() const override {
        std::cout << "Song [" << artist << ", " << title << "]: ";
        std::cout << contents << "\n";
    };

    Song(std::string artist, std::string title, std::string contents)
    : artist(std::move(artist))
    , title(std::move(title))
    , contents(std::move(contents))
    {}
};

class Movie : public Piece {
    const std::string title;
    const std::string year;
    const std::string contents;

public:
    void play() const override {
        std::cout << "Movie [" << title << ", " << year << "]: ";
        std::cout << contents << "\n";
    };

    Movie(std::string title, std::string year, std::string contents)
    : title(std::move(title))
    , year(std::move(year))
    , contents(std::move(contents))
    {}
};

class Playlist : public Playable {
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

    void setMode(const std::shared_ptr<PlayMode>& mode) {
        this->mode = mode;
    }

    void play() const override {
        std::cout << "Playlist [" << *name << "]" << std::endl;

        collection_t ordered_tracks = mode->orderTracks(*tracklist);

        for(auto element : ordered_tracks)
            element->play();
    }

    Playlist(const std::string& name) :
            tracklist(std::make_shared<collection_t>()),
            mode(createSequenceMode()),
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
    {}
};


/*TODO When you choose to manage the memory yourself you should follow the so-called
 * Rule of Three.
 * This rule states that if your class defines one or more of the following
 * methods it should probably explicitly define all three:
 * destructor;
 * copy constructor;
 * copy assignment operator.
 */

class Player {
private:

    bool begEqual(const std::string& str, const std::string& origin) {
        if (str.size() < origin.size())
            return false;

        for (size_t i = 0; i < origin.size(); i++) {
            if (str[i] != origin[i])
                return false;
        }
        return true;
    }

    bool findColon(const std::string& line) {
        for (char c : line) {
            if (c == ':')
                return true;
        }
        return false;
    }

    std::string decipher(const std::string& line) {

        char answer[ line.size() + 1];
        int help;

        for (size_t i = 0; i < line.size(); i++) {
            if (line[i] >= 'a' && line[i] <= 'z') {
                help = line[i] - 'a';
                help = (help + 13) % 26 + 'a';
                answer[i] = static_cast<char>(help);
            }
            else if (line[i] >= 'A' && line[i] <= 'Z') {
                help = line[i] - 'A';
                help = (help + 13) % 26 + 'A';
                answer[i] = static_cast<char>(help);
            }
            else {
                answer[i] = line[i];
            }
        }

        answer[line.size()] = '\0';
        return answer;
    }

    void censorship(const std::string& line) {

        char legalSpecial[8] = {',', '.', '!', '?', '\'', ':', ';', '-'};
        bool ok;

        for (char c : line) {

            if (isalnum(c) || isspace(c)) {
                continue;
            }
            else {
                ok = false;
                for (char s : legalSpecial) {
                    if (c == s)
                        ok = true;
                }
                if (!ok)
                    throw CorruptContentException();
            }
        }
    }

    std::shared_ptr<Piece> openSong(const std::vector <std::string>& data) {

        std::string artist, title, contents;
        bool found_artist = false, found_title = false;

        for (size_t i = 1; i < data.size(); i++) {
            if (begEqual(data[i], "artist:")) {
                artist = data[i].substr(7, data[i].size() - 7);
                found_artist = true;
            }
            else if (begEqual(data[i], "title:")) {
                title = data[i].substr(6, data[i].size() - 6);
                found_title = true;
            }
            else if (i == data.size() - 1) {
                censorship(data[i]);
                contents = data[i];
            }
            else {
                if (!findColon(data[i])) {
                    throw CorruptContentException();
                }
            }
        }

        if (found_artist && found_title) {
            return std::make_shared<Song>(artist, title, contents);
        }
        else {
            throw CorruptContentException();
        }
    }

    std::shared_ptr<Piece> openVideo(const std::vector <std::string>& data) {

        std::string title, year, contents;
        bool found_title = false, found_year = false;

        for (size_t i = 1; i < data.size(); i++) {
            if (begEqual(data[i], "title:")) {
                title = data[i].substr(6, data[i].size() - 6);
                found_title = true;
            }
            else if (begEqual(data[i], "year:")) {
                year = data[i].substr(5, data[i].size() - 5);
                found_year = true;
            }
            else if (i == data.size() - 1) {
                censorship(data[i]);
                contents = std::move(decipher(data[i]));

            }
            else {
                if (!findColon(data[i])) {
                    throw CorruptContentException();
                }
            }
        }

        if (found_title && found_year) {
            return std::make_shared<Movie>(title, year, contents);
        }
        else {
            throw CorruptContentException();
        }
    }

public:

    std::shared_ptr<Piece> openFile(const File& file) {

        std::string str = file.getDescription();
        std::vector <std::string> data;
        boost::algorithm::split(data, str, boost::is_any_of("|"));

        if(data.size() < 4) {
            throw CorruptFileException();
        }

        std::string type = data[0];
        if (type == "audio") {
            return openSong(data);
        }
        else if (type == "video") {
            return openVideo(data);
        }
        else {
            throw UnsupportedTypeException();
        }
    };

    std::shared_ptr<Playlist> createPlaylist(const std::string& name) const {
        return std::make_shared<Playlist>(name);
    }
};
#endif