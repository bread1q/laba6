#include "composite.h"
#include "circle.h"
#include "rectangle.h"
#include "square.h"
#include "triangle.h"
#include "line.h"

std::string ShapeAdapter::getTypeName() const
{
    if (dynamic_cast<Circle*>(shape_)) return "Circle";
    if (dynamic_cast<Rectangle*>(shape_)) return "Rectangle";
    if (dynamic_cast<Square*>(shape_)) return "Square";
    if (dynamic_cast<Triangle*>(shape_)) return "Triangle";
    if (dynamic_cast<Line*>(shape_)) return "Line";
    return "Unknown";
}
