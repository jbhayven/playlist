#ifndef _PLAYABLE_EXCEPTION_H
#define _PLAYABLE_EXCEPTION_H

#include  "player_exception.h"
#include <exception>

class PlayableException : public PlayerException {
};

class PlayableCompositeException : public PlayableException {};

class LoopingException : public PlayableCompositeException {
public:
    const char *what() const noexcept override {
        return "adding failed: a loop would be created";
    }
};

class OutOfBoundsException : public PlayableCompositeException {
public:
    const char *what() const noexcept override {
        return "position out of bounds";
    }
};

#endif