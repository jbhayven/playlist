#include "playlist.h"

size_t File::findColon(const std::string &line) {
    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] == ':')
            return i;
    }
    return line.size();
}

void File::censorship(const std::string &line) {
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

const std::string& File::getType() const {
    return this->type;
}

const std::unordered_map<std::string, std::string>& File::getMetadata() const {
    return this->metadata;
}

const std::string& File::getContents() const {
    return this->contents;
}

void Song::play() const {
    std::cout << "Song [" << artist << ", " << title << "]: ";
    std::cout << contents << "\n";
};

std::string Movie::decipher(std::string line) {

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

void Movie::play() const {
    std::cout << "Movie [" << title << ", " << year << "]: ";
    std::cout << contents << "\n";
};

size_t CompositePlayable::size() {
    return child_components.size();
}

bool CompositePlayable::reachable(CompositePlayable *looked_up) {
    if (this == looked_up) return true;

    for (auto elem : child_composites)
        if (elem->reachable(looked_up))
            return true;

    return false;
}

void CompositePlayable::add(piece_ptr elem, size_t position) {
    if (position > size()) throw OutOfBoundsException();

    child_components.insert(child_components.begin() + position, elem);
}

void CompositePlayable::add(piece_ptr elem) {
    child_components.push_back(elem);
}

void CompositePlayable::add(composite_ptr elem, size_t position) {
    if (elem->reachable(this)) throw LoopingException();
    if (position > size()) throw OutOfBoundsException();

    child_components.insert(child_components.begin() + position, elem);
    child_composites.push_back(elem.get());
}

void CompositePlayable::add(composite_ptr elem) {
    if (elem->reachable(this)) throw LoopingException();

    child_components.push_back(elem);
    child_composites.push_back(elem.get());
}

void CompositePlayable::remove(size_t position) {
    if (position >= size()) throw OutOfBoundsException();

    auto it = find(child_composites.begin(), child_composites.end(),
                   child_components[position].get());

    child_components.erase(child_components.begin() + position);
    if (it != child_composites.end()) child_composites.erase(it);
}

void CompositePlayable::remove() {
    if (size() == 0) throw OutOfBoundsException();

    remove(size() - 1);
}

void Playlist::setMode(const playmode_ptr &mode) {
    this->mode = mode;
}

void Playlist::play() const {
    std::cout << "Playlist [" << name << "]" << std::endl;

    std::vector<playable_ptr> ordered_tracks =
            mode->orderTracks(child_components);

    for (auto element : ordered_tracks)
        element->play();
}

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

Player::Player() {
    openers["audio"] = openSong;
    openers["video"] = openMovie;
}

std::shared_ptr<Piece> Player::openFile(const File &file) {
    auto it = openers.find(file.getType());
    if (it == openers.end())
        throw UnsupportedTypeException();

    return openers[file.getType()](file.getMetadata(), file.getContents());
}

std::shared_ptr<Playlist> Player::createPlaylist(const std::string &name) const {
    return std::make_shared<Playlist>(name);
}