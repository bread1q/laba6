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
#include <iostream>

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
    std::cout << "Creating from string: " << data << std::endl;

    std::istringstream iss(data);
    std::string type;
    iss >> type;

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

    std::cerr << "Unknown type: " << type << std::endl;
    return nullptr;
}

CompositeElement* ShapeFactory::createCircle(const std::string& data)
{
    std::istringstream iss(data);
    std::string type;
    int x, y, r, g, b, a;
    bool selected;
    int radius;

    iss >> type >> x >> y >> r >> g >> b >> a >> selected >> radius;

    std::cout << "Creating Circle: x=" << x << " y=" << y << " radius=" << radius
              << " selected=" << selected << std::endl;

    Circle* circle = new Circle(x, y, radius);
    circle->setColor(QColor(r, g, b, a));
    circle->setSelected(selected);

    return new ShapeAdapter(circle);
}

CompositeElement* ShapeFactory::createRectangle(const std::string& data)
{
    std::istringstream iss(data);
    std::string type;
    int x, y, r, g, b, a;
    bool selected;
    int width, height;

    iss >> type >> x >> y >> r >> g >> b >> a >> selected >> width >> height;

    std::cout << "Creating Rectangle: x=" << x << " y=" << y
              << " width=" << width << " height=" << height
              << " selected=" << selected << std::endl;

    Rectangle* rect = new Rectangle(x, y, width, height);
    rect->setColor(QColor(r, g, b, a));
    rect->setSelected(selected);

    return new ShapeAdapter(rect);
}

CompositeElement* ShapeFactory::createSquare(const std::string& data)
{
    std::istringstream iss(data);
    std::string type;
    int x, y, r, g, b, a;
    bool selected;
    int size;

    iss >> type >> x >> y >> r >> g >> b >> a >> selected >> size;

    std::cout << "Creating Square: x=" << x << " y=" << y
              << " size=" << size << " selected=" << selected << std::endl;

    Square* square = new Square(x, y, size);
    square->setColor(QColor(r, g, b, a));
    square->setSelected(selected);

    return new ShapeAdapter(square);
}

CompositeElement* ShapeFactory::createTriangle(const std::string& data)
{
    std::istringstream iss(data);
    std::string type;
    int x, y, r, g, b, a;
    bool selected;
    int size;

    iss >> type >> x >> y >> r >> g >> b >> a >> selected >> size;

    std::cout << "Creating Triangle: x=" << x << " y=" << y
              << " size=" << size << " selected=" << selected << std::endl;

    Triangle* triangle = new Triangle(x, y, size);
    triangle->setColor(QColor(r, g, b, a));
    triangle->setSelected(selected);

    return new ShapeAdapter(triangle);
}

CompositeElement* ShapeFactory::createLine(const std::string& data)
{
    std::istringstream iss(data);
    std::string type;
    int x, y, r, g, b, a;
    bool selected;
    int x2, y2, thickness;

    iss >> type >> x >> y >> r >> g >> b >> a >> selected >> x2 >> y2 >> thickness;

    std::cout << "Creating Line: x1=" << x << " y1=" << y
              << " x2=" << x2 << " y2=" << y2
              << " thickness=" << thickness << " selected=" << selected << std::endl;

    Line* line = new Line(x, y, x2, y2, thickness);
    line->setColor(QColor(r, g, b, a));
    line->setSelected(selected);

    return new ShapeAdapter(line);
}

CompositeElement* ShapeFactory::createGroup(const std::string& data)
{
    std::cout << "Creating group from data: " << data << std::endl;

    std::istringstream iss(data);
    std::string type;
    int selected;
    int r, g, b, a;
    int childCount;

    // Читаем заголовок группы
    iss >> type >> selected >> r >> g >> b >> a >> childCount;

    std::cout << "Group header: selected=" << selected
              << " color=" << r << "," << g << "," << b << "," << a
              << " childCount=" << childCount << std::endl;

    Group* group = new Group();
    group->setSelected(selected != 0);
    group->setColor(QColor(r, g, b, a));

    // Загружаем детей
    for (int i = 0; i < childCount; ++i) {
        // Ищем открывающую скобку
        char ch;
        iss >> ch; // Должна быть '{'
        if (ch != '{') {
            std::cerr << "Expected '{', got '" << ch << "'" << std::endl;
            break;
        }

        // Читаем данные ребенка до закрывающей скобки
        std::string childData;
        int braceCount = 1;
        while (braceCount > 0 && iss.get(ch)) {
            if (ch == '{') braceCount++;
            else if (ch == '}') braceCount--;

            if (braceCount > 0) {
                childData += ch;
            }
        }

        std::cout << "Child " << i << " data: " << childData << std::endl;

        if (!childData.empty()) {
            CompositeElement* child = createFromString(childData);
            if (child) {
                group->addChild(child);
                std::cout << "Successfully added child of type " << child->getTypeName() << std::endl;
            } else {
                std::cerr << "Failed to create child from data: " << childData << std::endl;
            }
        }
    }

    std::cout << "Group created with " << group->getChildren().size() << " children" << std::endl;
    return group;
}
