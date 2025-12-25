#include "line.h"

Line::Line(int x1, int y1, int x2, int y2, int thickness) : Shape(x1, y1), x2_(x2), y2_(y2), thickness_(thickness) {}

bool Line::contains(int x, int y) const {
    int left = std::min(x_, x2_) - thickness_ - 3;
    int top = std::min(y_, y2_) - thickness_ - 3;
    int right = std::max(x_, x2_) + thickness_ + 3;
    int bottom = std::max(y_, y2_) + thickness_ + 3;

    QRect bounds(left, top, right - left, bottom - top);
    return bounds.contains(x, y);
}

void Line::draw(QPainter &painter) const {
    if (selected_) {
        painter.setPen(QPen(Qt::blue, thickness_ + 2));
    } else {
        painter.setPen(QPen(color_, thickness_));
    }

    painter.drawLine(x_, y_, x2_, y2_);
}

bool Line::checkBounds(int left, int top, int right, int bottom) const {
    QRect bounds = getBorderRect();
    return (bounds.left() >= left &&
            bounds.right() <= right &&
            bounds.top() >= top &&
            bounds.bottom() <= bottom);
}

QRect Line::getBorderRect() const {
    int left = std::min(x_, x2_);
    int top = std::min(y_, y2_);
    int right = std::max(x_, x2_);
    int bottom = std::max(y_, y2_);

    return QRect(left, top, right - left, bottom - top);
}

QRect Line::getSafeBorderRect(int margin) const {
    QRect bounds = getBorderRect();
    int extra = thickness_ + margin;
    return bounds.adjusted(-extra, -extra, extra, extra);
}

void Line::move(int dx, int dy) {
    x_ += dx;
    y_ += dy;
    x2_ += dx;
    y2_ += dy;
}

int Line::getX2() const {
    return x2_;
}

int Line::getY2() const {
    return y2_;
}

int Line::getThickness() const {
    return thickness_;
}

void Line::setEndPoint(int x2, int y2) {
    x2_ = x2;
    y2_ = y2;
}

void Line::setThickness(int thickness) {
    thickness_ = thickness;
}
