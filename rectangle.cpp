#include "rectangle.h"

Rectangle::Rectangle(int x, int y, int width, int height) : Shape(x, y), width_(width), height_(height) {}

bool Rectangle::contains(int x, int y) const {
    return (x >= x_ && x <= x_ + width_ && y >= y_ && y <= y_ + height_);
}

void Rectangle::draw(QPainter &painter) const {
    if (selected_) {
        painter.setPen(QPen(Qt::blue, 2));
        painter.setBrush(color_.lighter(150));
    }
    else {
        painter.setPen(QPen(Qt::black, 1));
        painter.setBrush(color_);
    }

    painter.drawRect(x_, y_, width_, height_);
}

QRect Rectangle::getBorderRect() const {
    return QRect(x_, y_, width_, height_);
}

int Rectangle::getWidth() const {
    return width_;
}

int Rectangle::getHeight() const {
    return height_;
}

void Rectangle::setSize(int width, int height) {
    width_ = width;
    height_ = height;
}
