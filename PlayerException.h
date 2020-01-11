#ifndef _PLAYER_EXCEPTION_H
#define _PLAYER_EXCEPTION_H

#include <exception>

class PlayerException : std::exception {
protected:
    // Nie jestem pewien, czy to powinno być statyczne; może lepiej inicjalizować
    // dla każdego obiektu osobno?
    static const char* message;
public:
    // to lepiej pasuje do przykładu
    const char* what() const noexcept override {
        return message;
    }
};

class LoopingPlaylistsException : PlayerException {
    inline static const char* message = "adding failed: a loop would be created";
};

class CorruptFileException : PlayerException {
    inline static const char* message = "corrupt file";
};

class CorruptContentException : PlayerException {
    inline static const char* message = "corrupt content";
};

class UnsupportedTypeException : PlayerException {
    inline static const char* message = "unsupported type";
};

#endif