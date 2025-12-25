#include "circle.h"

Circle::Circle(int x, int y, int radius) : Shape(x, y), radius_(radius) {}

bool Circle::contains(int x, int y) const {
    int dx = x - x_;
    int dy = y - y_;
    return (dx * dx + dy * dy) <= (radius_ * radius_);
}

void Circle::draw(QPainter &painter) const {
    if (selected_) {
        painter.setPen(QPen(Qt::blue, 2));
        painter.setBrush(color_.lighter(150));
    }
    else {
        painter.setPen(QPen(Qt::black, 2));
        painter.setBrush(color_);
    }

    painter.drawEllipse(x_ - radius_, y_ - radius_, 2 * radius_, 2 * radius_);
}

QRect Circle::getBorderRect() const {
    return QRect(x_ - radius_, y_ - radius_, 2 * radius_, 2 * radius_);
}

int Circle::getRadius() const {
    return radius_;
}

void Circle::setRadius(int r) {
    radius_ = r;
}
