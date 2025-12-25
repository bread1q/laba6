#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "shape.h"

class Rectangle : public Shape
{
protected:
    int width_;
    int height_;

public:
    Rectangle(int x, int y, int width = 50, int height = 30);

    void draw(QPainter &painter) const override;
    bool contains(int x, int y) const override;
    QRect getBorderRect() const override;

    int getWidth() const;
    int getHeight() const;
    void setSize(int width, int height);
};

#endif // RECTANGLE_H
