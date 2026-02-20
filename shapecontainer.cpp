#include "shapecontainer.h"
#include "shapefactory.h"
#include "group.h"
#include <fstream>
#include <sstream>
#include <iostream>

ShapeContainer::ShapeContainer() {}

ShapeContainer::~ShapeContainer() {
    clear();
}

void ShapeContainer::addElement(CompositeElement* element) {
    if (element != nullptr) {
        elements_.push_back(element);
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
    notifyObservers("container_changed");
}

void ShapeContainer::clearSelection() {
    for (auto element : elements_) {
        element->setSelected(false);
    }
    notifyObservers("selection_changed");
}

void ShapeContainer::removeSelected() {
    for (int i = (int)elements_.size() - 1; i >= 0; i--) {
        if (elements_[i]->getSelected()) {
            delete elements_[i];
            elements_.erase(elements_.begin() + i);
        }
    }
    notifyObservers("element_removed");
}

void ShapeContainer::selectAll() {
    for (auto element : elements_) {
        element->setSelected(true);
    }
    notifyObservers("selection_changed");
}

void ShapeContainer::notifySelectionChanged() {
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
        return;
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
    notifyObservers("container_changed");
}

void ShapeContainer::ungroupSelected() {
    std::vector<CompositeElement*> selected = getSelectedElements();

    for (auto element : selected) {
        if (element->isGroup()) {
            Group* group = dynamic_cast<Group*>(element);
            if (group) {
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
                        break;
                    }
                }
            }
        }
    }
    notifyObservers("container_changed");
}

void ShapeContainer::moveSelected(int dx, int dy, int maxX, int maxY, int topMargin) {
    int left = 0;
    int top = topMargin;
    int right = maxX;
    int bottom = maxY;

    for (auto element : elements_) {
        if (element->getSelected()) {
            element->safeMove(dx, dy, left, top, right, bottom);
        }
    }
    notifyObservers("elements_moved");
}

void ShapeContainer::setSelectedColor(const QColor &color) {
    std::vector<CompositeElement*> allSelected;
    for (auto element : elements_) {
        if (element->getSelected()) {
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
