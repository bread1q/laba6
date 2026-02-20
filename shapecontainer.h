#ifndef SHAPECONTAINER_H
#define SHAPECONTAINER_H

#include <vector>
#include "composite.h"
#include "observer.h"

class ShapeContainer : public Observable
{
private:
    std::vector<CompositeElement*> elements_;
    bool ignoreSelectionNotifications_;

public:
    ShapeContainer();
    ~ShapeContainer();

    void addElement(CompositeElement* element);
    void removeElement(int i);
    void clear();

    void clearSelection();
    void removeSelected();
    void selectAll();
    void notifySelectionChanged();  // ДОЛЖЕН БЫТЬ ТОЛЬКО ОДИН РАЗ!

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

    void setIgnoreSelectionNotifications(bool ignore);
    bool isIgnoringSelectionNotifications() const;

private:
    void collectAllElements(CompositeElement* element, std::vector<CompositeElement*>& result) const;
    void collectNonGroupElements(CompositeElement* element, std::vector<CompositeElement*>& result) const;
};

#endif // SHAPECONTAINER_H
