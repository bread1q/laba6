#ifndef SHAPECONTAINER_H
#define SHAPECONTAINER_H

#include <vector>
#include "shape.h"

class ShapeContainer
{
private:
    std::vector<Shape*> shapes_;

public:
    ShapeContainer();
    ~ShapeContainer();

    void addShape(Shape *shape);
    void removeShape(int i);
    void clear();

    void clearSelection();
    void removeSelected();
    void selectAll();

    Shape* getShape(int i) const;
    int getCount() const;

    void moveSelected(int dx, int dy, int maxX, int maxY, int topMargin);
    void setSelectedColor(const QColor &color);
};

#endif // SHAPECONTAINER_H
