#ifndef SHAPE_H
#define SHAPE_H

#include <QPainter>
#include <QRect>
#include <QColor>

class Shape
{
protected:
    int x_;
    int y_;
    QColor color_;
    bool selected_;

public:
    Shape(int x, int y);
    virtual ~Shape() = default;

    virtual void draw(QPainter &painter) const = 0;
    virtual bool contains(int x, int y) const = 0;
    virtual QRect getBorderRect() const = 0;
    virtual QRect getSafeBorderRect(int margin = 0) const;

    virtual void move(int dx, int dy);
    virtual bool checkBounds(int left, int top, int right, int bottom) const;
    virtual bool safeMove(int dx, int dy, int left, int top, int right, int bottom);

    virtual bool canChangeSize(int left, int top, int right, int bottom, int margin = 0) const;

    bool getSelected() const;
    void setSelected(bool selected);

    QColor getColor() const;
    void setColor(const QColor& color);

    int getX() const;
    int getY() const;
    void setPosition(int x, int y);
};

#endif // SHAPE_H
