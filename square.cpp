#include "square.h"

Square::Square(int x, int y, int size) : Rectangle(x, y, size, size) {}

void Square::setSize(int size) {
    Rectangle::setSize(size, size);
}

void Square::setSide(int size) {
    setSize(size);
}

int Square::getSide() const {
    return getWidth();
}
