#ifndef CIRCLE_H
#define CIRCLE_H

#include "shape.h"

class Circle : public Shape
{
private:
    int radius_;

public:
    Circle(int x, int y, int radius);

    void draw(QPainter &painter) const override;
    bool contains(int x, int y) const override;
    QRect getBorderRect() const override;

    int getRadius() const;
    void setRadius(int r);
};

#endif // CIRCLE_H
