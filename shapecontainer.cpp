#include "shapecontainer.h"
#include "group.h"
#include "line.h"
#include <QPainter>

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

void ShapeContainer::collectAllElements(CompositeElement* element, std::vector<CompositeElement*>& result) const {
    result.push_back(element);
    if (element->isGroup()) {
        const std::vector<CompositeElement*>& children = element->getChildren();
        for (auto child : children) {
            collectAllElements(child, result);
        }
    }
}

void ShapeContainer::moveSelected(int dx, int dy, int maxX, int maxY, int topMargin) {
    int left = 0;
    int top = topMargin;
    int right = maxX;
    int bottom = maxY;

    // Собираем все выбранные элементы (включая вложенные в группы)
    std::vector<CompositeElement*> allSelected;
    for (auto element : elements_) {
        if (element->getSelected()) {
            collectAllElements(element, allSelected);
        }
    }

    // Перемещаем каждый выбранный элемент
    for (auto element : allSelected) {
        // Пропускаем группы, так как они будут перемещены через своих родителей
        if (!element->isGroup()) {
            int oldX = element->getX();
            int oldY = element->getY();

            // Перемещаем
            element->move(dx, dy);

            // Проверяем границы
            QRect newBounds = element->getBorderRect();
            if (newBounds.left() < left || newBounds.right() > right ||
                newBounds.top() < top || newBounds.bottom() > bottom) {
                // Возвращаем на старую позицию
                element->setPosition(oldX, oldY);
            }
        }
    }

    // Также перемещаем группы (они перемещают своих детей)
    for (auto element : elements_) {
        if (element->getSelected() && element->isGroup()) {
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
