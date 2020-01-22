#include "playlist.h"
#include  <cassert>

bool add_file(Player player, std::shared_ptr<Playlist> playlist, const char* file_content) {
    try {
        auto file = File(file_content);
        auto opened_file = player.openFile(file);
        playlist->add(opened_file);
        return true;
    } catch (PlayerException const& e) {
        std::cout << "Nie udało się!" << std::endl;
        std::cout << file_content << std::endl;
        std::cout << e.what() << std::endl;
        return false;
    }
}

class InheritedOpener : public SongOpener {

};

int main() {
    Player player{};

    auto playlist = player.createPlaylist("Playlista numer 1");

    assert(add_file(player, playlist, "audio|artist:Lady Gaga|title:the_title|Song0"));
    assert(add_file(player, playlist, "audio|artist:Test|artist:Lady Gaga|title:the_title|Song1"));
    assert(add_file(player, playlist, "audio|artist:Lady Gaga|artist:Test|title:the_title|Song2"));
    assert(!add_file(player, playlist, "audio|artist:LadyGaga|title:the_title|Song3|Nothing2"));
    assert(add_file(player, playlist, "audio|artist:LadyGaga:Test|title:the_title|Song4"));
    assert(add_file(player, playlist, "audio|artist:LadyGaga|title:the_title|metadata:kolejne_data|Song5"));
    assert(add_file(player, playlist, "audio|artist:zażółć gęślą jaźń|title:the_title|Song6"));
    assert(!add_file(player, playlist, "|audio|artist:zażółć gęślą jaźń|title:the_title|Song7"));
    assert(!add_file(player, playlist, "audio|artist:LadyGaga|no_title:no_title|Song8"));
    assert(!add_file(player, playlist, "audio|artist:Lady Gaga|title:the_title|Song9|"));
    assert(!add_file(player, playlist, "audio|artist:Lady Gaga||title:the_title|Song10"));
    assert(add_file(player, playlist, "audio|artist:Lady Gaga|title:the_title| ,.!?':;- Song11"));
    assert(add_file(player, playlist, "audio|artist:Lady Gaga|title:the_title,.!?':;- | Song12"));
    assert(add_file(player, playlist, "audio|artist:Lady Gaga|title:the_title| ,.!?':;- : test | Song13"));
    assert(add_file(player, playlist, "audio|artist:Lady Gaga|title:the_ti%tle|Song14"));
    assert(!add_file(player, playlist, "audio|artist:Lady Gaga|title:the_title|Song15%%"));
    assert(add_file(player, playlist, "audio|artist:Xkbz69|title:Rap43v3r|Song16"));
    assert(add_file(player, playlist, "audio|artist:Lady Gaga|title:the_title|"));

    playlist->play();

    auto filmlist = player.createPlaylist("Playlista z filmami");

    assert(add_file(player, filmlist, "video|title:TheMovie1|year:1999|ybypbagrag"));
    assert(add_file(player, filmlist, "video|title:TheMovie2|year:0|ybypbagrag"));
    assert(add_file(player, filmlist, "video|title:TheMovie3|year:2221929|ybypbagrag"));
    assert(!add_file(player, filmlist, "video|title:TheMovie4|year:-1|ybypbagrag"));
    assert(!add_file(player, filmlist, "video|title:TheMovie5|year:tekstowyRok|ybypbagrag"));
    assert(add_file(player, filmlist, "video|title:TheMovie6|year:tekstowyRok|year:2020|ybypbagrag"));
    assert(!add_file(player, filmlist, "video|title:TheMovie7|year:2020|year:tekstowyRok|ybypbagrag"));
    assert(!add_file(player, filmlist, "video||title:TheMovie8|year:1999|ybypbagrag"));
    assert(add_file(player, filmlist, "video|title:TheMovie9|year:1999|"));
    assert(add_file(player, filmlist, "video|title:TheMovie10|year:1999|ybypbagrag ,.!?':;-"));
    assert(!add_file(player, filmlist, "video|title:TheMovie11|year:1999|ybypbagrag_"));
    assert(add_file(player, filmlist, "video|title:TheMovie12,.!?':;-|year:1999|ybypbagrag"));
    assert(!add_file(player, filmlist, "video|title:TheMovie13|extra_data|year:1999|ybypbagrag"));
    assert(add_file(player, filmlist, "video|title:TheMovie14|extra_data:data_value|year:1999|ybypbagrag"));

    filmlist->play();

    auto nad = player.createPlaylist("nad");
    auto syn1 = player.createPlaylist("1");
    auto syn2 = player.createPlaylist("2");
    auto syn3 = player.createPlaylist("3");

    nad->add(syn1);
    syn1->add(syn2);

    nad->add(syn2);

    syn1->add(syn3);

    try {
        syn3->add(nad);
    } catch(LoopingException const& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        syn3->remove();
    } catch(PlayerException const& e) {
        std::cout << e.what() << std::endl;
    }


    try {
        nad->remove(2);
    } catch(PlayerException const& e) {
        std::cout << e.what() << std::endl;
    }

    nad->play();
    std::cout << "========" << std::endl;
    syn1->play();
    std::cout << "========" << std::endl;
    syn2->play();
    std::cout << "========" << std::endl;
    syn3->play();

    nad->remove(0);
    syn1->remove();
    syn3->add(nad);

    syn1->remove();
    syn2->add(syn1);

    std::cout << "========" << std::endl;
    nad->play();
    std::cout << "========" << std::endl;
    syn1->play();
    std::cout << "========" << std::endl;
    syn2->play();
    std::cout << "========" << std::endl;
    syn3->play();

    auto list4 = player.createPlaylist("4");
    {
        auto list5 = player.createPlaylist("5");
        list4->add(list5);
        list4->setMode(createShuffleMode(101010));
    }

    std::cout << "========" << std::endl;
    list4->play();

    std::unordered_map< std::string, std::shared_ptr<Opener> > others;
    others["copied_audio"] = std::make_shared<InheritedOpener>();
    Player richerPlayer{others};

    auto playinherit = player.createPlaylist("Playlista numer 1");

    assert(add_file(richerPlayer, playinherit, "copied_audio|artist:Lady Gaga|title:the_title|Song0"));
    assert(add_file(richerPlayer, playinherit, "copied_audio|artist:Test|artist:Lady Gaga|title:the_title|Song1"));
    assert(add_file(richerPlayer, playinherit, "copied_audio|artist:Lady Gaga|artist:Test|title:the_title|Song2"));
    assert(!add_file(richerPlayer, playinherit, "copied_audio|artist:LadyGaga|title:the_title|Song3|Nothing2"));
    assert(add_file(richerPlayer, playinherit, "copied_audio|artist:LadyGaga:Test|title:the_title|Song4"));
    assert(add_file(richerPlayer, playinherit, "copied_audio|artist:LadyGaga|title:the_title|metadata:kolejne_data|Song5"));
    assert(add_file(richerPlayer, playinherit, "copied_audio|artist:zażółć gęślą jaźń|title:the_title|Song6"));
    assert(!add_file(richerPlayer, playinherit, "|copied_audio|artist:zażółć gęślą jaźń|title:the_title|Song7"));
    assert(!add_file(richerPlayer, playinherit, "copied_audio|artist:LadyGaga|no_title:no_title|Song8"));
    assert(!add_file(richerPlayer, playinherit, "copied_audio|artist:Lady Gaga|title:the_title|Song9|"));
    assert(!add_file(richerPlayer, playinherit, "copied_audio|artist:Lady Gaga||title:the_title|Song10"));
    assert(add_file(richerPlayer, playinherit, "copied_audio|artist:Lady Gaga|title:the_title| ,.!?':;- Song11"));
    assert(add_file(richerPlayer, playinherit, "copied_audio|artist:Lady Gaga|title:the_title,.!?':;- | Song12"));
    assert(add_file(richerPlayer, playinherit, "copied_audio|artist:Lady Gaga|title:the_title| ,.!?':;- : test | Song13"));
    assert(add_file(richerPlayer, playinherit, "copied_audio|artist:Lady Gaga|title:the_ti%tle|Song14"));
    assert(!add_file(richerPlayer, playinherit, "copied_audio|artist:Lady Gaga|title:the_title|Song15%%"));
    assert(add_file(richerPlayer, playinherit, "copied_audio|artist:Xkbz69|title:Rap43v3r|Song16"));
    assert(add_file(richerPlayer, playinherit, "copied_audio|artist:Lady Gaga|title:the_title|"));

    playinherit->play();

    return 0;
}
