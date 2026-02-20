#include "shapecontainer.h"
#include "shapefactory.h"
#include "group.h"
#include "arrow.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <QDebug>

ShapeContainer::ShapeContainer() : arrowSource_(nullptr) {}

ShapeContainer::~ShapeContainer() {
    clear();
}

void ShapeContainer::addElement(CompositeElement* element) {
    if (element != nullptr) {
        elements_.push_back(element);
        qDebug() << "ShapeContainer::addElement - notifying";
        notifyObservers("element_added", element);
    }
}

void ShapeContainer::removeElement(int i) {
    if (i >= 0 && i < (int)elements_.size()) {
        delete elements_[i];
        elements_.erase(elements_.begin() + i);
        notifyObservers("element_removed");
    }
}

void ShapeContainer::clear() {
    for (auto element : elements_) {
        delete element;
    }
    elements_.clear();

    for (auto arrow : arrows_) {
        delete arrow;
    }
    arrows_.clear();

    notifyObservers("container_changed");
}

void ShapeContainer::clearSelection() {
    qDebug() << "ShapeContainer::clearSelection";

    std::vector<CompositeElement*> toClear;
    for (auto element : elements_) {
        if (element && element->getSelected()) {
            toClear.push_back(element);
        }
    }

    for (auto element : toClear) {
        element->setSelected(false);
    }

    for (auto arrow : arrows_) {
        if (arrow && arrow->getSelected()) {
            arrow->setSelected(false);
        }
    }

    if (!toClear.empty()) {
        notifyObservers("selection_changed");
    }
}

void ShapeContainer::removeSelected() {
    qDebug() << "\n=== removeSelected ===";
    qDebug() << "Elements before:" << elements_.size();
    for (size_t i = 0; i < elements_.size(); i++) {
        qDebug() << "  Element" << i << ":" << elements_[i] << "selected:" << elements_[i]->getSelected();
    }

    qDebug() << "Arrows before:" << arrows_.size();
    for (size_t i = 0; i < arrows_.size(); i++) {
        qDebug() << "  Arrow" << i << ":" << arrows_[i]
                 << "source:" << arrows_[i]->getSource()
                 << "target:" << arrows_[i]->getTarget()
                 << "selected:" << arrows_[i]->getSelected();
    }

    // Сначала собираем все элементы для удаления
    std::vector<CompositeElement*> toDelete;
    for (auto element : elements_) {
        if (element && element->getSelected()) {
            toDelete.push_back(element);
            qDebug() << "  Will delete element:" << element;
        }
    }

    if (toDelete.empty()) {
        qDebug() << "No elements to delete";
    }

    // Удаляем все стрелки, связанные с этими элементами
    qDebug() << "Checking arrows for deletion...";
    for (int i = arrows_.size() - 1; i >= 0; i--) {
        Arrow* arrow = arrows_[i];
        bool shouldDelete = false;

        qDebug() << "  Arrow" << i << "source:" << arrow->getSource() << "target:" << arrow->getTarget();

        for (auto element : toDelete) {
            if (arrow->getSource() == element) {
                qDebug() << "    Source matches element" << element;
                shouldDelete = true;
                break;
            }
            if (arrow->getTarget() == element) {
                qDebug() << "    Target matches element" << element;
                shouldDelete = true;
                break;
            }
        }

        if (shouldDelete) {
            qDebug() << "  DELETING arrow" << i;
            delete arrow;
            arrows_.erase(arrows_.begin() + i);
        } else {
            qDebug() << "  Keeping arrow" << i;
        }
    }

    // Удаляем выбранные стрелки
    qDebug() << "Checking for selected arrows...";
    for (int i = arrows_.size() - 1; i >= 0; i--) {
        if (arrows_[i]->getSelected()) {
            qDebug() << "Deleting selected arrow:" << arrows_[i];
            delete arrows_[i];
            arrows_.erase(arrows_.begin() + i);
        }
    }

    // Удаляем элементы
    qDebug() << "Deleting elements...";
    for (auto element : toDelete) {
        for (int i = elements_.size() - 1; i >= 0; i--) {
            if (elements_[i] == element) {
                qDebug() << "Deleting element:" << element;
                delete elements_[i];
                elements_.erase(elements_.begin() + i);
                break;
            }
        }
    }

    qDebug() << "Elements after:" << elements_.size();
    qDebug() << "Arrows after:" << arrows_.size();
    for (size_t i = 0; i < arrows_.size(); i++) {
        qDebug() << "  Arrow" << i << ":" << arrows_[i]
                 << "source:" << arrows_[i]->getSource()
                 << "target:" << arrows_[i]->getTarget();
    }
    qDebug() << "=== removeSelected finished ===\n";

    notifyObservers("element_removed");
}

void ShapeContainer::selectAll() {
    qDebug() << "ShapeContainer::selectAll";
    for (auto element : elements_) {
        element->setSelected(true);
    }
    for (auto arrow : arrows_) {
        arrow->setSelected(true);
    }
    notifyObservers("selection_changed");
}

void ShapeContainer::notifySelectionChanged() {
    qDebug() << "CONTAINER: notifySelectionChanged()";
    notifyObservers("selection_changed");
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
        if (element && element->getSelected()) {
            selected.push_back(element);
        }
    }
    return selected;
}

bool ShapeContainer::hasSelectedElements() const {
    for (auto element : elements_) {
        if (element && element->getSelected()) {
            return true;
        }
    }
    return false;
}

int ShapeContainer::getSelectedCount() const {
    int count = 0;
    for (auto element : elements_) {
        if (element && element->getSelected()) {
            count++;
        }
    }
    return count;
}

void ShapeContainer::groupSelected() {
    std::vector<CompositeElement*> selected = getSelectedElements();

    if (selected.size() < 2) {
        return;
    }

    qDebug() << "=== groupSelected ===";
    qDebug() << "Selected elements:" << selected.size();

    // Сохраняем все стрелки, которые связаны с выбранными элементами
    std::vector<Arrow*> arrowsToRemove;
    std::vector<std::tuple<CompositeElement*, CompositeElement*, bool>> arrowsToRecreate;

    for (auto arrow : arrows_) {
        CompositeElement* source = arrow->getSource();
        CompositeElement* target = arrow->getTarget();

        bool sourceInGroup = (std::find(selected.begin(), selected.end(), source) != selected.end());
        bool targetInGroup = (std::find(selected.begin(), selected.end(), target) != selected.end());

        // Если оба конца стрелки попадают в группу - стрелка останется внутри группы
        if (sourceInGroup && targetInGroup) {
            qDebug() << "Arrow both ends in group - will be inside group";
            arrowsToRemove.push_back(arrow);
            // Внутри группы стрелка сохранится, но мы ее потом пересоздадим
            arrowsToRecreate.push_back({source, target, arrow->isBidirectional()});
        }
        // Если только один конец в группе - стрелка будет вести от/к группе
        else if (sourceInGroup || targetInGroup) {
            qDebug() << "Arrow one end in group - will be recreated with group";
            arrowsToRemove.push_back(arrow);
            arrowsToRecreate.push_back({source, target, arrow->isBidirectional()});
        }
    }

    // Удаляем старые стрелки
    for (auto arrow : arrowsToRemove) {
        auto it = std::find(arrows_.begin(), arrows_.end(), arrow);
        if (it != arrows_.end()) {
            delete *it;
            arrows_.erase(it);
        }
    }

    Group* newGroup = new Group();

    for (auto element : selected) {
        for (int i = (int)elements_.size() - 1; i >= 0; i--) {
            if (elements_[i] == element) {
                newGroup->addChild(element);
                elements_.erase(elements_.begin() + i);
                break;
            }
        }
    }

    elements_.push_back(newGroup);
    newGroup->setSelected(true);

    // Восстанавливаем стрелки
    for (auto& [source, target, bidirectional] : arrowsToRecreate) {
        // Проверяем, где теперь находятся source и target
        CompositeElement* newSource = source;
        CompositeElement* newTarget = target;

        // Если source был в группе, заменяем его на группу
        if (std::find(selected.begin(), selected.end(), source) != selected.end()) {
            newSource = newGroup;
        }
        // Если target был в группе, заменяем его на группу
        if (std::find(selected.begin(), selected.end(), target) != selected.end()) {
            newTarget = newGroup;
        }

        qDebug() << "Recreating arrow from" << newSource << "to" << newTarget;
        addArrow(newSource, newTarget, bidirectional);
    }

    qDebug() << "=== groupSelected finished ===";

    notifyObservers("container_changed");
}

void ShapeContainer::ungroupSelected() {
    std::vector<CompositeElement*> selected = getSelectedElements();
    bool changed = false;

    for (auto element : selected) {
        if (element && element->isGroup()) {
            Group* group = dynamic_cast<Group*>(element);
            if (group) {
                // Удаляем все стрелки, связанные с этой группой
                removeArrowsWithElement(group);

                const std::vector<CompositeElement*>& children = group->getChildren();

                for (auto child : children) {
                    child->setSelected(element->getSelected());
                    elements_.push_back(child);
                }

                for (int i = (int)elements_.size() - 1; i >= 0; i--) {
                    if (elements_[i] == element) {
                        const_cast<std::vector<CompositeElement*>&>(group->getChildren()).clear();
                        delete elements_[i];
                        elements_.erase(elements_.begin() + i);
                        changed = true;
                        break;
                    }
                }
            }
        }
    }

    if (changed) {
        notifyObservers("container_changed");
    }
}

void ShapeContainer::moveSelected(int dx, int dy, int maxX, int maxY, int topMargin) {
    int left = 0;
    int top = topMargin;
    int right = maxX;
    int bottom = maxY;

    qDebug() << "=== moveSelected ===";
    qDebug() << "dx:" << dx << "dy:" << dy;

    // Сначала собираем все выбранные элементы
    std::vector<CompositeElement*> selected;
    for (auto element : elements_) {
        if (element && element->getSelected()) {
            selected.push_back(element);
            qDebug() << "Selected element:" << element;
        }
    }

    // Перемещаем выбранные элементы
    for (auto element : selected) {
        element->safeMove(dx, dy, left, top, right, bottom);
    }

    // Теперь обрабатываем стрелки: если переместился source, двигаем target
    for (auto arrow : arrows_) {
        CompositeElement* source = arrow->getSource();
        CompositeElement* target = arrow->getTarget();

        qDebug() << "Arrow source:" << source << "target:" << target;

        // Если source был перемещен (он в selected), двигаем target
        if (std::find(selected.begin(), selected.end(), source) != selected.end()) {
            qDebug() << "Moving target because source moved";
            if (target) {
                target->safeMove(dx, dy, left, top, right, bottom);
            }
        }

        // Если стрелка двунаправленная и переместился target, двигаем source
        if (arrow->isBidirectional()) {
            if (std::find(selected.begin(), selected.end(), target) != selected.end()) {
                qDebug() << "Moving source because target moved (bidirectional)";
                if (source) {
                    source->safeMove(dx, dy, left, top, right, bottom);
                }
            }
        }
    }

    notifyObservers("elements_moved");
}

void ShapeContainer::setSelectedColor(const QColor &color) {
    std::vector<CompositeElement*> allSelected;
    for (auto element : elements_) {
        if (element && element->getSelected()) {
            collectAllElements(element, allSelected);
        }
    }

    for (auto element : allSelected) {
        element->setColor(color);
    }
    notifyObservers("elements_changed");
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
    oss << elements_.size() << "\n";

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

    file << elements_.size() << "\n";

    for (auto element : elements_) {
        std::string elementData = element->save();
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
    clear();

    std::istringstream iss(data);
    int elementCount;
    iss >> elementCount;
    iss.ignore(1, '\n');

    for (int i = 0; i < elementCount; ++i) {
        int dataLength;
        iss >> dataLength;
        iss.ignore(1, '\n');

        std::string elementData(dataLength, ' ');
        iss.read(&elementData[0], dataLength);

        CompositeElement* element = ShapeFactory::createFromString(elementData);
        if (element) {
            elements_.push_back(element);
        }

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

    clear();

    std::string line;
    int lineNumber = 0;

    if (!std::getline(file, line)) {
        std::cerr << "Empty file" << std::endl;
        return false;
    }

    int elementCount = std::stoi(line);
    std::cout << "Loading " << elementCount << " elements" << std::endl;

    for (int i = 0; i < elementCount; ++i) {
        if (!std::getline(file, line)) {
            std::cerr << "Unexpected end of file at element " << i << std::endl;
            break;
        }

        if (line.empty()) {
            i--;
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

// Методы для работы со стрелками
void ShapeContainer::addArrow(CompositeElement* source, CompositeElement* target, bool bidirectional) {
    if (!source || !target || source == target) return;

    Arrow* arrow = new Arrow(source, target, bidirectional);
    arrows_.push_back(arrow);
    notifyObservers("element_added", arrow);
}

void ShapeContainer::removeArrow(Arrow* arrow) {
    auto it = std::find(arrows_.begin(), arrows_.end(), arrow);
    if (it != arrows_.end()) {
        delete *it;
        arrows_.erase(it);
        notifyObservers("element_removed");
    }
}

void ShapeContainer::removeSelectedArrows() {
    for (int i = arrows_.size() - 1; i >= 0; i--) {
        if (arrows_[i]->getSelected()) {
            delete arrows_[i];
            arrows_.erase(arrows_.begin() + i);
        }
    }
    notifyObservers("element_removed");
}

void ShapeContainer::drawArrows(QPainter& painter) const {
    for (auto arrow : arrows_) {
        arrow->draw(painter);
    }
}

CompositeElement* ShapeContainer::findElementAt(int x, int y, bool includeArrows) {
    // Сначала проверяем стрелки (они обычно тоньше)
    if (includeArrows) {
        for (int i = arrows_.size() - 1; i >= 0; i--) {
            if (arrows_[i]->contains(x, y)) {
                return arrows_[i];
            }
        }
    }

    // Потом проверяем обычные элементы
    for (int i = elements_.size() - 1; i >= 0; i--) {
        if (elements_[i]->contains(x, y)) {
            return elements_[i];
        }
    }

    return nullptr;
}

void ShapeContainer::removeArrowsWithElement(CompositeElement* element) {
    qDebug() << "removeArrowsWithElement for" << element;

    for (int i = arrows_.size() - 1; i >= 0; i--) {
        Arrow* arrow = arrows_[i];
        if (arrow->getSource() == element || arrow->getTarget() == element) {
            qDebug() << "  Removing arrow" << arrow;
            delete arrow;
            arrows_.erase(arrows_.begin() + i);
        }
    }
}
