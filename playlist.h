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

/*
 * Być może warto część interfejsu przenieść do nadrzędnej klasy "Composite"
 */
class Playlist : public Playable {
    using playable_t = std::shared_ptr<Playable>;
    using playlist_t = std::shared_ptr<Playlist>;
    using playmode_t = std::shared_ptr<PlayMode>;

    std::vector<playable_t> tracklist;
    std::vector<Playlist*> child_playlists;
    playmode_t mode;
    std::string name;

    size_t size() {
        return tracklist.size();
    }

    bool reachable(Playlist* looked_up) {
        std::cout << "Lookup: " << this->name << " " << looked_up->name << std::endl;
        if(this == looked_up) return true;

        for(auto elem : child_playlists)
            if(elem->reachable(looked_up))
                return true;

        return false;
    }

public:
    void add(playable_t playable, size_t position) {
        if(position > size()) throw OutOfBoundsException();

        tracklist.insert(tracklist.begin() + position, playable);
    }

    void add(playable_t playable) {
        add(playable, size());
    }

    void add(playlist_t playlist, size_t position) {
        if(playlist->reachable(this)) throw LoopingPlaylistsException();

        add(playable_t(playlist), position);
        child_playlists.push_back(playlist.get());
    }

    void add(playlist_t playlist) {
        add(playlist, size());
    }

    void remove() {
        if(size() == 0) throw OutOfBoundsException();

        remove(size() - 1);
    }

    void remove(size_t position) {
        if(position >= size()) throw OutOfBoundsException();

        auto it = find(child_playlists.begin(), child_playlists.end(),
                       tracklist[position].get());

        tracklist.erase(tracklist.begin() + position);
        if(it != child_playlists.end()) child_playlists.erase(it);
    }

    void setMode(const playmode_t& mode) {
        this->mode = mode;
    }

    void play() const override {
        std::cout << "Playlist [" << name << "]" << std::endl;

        std::vector<playable_t> ordered_tracks = mode->orderTracks(tracklist);

        for(auto element : ordered_tracks)
            element->play();
    }

    Playlist(const std::string& name) :
        tracklist(),
        child_playlists(),
        mode(createSequenceMode()),
        name(name)
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
