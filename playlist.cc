#include "playlist.h"

size_t File::findColon(const std::string &line) const noexcept {
    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] == ':')
            return i;
    }
    return line.size();
}

void File::censorship(const std::string &line) const {
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

File::File(std::string description) {

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

const std::string &File::getType() const noexcept {
    return this->type;
}

const std::unordered_map<std::string, std::string> &
File::getMetadata() const noexcept {
    return this->metadata;
}

const std::string &File::getContents() const noexcept {
    return this->contents;
}

void Playlist::setMode(const playmode_ptr &mode) noexcept {
    this->mode = mode;
}

void Playlist::play() const {
    std::cout << "Playlist [" << name << "]" << std::endl;

    std::vector<playable_ptr> ordered_tracks =
            mode->orderTracks(child_components);

    for (auto element : ordered_tracks)
        element->play();
}

Player::Player() {
    openers["audio"] = std::make_shared<SongOpener>();
    openers["video"] = std::make_shared<MovieOpener>();
}

Player::Player(std::unordered_map<std::string,
        std::shared_ptr<Opener>> otherOpeners) {
    openers["audio"] = std::make_shared<SongOpener>();
    openers["video"] = std::make_shared<MovieOpener>();
    openers.merge(otherOpeners);
}

std::shared_ptr<Piece> Player::openFile(const File &file) {
    auto it = openers.find(file.getType());
    if (it == openers.end())
        throw UnsupportedTypeException();

    return openers[file.getType()]->open(file.getMetadata(),
                                         file.getContents());
}

std::shared_ptr<Playlist>
Player::createPlaylist(const std::string &name) const {
    return std::make_shared<Playlist>(name);
}