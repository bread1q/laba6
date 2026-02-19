#ifndef SHAPEFACTORY_H
#define SHAPEFACTORY_H

#include "composite.h"
#include <memory>
#include <string>

class ShapeFactory
{
public:
    virtual ~ShapeFactory() = default;

    // Создание элемента по его типу и данным
    static CompositeElement* createElement(const std::string& type, const std::string& data);

    // Создание элемента из строки (содержит тип и данные)
    static CompositeElement* createFromString(const std::string& data);

private:
    // Вспомогательные методы для создания конкретных фигур
    static CompositeElement* createCircle(const std::string& data);
    static CompositeElement* createRectangle(const std::string& data);
    static CompositeElement* createSquare(const std::string& data);
    static CompositeElement* createTriangle(const std::string& data);
    static CompositeElement* createLine(const std::string& data);
    static CompositeElement* createGroup(const std::string& data);
};

#endif // SHAPEFACTORY_H
