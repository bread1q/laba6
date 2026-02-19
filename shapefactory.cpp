#include "shapefactory.h"
#include "composite.h"
#include "group.h"
#include "circle.h"
#include "rectangle.h"
#include "square.h"
#include "triangle.h"
#include "line.h"
#include <sstream>
#include <memory>

CompositeElement* ShapeFactory::createElement(const std::string& type, const std::string& data)
{
    if (type == "Circle") {
        return createCircle(data);
    }
    else if (type == "Rectangle") {
        return createRectangle(data);
    }
    else if (type == "Square") {
        return createSquare(data);
    }
    else if (type == "Triangle") {
        return createTriangle(data);
    }
    else if (type == "Line") {
        return createLine(data);
    }
    else if (type == "Group") {
        return createGroup(data);
    }

    return nullptr;
}

CompositeElement* ShapeFactory::createFromString(const std::string& data)
{
    std::istringstream iss(data);
    std::string type;
    iss >> type;

    // Читаем остальные данные
    std::string remainingData;
    std::getline(iss, remainingData);

    return createElement(type, remainingData);
}

CompositeElement* ShapeFactory::createCircle(const std::string& data)
{
    std::istringstream iss(data);
    int x, y, r, g, b, a;
    bool selected;
    int radius;

    iss >> x >> y >> r >> g >> b >> a >> selected >> radius;

    Circle* circle = new Circle(x, y, radius);
    circle->setColor(QColor(r, g, b, a));
    circle->setSelected(selected);

    return new ShapeAdapter(circle);
}

CompositeElement* ShapeFactory::createRectangle(const std::string& data)
{
    std::istringstream iss(data);
    int x, y, r, g, b, a;
    bool selected;
    int width, height;

    iss >> x >> y >> r >> g >> b >> a >> selected >> width >> height;

    Rectangle* rect = new Rectangle(x, y, width, height);
    rect->setColor(QColor(r, g, b, a));
    rect->setSelected(selected);

    return new ShapeAdapter(rect);
}

CompositeElement* ShapeFactory::createSquare(const std::string& data)
{
    std::istringstream iss(data);
    int x, y, r, g, b, a;
    bool selected;
    int size;

    iss >> x >> y >> r >> g >> b >> a >> selected >> size;

    Square* square = new Square(x, y, size);
    square->setColor(QColor(r, g, b, a));
    square->setSelected(selected);

    return new ShapeAdapter(square);
}

CompositeElement* ShapeFactory::createTriangle(const std::string& data)
{
    std::istringstream iss(data);
    int x, y, r, g, b, a;
    bool selected;
    int size;

    iss >> x >> y >> r >> g >> b >> a >> selected >> size;

    Triangle* triangle = new Triangle(x, y, size);
    triangle->setColor(QColor(r, g, b, a));
    triangle->setSelected(selected);

    return new ShapeAdapter(triangle);
}

CompositeElement* ShapeFactory::createLine(const std::string& data)
{
    std::istringstream iss(data);
    int x, y, r, g, b, a;
    bool selected;
    int x2, y2, thickness;

    iss >> x >> y >> r >> g >> b >> a >> selected >> x2 >> y2 >> thickness;

    Line* line = new Line(x, y, x2, y2, thickness);
    line->setColor(QColor(r, g, b, a));
    line->setSelected(selected);

    return new ShapeAdapter(line);
}

CompositeElement* ShapeFactory::createGroup(const std::string& data)
{
    std::istringstream iss(data);
    int childCount;
    bool selected;
    int r, g, b, a;

    iss >> childCount >> selected >> r >> g >> b >> a;

    Group* group = new Group();
    group->setSelected(selected);
    group->setColor(QColor(r, g, b, a));

    // Загружаем детей
    for (int i = 0; i < childCount; ++i) {
        int dataLength;
        iss >> dataLength;

        // Пропускаем пробел
        iss.ignore(1, ' ');

        // Читаем данные ребенка
        std::string childData(dataLength, ' ');
        iss.read(&childData[0], dataLength);

        // Создаем ребенка
        CompositeElement* child = createFromString(childData);
        if (child) {
            group->addChild(child);
        }

        // Пропускаем пробел, если есть
        if (iss.peek() == ' ') {
            iss.ignore(1, ' ');
        }
    }

    return group;
}
