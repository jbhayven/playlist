#ifndef _PLAYER_EXCEPTION_H
#define _PLAYER_EXCEPTION_H

#include <exception>

class PlayerException : public std::exception {
public:
    virtual const char* what() const noexcept override = 0;
};

class LoopingPlaylistsException : public PlayerException {
    const char* what() const noexcept override { 
        return "adding failed: a loop would be created";
    }
};

class OutOfBoundsException : public PlayerException {
    const char* what() const noexcept override { 
        return "position out of bounds";
    }
};

class CorruptFileException : public PlayerException {
    const char* what() const noexcept override { 
        return "corrupt file";
    }
};

class CorruptContentException : public PlayerException {
    const char* what() const noexcept override { 
        return "corrupt content";
    }
};

class UnsupportedTypeException : public PlayerException {
    const char* what() const noexcept override { 
        return "unsupported type";
    }
};

#endif
