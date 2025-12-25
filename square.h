#ifndef SQUARE_H
#define SQUARE_H

#include "rectangle.h"

class Square : public Rectangle
{
public:
    Square(int x, int y, int size = 40);

    void setSize(int size);

    void setSide(int size);
    int getSide() const;
};

#endif // SQUARE_H
