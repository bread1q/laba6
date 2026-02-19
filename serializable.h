#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <string>
#include <sstream>

class Serializable
{
public:
    virtual ~Serializable() = default;

    // Сохранение объекта в строку
    virtual std::string save() const = 0;

    // Загрузка объекта из строки
    virtual void load(const std::string& data) = 0;

    // Получение имени типа для идентификации при загрузке
    virtual std::string getTypeName() const = 0;
};

#endif // SERIALIZABLE_H
