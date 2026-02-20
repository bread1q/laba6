#ifndef SHAPECONTAINER_H
#define SHAPECONTAINER_H

#include <vector>
#include "composite.h"
#include "observer.h"

// Предварительное объявление класса Arrow
class Arrow;

class ShapeContainer : public Observable
{
private:
    std::vector<CompositeElement*> elements_;
    std::vector<Arrow*> arrows_;
    CompositeElement* arrowSource_;

    void removeArrowsWithElement(CompositeElement* element);  // Добавить эту строку

public:
    ShapeContainer();
    ~ShapeContainer();

    void addElement(CompositeElement* element);
    void removeElement(int i);
    void clear();

    void clearSelection();
    void removeSelected();
    void selectAll();
    void notifySelectionChanged();

    CompositeElement* getElement(int i) const;
    int getCount() const;

    std::vector<CompositeElement*> getSelectedElements() const;
    bool hasSelectedElements() const;
    int getSelectedCount() const;

    void groupSelected();
    void ungroupSelected();

    void moveSelected(int dx, int dy, int maxX, int maxY, int topMargin);
    void setSelectedColor(const QColor &color);

    std::string saveToString() const;
    bool saveToFile(const std::string& filename) const;

    void loadFromString(const std::string& data);
    bool loadFromFile(const std::string& filename);

    // Методы для работы со стрелками
    void addArrow(CompositeElement* source, CompositeElement* target, bool bidirectional = false);
    void removeArrow(Arrow* arrow);
    void removeSelectedArrows();
    std::vector<Arrow*> getArrows() const { return arrows_; }
    void drawArrows(QPainter& painter) const;

    void setArrowSource(CompositeElement* source) { arrowSource_ = source; }
    CompositeElement* getArrowSource() const { return arrowSource_; }
    void clearArrowSource() { arrowSource_ = nullptr; }

    CompositeElement* findElementAt(int x, int y, bool includeArrows = true);

private:
    void collectAllElements(CompositeElement* element, std::vector<CompositeElement*>& result) const;
    void collectNonGroupElements(CompositeElement* element, std::vector<CompositeElement*>& result) const;
};

#endif // SHAPECONTAINER_H
