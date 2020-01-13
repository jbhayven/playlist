#ifndef PLAYLIST_PLAYABLE_H
#define PLAYLIST_PLAYABLE_H

class Playable {
public:
    virtual void play() const = 0;
    
    virtual ~Playable() = 0;
};

#endif
