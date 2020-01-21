#include "playable.h"

size_t CompositePlayable::size() const {
    return child_components.size();
}

bool CompositePlayable::reachable(CompositePlayable *looked_up) const {
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
