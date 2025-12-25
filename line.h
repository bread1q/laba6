#ifndef LINE_H
#define LINE_H

#include "shape.h"
#include <algorithm>

class Line : public Shape
{
private:
    int x2_;
    int y2_;
    int thickness_;

public:
    Line(int x1, int y1, int x2, int y2, int thickness = 3);

    void draw(QPainter &painter) const override;
    bool contains(int x, int y) const override;
    QRect getBorderRect() const override;
    QRect getSafeBorderRect(int margin = 0) const override;

    bool checkBounds(int left, int top, int right, int bottom) const override;

    void move(int dx, int dy) override;

    int getX2() const;
    int getY2() const;
    int getThickness() const;
    void setEndPoint(int x2, int y2);
    void setThickness(int thickness);
};

#endif // LINE_H
