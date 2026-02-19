#ifndef GROUP_H
#define GROUP_H

#include "composite.h"
#include <vector>
#include <algorithm>

class Group : public CompositeElement
{
private:
    std::vector<CompositeElement*> children_;
    bool selected_;
    QColor color_; // Цвет группы (для выделения)
    void clearChildren();

public:
    Group();
    ~Group();

    void draw(QPainter &painter) const override;
    bool contains(int x, int y) const override;
    QRect getBorderRect() const override;
    QRect getSafeBorderRect(int margin = 0) const override;

    void move(int dx, int dy) override;
    bool checkBounds(int left, int top, int right, int bottom) const override;
    bool safeMove(int dx, int dy, int left, int top, int right, int bottom) override;

    bool canChangeSize(int left, int top, int right, int bottom, int margin = 0) const override;

    bool getSelected() const override;
    void setSelected(bool selected) override;

    QColor getColor() const override;
    void setColor(const QColor& color) override;

    int getX() const override;
    int getY() const override;
    void setPosition(int x, int y) override;

    void setPositionRelative(int dx, int dy) override;

    // Методы CompositeElement
    void addChild(CompositeElement* child) override;
    void removeChild(CompositeElement* child) override;
    const std::vector<CompositeElement*>& getChildren() const override;
    bool isGroup() const override { return true; }

    // Специальные методы для Group
    bool isEmpty() const { return children_.empty(); }
    int getChildCount() const { return children_.size(); }

    std::string getTypeName() const override { return "Group"; }

    // Методы Serializable
    std::string save() const override;
    void load(const std::string& data) override;

    // Методы для сохранения/загрузки детей
    void saveChildren(std::ostream& os) const override;
    void loadChildren(std::istream& is) override;
};

#endif // GROUP_H
