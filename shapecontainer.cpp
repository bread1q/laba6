#include "shapecontainer.h"
#include "shapefactory.h"
#include "group.h"
#include "line.h"
#include <QPainter>
#include <fstream>
#include <sstream>
#include <iostream>

ShapeContainer::ShapeContainer() {}

ShapeContainer::~ShapeContainer() {
    clear();
}

void ShapeContainer::addElement(CompositeElement* element) {
    if (element != nullptr) elements_.push_back(element);
}

void ShapeContainer::removeElement(int i) {
    if (i >= 0 && i < (int)elements_.size()) {
        delete elements_[i];
        elements_.erase(elements_.begin() + i);
    }
}

void ShapeContainer::clear() {
    for (auto element : elements_) {
        delete element;
    }
    elements_.clear();
}

void ShapeContainer::clearSelection() {
    for (auto element : elements_) {
        element->setSelected(false);
    }
}

void ShapeContainer::removeSelected() {
    for (int i = (int)elements_.size() - 1; i >= 0; i--) {
        if (elements_[i]->getSelected()) {
            delete elements_[i];
            elements_.erase(elements_.begin() + i);
        }
    }
}

void ShapeContainer::selectAll() {
    for (auto element : elements_) {
        element->setSelected(true);
    }
}

CompositeElement* ShapeContainer::getElement(int i) const {
    if (i >= 0 && i < (int)elements_.size()) {
        return elements_[i];
    }
    return nullptr;
}

int ShapeContainer::getCount() const {
    return (int)elements_.size();
}

std::vector<CompositeElement*> ShapeContainer::getSelectedElements() const {
    std::vector<CompositeElement*> selected;
    for (auto element : elements_) {
        if (element->getSelected()) {
            selected.push_back(element);
        }
    }
    return selected;
}

bool ShapeContainer::hasSelectedElements() const {
    for (auto element : elements_) {
        if (element->getSelected()) {
            return true;
        }
    }
    return false;
}

int ShapeContainer::getSelectedCount() const {
    int count = 0;
    for (auto element : elements_) {
        if (element->getSelected()) {
            count++;
        }
    }
    return count;
}

void ShapeContainer::groupSelected() {
    std::vector<CompositeElement*> selected = getSelectedElements();

    if (selected.size() < 2) {
        return; // Нужно минимум 2 элемента для группировки
    }

    // Создаем новую группу
    Group* newGroup = new Group();

    // Добавляем выбранные элементы в группу и удаляем их из контейнера
    for (auto element : selected) {
        // Находим элемент в контейнере и удаляем его
        for (int i = (int)elements_.size() - 1; i >= 0; i--) {
            if (elements_[i] == element) {
                // Добавляем в группу
                newGroup->addChild(element);
                // Удаляем из контейнера, но НЕ удаляем память
                elements_.erase(elements_.begin() + i);
                break;
            }
        }
    }

    // Добавляем новую группу в контейнер
    elements_.push_back(newGroup);
    newGroup->setSelected(true);
}

void ShapeContainer::ungroupSelected() {
    std::vector<CompositeElement*> selected = getSelectedElements();

    for (auto element : selected) {
        if (element->isGroup()) {
            Group* group = dynamic_cast<Group*>(element);
            if (group) {
                // Получаем детей группы
                const std::vector<CompositeElement*>& children = group->getChildren();

                // Добавляем детей обратно в контейнер
                for (auto child : children) {
                    // Устанавливаем выделение как у группы
                    child->setSelected(element->getSelected());
                    elements_.push_back(child);
                }

                // Находим группу в контейнере и удаляем ее
                for (int i = (int)elements_.size() - 1; i >= 0; i--) {
                    if (elements_[i] == element) {
                        // Удаляем группу, но НЕ удаляем детей (они уже добавлены в контейнер)
                        // Важно: очищаем вектор детей, чтобы деструктор не удалил их
                        const_cast<std::vector<CompositeElement*>&>(group->getChildren()).clear();
                        delete elements_[i];
                        elements_.erase(elements_.begin() + i);
                        break;
                    }
                }
            }
        }
    }
}

void ShapeContainer::moveSelected(int dx, int dy, int maxX, int maxY, int topMargin) {
    int left = 0;
    int top = topMargin;
    int right = maxX;
    int bottom = maxY;

    // Для каждого выбранного элемента пытаемся переместить его
    for (auto element : elements_) {
        if (element->getSelected()) {
            // Используем safeMove для проверки границ
            element->safeMove(dx, dy, left, top, right, bottom);
        }
    }
}

void ShapeContainer::setSelectedColor(const QColor &color) {
    // Собираем все выбранные элементы (включая вложенные в группы)
    std::vector<CompositeElement*> allSelected;
    for (auto element : elements_) {
        if (element->getSelected()) {
            collectAllElements(element, allSelected);
        }
    }

    // Устанавливаем цвет всем выбранным элементам
    for (auto element : allSelected) {
        element->setColor(color);
    }
}

void ShapeContainer::collectAllElements(CompositeElement* element, std::vector<CompositeElement*>& result) const {
    result.push_back(element);
    if (element->isGroup()) {
        const std::vector<CompositeElement*>& children = element->getChildren();
        for (auto child : children) {
            collectAllElements(child, result);
        }
    }
}

void ShapeContainer::collectNonGroupElements(CompositeElement* element, std::vector<CompositeElement*>& result) const {
    if (!element->isGroup()) {
        result.push_back(element);
    } else {
        const std::vector<CompositeElement*>& children = element->getChildren();
        for (auto child : children) {
            collectNonGroupElements(child, result);
        }
    }
}

std::string ShapeContainer::saveToString() const
{
    std::ostringstream oss;

    // Сохраняем количество элементов
    oss << elements_.size() << "\n";

    // Сохраняем каждый элемент
    for (auto element : elements_) {
        std::string elementData = element->save();
        oss << elementData.length() << "\n";
        oss << elementData << "\n";
    }

    return oss.str();
}

bool ShapeContainer::saveToFile(const std::string& filename) const
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // Сохраняем количество элементов
    file << elements_.size() << "\n";

    // Сохраняем каждый элемент
    for (auto element : elements_) {
        std::string elementData = element->save();
        // Убираем лишние пробелы в конце строки, если есть
        while (!elementData.empty() && elementData.back() == ' ') {
            elementData.pop_back();
        }
        file << elementData << "\n";
    }

    file.close();
    return true;
}

void ShapeContainer::loadFromString(const std::string& data)
{
    clear();  // Очищаем текущие элементы

    std::istringstream iss(data);
    int elementCount;
    iss >> elementCount;

    // Пропускаем оставшуюся часть строки (символ новой строки)
    iss.ignore(1, '\n');

    // Загружаем каждый элемент
    for (int i = 0; i < elementCount; ++i) {
        int dataLength;
        iss >> dataLength;

        // Пропускаем оставшуюся часть строки (символ новой строки)
        iss.ignore(1, '\n');

        // Читаем данные элемента
        std::string elementData(dataLength, ' ');
        iss.read(&elementData[0], dataLength);

        // Создаем элемент с помощью фабрики
        CompositeElement* element = ShapeFactory::createFromString(elementData);
        if (element) {
            elements_.push_back(element);
        }

        // Пропускаем оставшийся символ новой строки, если есть
        if (iss.peek() == '\n') {
            iss.ignore(1, '\n');
        }
    }
}

bool ShapeContainer::loadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return false;
    }

    clear();  // Очищаем текущие элементы

    std::string line;
    int lineNumber = 0;

    // Читаем первую строку - количество элементов
    if (!std::getline(file, line)) {
        std::cerr << "Empty file" << std::endl;
        return false;
    }

    int elementCount = std::stoi(line);
    std::cout << "Loading " << elementCount << " elements" << std::endl;

    // Загружаем каждый элемент
    for (int i = 0; i < elementCount; ++i) {
        if (!std::getline(file, line)) {
            std::cerr << "Unexpected end of file at element " << i << std::endl;
            break;
        }

        // Пропускаем пустые строки
        if (line.empty()) {
            i--; // Не считаем пустую строку за элемент
            continue;
        }

        std::cout << "Loading element " << i << ": " << line << std::endl;

        CompositeElement* element = ShapeFactory::createFromString(line);
        if (element) {
            elements_.push_back(element);
            std::cout << "Successfully created element of type " << element->getTypeName() << std::endl;
        } else {
            std::cerr << "Failed to create element from line: " << line << std::endl;
        }
    }

    file.close();
    std::cout << "Loaded " << elements_.size() << " elements" << std::endl;
    return true;
}
