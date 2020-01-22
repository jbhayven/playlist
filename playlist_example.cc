#include <unordered_map>
#include <iostream>
#include <utility>
#include "lib_playlist.h"

using namespace std;

class mp4 : public Piece {
    const std::string artist;
    const std::string title;
    const std::string length;
    const std::string contents;

public:
    mp4(std::unordered_map<std::string, std::string> metadata,
        std::string contents)
            : artist(metadata["artist"]), title(metadata["title"]),
              length(metadata["length"]), contents(std::move(contents)) {}

    void play() const noexcept override {
        std::cout << "mp4 [" << artist << ", " << title << ", " << length
                  << "]: ";
        std::cout << contents << "\n";
    };
};

class mp4Opener : public Opener {
public:
    std::shared_ptr<Piece>
    open(std::unordered_map<std::string, std::string> metadata,
         std::string contents) override {
        return std::make_shared<mp4>(
                mp4(std::move(metadata), std::move(contents)));
    }
};

int main() {

    unordered_map<string, shared_ptr<Opener>> myOpeners;
    myOpeners["mp4"] = make_shared<mp4Opener>();

    auto player = Player(myOpeners);

    auto mishmash = player.createPlaylist("mishmash");
    auto armstrong = player.createPlaylist("armstrong");

    auto whatAWonderfulWorld = player.openFile(
            File("audio|artist:Louis Armstrong|title:What a Wonderful World|"
                 "I see trees of green, red roses too..."));
    auto helloDolly = player.openFile(
            File("audio|artist:Louis Armstrong|title:Hello, Dolly!|"
                 "Hello, Dolly! This is Louis, Dolly"));
    auto nowy = player.openFile(
            File("mp4|artist:ktos|title:cos|length:1m13s|halo, halo, jest tam kto?"));

    armstrong->add(whatAWonderfulWorld);
    armstrong->add(helloDolly);
    armstrong->add(nowy);
    auto direstraits = player.openFile(
            File("audio|artist:Dire Straits|title:Money for Nothing|year:1717|"
                 "Now look at them yo-yo's that's the way you do it..."));
    auto cabaret = player.openFile(
            File("video|title:Cabaret|year:9876|year:1972|Qvfcynlvat Pnonerg"));


    mishmash->add(cabaret);
    mishmash->add(armstrong);
    mishmash->add(direstraits, 1);
    mishmash->add(direstraits);

    std::cout << "=== Playing 'mishmash' (default sequence mode)" << std::endl;
    mishmash->play();

    std::cout
            << "=== Playing 'mishmash' (shuffle mode, seed 0 for std::default_random_engine)"
            << std::endl;
    mishmash->setMode(createShuffleMode(0));
    mishmash->play();

    std::cout
            << "=== Playing 'mishmash' (removed cabaret and last direstraits, odd-even mode)"
            << std::endl;
    mishmash->remove(0);
    mishmash->remove();
    mishmash->setMode(createOddEvenMode());
    mishmash->play();

    std::cout
            << "=== Playing 'mishmash' (sequence mode, 'armstrong' odd-even mode)"
            << std::endl;
    armstrong->setMode(createOddEvenMode());
    mishmash->setMode(createSequenceMode());
    mishmash->play();

    try {
        auto unsupported = player.openFile(
                File("mp3|artist:Unsupported|title:Unsupported|Content"));
    } catch (PlayerException const &e) {
        std::cout << e.what() << std::endl;
    }

    try {
        auto corrupted = player.openFile(File("Corrupt"));
    } catch (PlayerException const &e) {
        std::cout << e.what() << std::endl;
    }

    try {
        auto corrupted = player.openFile(
                File("audio|artist:Louis Armstrong|title:Hello, Dolly!|%#!@*&"));
    } catch (PlayerException const &e) {
        std::cout << e.what() << std::endl;
    }

    auto mode = createShuffleMode(1);
    armstrong->setMode(mode);
    mishmash->setMode(mode);

    std::cout << "=== Playing 'armstrong' (shuffle mode(1))" << std::endl;
    armstrong->play();
    std::cout
            << "=== Playing 'mishmash' (shuffle mode(1), 'armstrong' shuffle mode(1))"
            << std::endl;
    mishmash->play();
    mishmash->add(armstrong, 1);

    try {
        armstrong->add(mishmash);
    } catch (PlayerException const &e) {
        std::cout << e.what() << std::endl;
    }

    try {
        auto newVideo = player.openFile(File("video|year:1023|Qvfcynlvat Pnonerg"));
        armstrong->add(newVideo);
        armstrong->play();
    } catch (PlayerException const &e) {
        std::cout << e.what() << std::endl;
    }


}