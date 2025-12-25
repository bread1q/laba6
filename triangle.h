#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "shape.h"

class Triangle : public Shape
{
private:
    int size_;

public:
    Triangle(int x, int y, int size = 40);

    void draw(QPainter &painter) const override;
    bool contains(int x, int y) const override;
    QRect getBorderRect() const override;

    int getSize() const;
    void setSize(int size);
};

#endif // TRIANGLE_H
