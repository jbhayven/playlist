#ifndef _PLAYABLE_EXCEPTION_H
#define _PLAYABLE_EXCEPTION_H

#include <exception>

class PlayableException : public std::exception {};

class PlayableCompositeException : public PlayableException {};

class LoopingException : public PlayableCompositeException {
    const char *what() const noexcept override {
        return "adding failed: a loop would be created";
    }
};

class OutOfBoundsException : public PlayableCompositeException {
    const char *what() const noexcept override {
        return "position out of bounds";
    }
};

#endif