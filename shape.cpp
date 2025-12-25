#include "shape.h"

Shape::Shape(int x, int y) : x_(x), y_(y), color_(Qt::red), selected_(false) {}

void Shape::move(int dx, int dy) {
    x_ += dx;
    y_ += dy;
}

QRect Shape::getSafeBorderRect(int margin) const {
    QRect bounds = getBorderRect();
    return bounds.adjusted(-margin, -margin, margin, margin);
}

bool Shape::checkBounds(int left, int top, int right, int bottom) const {
    QRect bounds = getBorderRect();
    return (bounds.left() >= left &&
            bounds.right() <= right &&
            bounds.top() >= top &&
            bounds.bottom() <= bottom);
}

bool Shape::safeMove(int dx, int dy, int left, int top, int right, int bottom) {
    int oldX = x_;
    int oldY = y_;

    move(dx, dy);

    if (!checkBounds(left, top, right, bottom)) {
        setPosition(oldX, oldY);
        return false;
    }

    return true;
}

bool Shape::canChangeSize(int left, int top, int right, int bottom, int margin) const {
    return checkBounds(left, top, right, bottom);
}

bool Shape::getSelected() const {
    return selected_;
}

void Shape::setSelected(bool selected) {
    selected_ = selected;
}

QColor Shape::getColor() const {
    return color_;
}

void Shape::setColor(const QColor &color) {
    color_ = color;
}

int Shape::getX() const {
    return x_;
}

int Shape::getY() const {
    return y_;
}

void Shape::setPosition(int x, int y) {
    x_ = x;
    y_ = y;
}
