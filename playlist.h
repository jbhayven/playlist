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
#include "playable_exception.h"
#include "player_mode.h"
#include "playable.h"

class File {
    std::string type;
    std::unordered_map<std::string, std::string> metadata;
    std::string contents;

    size_t findColon(const std::string &line) {
        for (size_t i = 0; i < line.size(); i++) {
            if (line[i] == ':')
                return i;
        }
        return line.size();
    }

    void censorship(const std::string &line) {
        char legalSpecial[8] = {',', '.', '!', '?', '\'', ':', ';', '-'};
        bool ok;

        for (char c : line) {

            if (isalnum(c) || isspace(c)) {
                continue;
            } else {
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


public:

    File(std::string description) {

        std::vector<std::string> data;
        boost::algorithm::split(data, description, boost::is_any_of("|"));

        if (data.size() < 2) { //it has to contain at least type and contents
            throw CorruptFileException();
        }

        type = data[0];
        contents = data[data.size() - 1];
        censorship(contents);
        std::string m_name, m_value;

        for (size_t i = 1; i < data.size() - 1; i++) {
            size_t colon = findColon(data[i]);

            if (colon == data[i].size())
                throw CorruptFileException();

            m_name = data[i].substr(0, colon);
            m_value = data[i].substr(colon + 1, data[i].size() - colon - 1);

            metadata[m_name] = m_value;
        }
    }

    const std::string &getType() const {
        return this->type;
    }

    const std::unordered_map<std::string, std::string> &getMetadata() const {
        return this->metadata;
    }

    const std::string &getContents() const {
        return this->contents;
    }
};


class Piece : public Playable {};

class Song : public Piece {
    const std::string artist;
    const std::string title;
    const std::string contents;

public:
    void play() const override {
        std::cout << "Song [" << artist << ", " << title << "]: ";
        std::cout << contents << "\n";
    };

    Song(std::unordered_map<std::string, std::string> metadata,
         std::string contents)
            : artist(metadata["artist"]), title(metadata["title"]),
              contents(std::move(contents)) {}
};

class Movie : public Piece {
    const std::string title;
    const std::string year;
    const std::string contents;

    std::string decipher(std::string line) {

        char answer[line.size() + 1];
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

public:
    void play() const override {
        std::cout << "Movie [" << title << ", " << year << "]: ";
        std::cout << contents << "\n";
    };

    Movie(std::unordered_map<std::string, std::string> metadata,
          std::string contents)
            : title(metadata["title"]), year(metadata["year"]),
              contents(std::move(decipher(std::move(contents)))) {}
};

class CompositePlayable : public Playable {
protected:
    using playable_ptr = std::shared_ptr<Playable>;
    using composite_ptr = std::shared_ptr<CompositePlayable>;
    using piece_ptr = std::shared_ptr<Piece>;

    std::vector<playable_ptr> child_components;
    std::vector<CompositePlayable *> child_composites;

    size_t size() {
        return child_components.size();
    }

    bool reachable(CompositePlayable *looked_up) {
        if (this == looked_up) return true;

        for (auto elem : child_composites)
            if (elem->reachable(looked_up))
                return true;

        return false;
    }

public:
    virtual void add(piece_ptr elem, size_t position) {
        if (position > size()) throw OutOfBoundsException();

        child_components.insert(child_components.begin() + position, elem);
    }

    virtual void add(piece_ptr elem) {
        child_components.push_back(elem);
    }

    virtual void add(composite_ptr elem, size_t position) {
        if (elem->reachable(this)) throw LoopingException();
        if (position > size()) throw OutOfBoundsException();

        child_components.insert(child_components.begin() + position, elem);
        child_composites.push_back(elem.get());
    }

    virtual void add(composite_ptr elem) {
        if (elem->reachable(this)) throw LoopingException();

        child_components.push_back(elem);
        child_composites.push_back(elem.get());
    }

    virtual void remove(size_t position) {
        if (position >= size()) throw OutOfBoundsException();

        auto it = find(child_composites.begin(), child_composites.end(),
                       child_components[position].get());

        child_components.erase(child_components.begin() + position);
        if (it != child_composites.end()) child_composites.erase(it);
    }

    virtual void remove() {
        if (size() == 0)
            throw OutOfBoundsException();

        remove(size() - 1);
    }
};

class Playlist : public CompositePlayable {
    using playmode_ptr = std::shared_ptr<PlayMode>;

    playmode_ptr mode;
    std::string name;

public:
    void setMode(const playmode_ptr &mode) {
        this->mode = mode;
    }

    void play() const override {
        std::cout << "Playlist [" << name << "]" << std::endl;

        std::vector<playable_ptr> ordered_tracks =
                mode->orderTracks(child_components);

        for (auto element : ordered_tracks)
            element->play();
    }

    Playlist(std::string name) :
            mode(createSequenceMode()),
            name(std::move(name)) {}
};

class Player {
private:
    typedef std::shared_ptr<Piece> (*pfunc)(
            std::unordered_map<std::string, std::string>, std::string);

    std::unordered_map<std::string, pfunc> openers;

    static std::shared_ptr<Piece>
    openSong(std::unordered_map<std::string, std::string> metadata,
             std::string contents) {
        return std::make_shared<Song>(
                Song(std::move(metadata), std::move(contents)));
    }

    static std::shared_ptr<Piece>
    openMovie(std::unordered_map<std::string, std::string> metadata,
              std::string contents) {
        return std::make_shared<Movie>(
                Movie(std::move(metadata), std::move(contents)));
    }

public:
    Player() {
        openers["audio"] = openSong;
        openers["video"] = openMovie;
    }

    std::shared_ptr<Piece> openFile(const File &file) {
        auto it = openers.find(file.getType());
        if (it == openers.end())
            throw UnsupportedTypeException();

        return openers[file.getType()](file.getMetadata(), file.getContents());
    }

    std::shared_ptr<Playlist> createPlaylist(const std::string &name) const {
        return std::make_shared<Playlist>(name);
    }
};

#endif