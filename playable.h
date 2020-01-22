#ifndef PLAYLIST_PLAYABLE_H
#define PLAYLIST_PLAYABLE_H

#include "playable_exception.h"
#include <algorithm>
#include <memory>
#include <vector>

class Playable {
public:
    virtual void play() const = 0;

    virtual ~Playable() = default;
};

class Piece : public Playable {
};

class CompositePlayable : public Playable {
protected:
    using playable_ptr = std::shared_ptr<Playable>;
    using composite_ptr = std::shared_ptr<CompositePlayable>;
    using piece_ptr = std::shared_ptr<Piece>;

    std::vector<playable_ptr> child_components;
    std::vector<CompositePlayable *> child_composites;

    size_t size() const;

    bool reachable(CompositePlayable *looked_up) const;

public:
    virtual void add(piece_ptr elem, size_t position);

    virtual void add(piece_ptr elem);

    virtual void add(composite_ptr elem, size_t position);

    virtual void add(composite_ptr elem);

    virtual void remove(size_t position);

    virtual void remove();
};

#endif