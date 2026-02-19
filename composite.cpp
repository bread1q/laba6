#include "composite.h"
#include "circle.h"
#include "rectangle.h"
#include "square.h"
#include "triangle.h"
#include "line.h"
#include <sstream>
#include <iostream>

std::string ShapeAdapter::getTypeName() const
{
    if (dynamic_cast<Circle*>(shape_)) return "Circle";
    if (dynamic_cast<Rectangle*>(shape_)) return "Rectangle";
    if (dynamic_cast<Square*>(shape_)) return "Square";
    if (dynamic_cast<Triangle*>(shape_)) return "Triangle";
    if (dynamic_cast<Line*>(shape_)) return "Line";
    return "Unknown";
}

// Методы сохранения/загрузки для ShapeAdapter
std::string ShapeAdapter::save() const
{
    std::ostringstream oss;

    // Сохраняем тип
    oss << getTypeName() << " ";

    // Сохраняем основные свойства
    oss << shape_->getX() << " "
        << shape_->getY() << " "
        << shape_->getColor().red() << " "
        << shape_->getColor().green() << " "
        << shape_->getColor().blue() << " "
        << shape_->getColor().alpha() << " "
        << (shape_->getSelected() ? "1" : "0") << " ";

    // Сохраняем специфичные для фигуры данные
    if (Circle* circle = dynamic_cast<Circle*>(shape_)) {
        oss << circle->getRadius();
    }
    else if (Rectangle* rect = dynamic_cast<Rectangle*>(shape_)) {
        oss << rect->getWidth() << " " << rect->getHeight();
    }
    else if (Square* square = dynamic_cast<Square*>(shape_)) {
        oss << square->getSide();
    }
    else if (Triangle* triangle = dynamic_cast<Triangle*>(shape_)) {
        oss << triangle->getSize();
    }
    else if (Line* line = dynamic_cast<Line*>(shape_)) {
        oss << line->getX2() << " " << line->getY2() << " " << line->getThickness();
    }

    return oss.str();
}

void ShapeAdapter::load(const std::string& data)
{
    std::istringstream iss(data);
    std::string type;
    iss >> type;

    // Основные свойства
    int x, y, r, g, b, a;
    bool selected;
    iss >> x >> y >> r >> g >> b >> a >> selected;

    QColor color(r, g, b, a);

    // Удаляем старую фигуру
    delete shape_;

    // Создаем новую фигуру в зависимости от типа
    if (type == "Circle") {
        int radius;
        iss >> radius;
        shape_ = new Circle(x, y, radius);
    }
    else if (type == "Rectangle") {
        int width, height;
        iss >> width >> height;
        shape_ = new Rectangle(x, y, width, height);
    }
    else if (type == "Square") {
        int size;
        iss >> size;
        shape_ = new Square(x, y, size);
    }
    else if (type == "Triangle") {
        int size;
        iss >> size;
        shape_ = new Triangle(x, y, size);
    }
    else if (type == "Line") {
        int x2, y2, thickness;
        iss >> x2 >> y2 >> thickness;
        shape_ = new Line(x, y, x2, y2, thickness);
    }

    if (shape_) {
        shape_->setColor(color);
        shape_->setSelected(selected);
    }
}

// Методы для сохранения/загрузки детей (для групп)
void CompositeElement::saveChildren(std::ostream& os) const
{
    // Базовая реализация ничего не делает
    Q_UNUSED(os);
}

void CompositeElement::loadChildren(std::istream& is)
{
    // Базовая реализация ничего не делает
    Q_UNUSED(is);
}
