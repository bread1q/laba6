#include "triangle.h"

Triangle::Triangle(int x, int y, int size) : Shape(x, y), size_(size) {}

bool Triangle::contains(int x, int y) const {
    QRect bounds = getBorderRect().adjusted(-5, -5, 5, 5);
    return bounds.contains(x, y);
}

void Triangle::draw(QPainter &painter) const {
    if (selected_) {
        painter.setPen(QPen(Qt::blue, 2));
        painter.setBrush(color_.lighter(150));
    }
    else {
        painter.setPen(QPen(Qt::black, 1));
        painter.setBrush(color_);
    }

    QPoint points[3] = {
        QPoint(x_, y_ + size_ / 2),
        QPoint(x_ + size_, y_ + size_ / 2),
        QPoint(x_ + size_ / 2, y_ - size_ / 2)
    };

    painter.drawPolygon(points, 3);
}

QRect Triangle::getBorderRect() const {
    return QRect(x_, y_ - size_ / 2, size_, size_);
}

int Triangle::getSize() const {
    return size_;
}

void Triangle::setSize(int size) {
    size_ = size;
}
