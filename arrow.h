#ifndef ARROW_H
#define ARROW_H

#include "composite.h"
#include "observer.h"
#include <QPainter>
#include <cmath>

class Arrow : public CompositeElement, public Observer {
private:
    CompositeElement* source_;
    CompositeElement* target_;
    bool selected_;
    bool bidirectional_;

public:
    Arrow(CompositeElement* source, CompositeElement* target, bool bidirectional = false);
    ~Arrow();

    // CompositeElement interface
    void draw(QPainter &painter) const override;
    bool contains(int x, int y) const override;
    QRect getBorderRect() const override;
    QRect getSafeBorderRect(int margin) const override;
    void move(int dx, int dy) override;
    bool checkBounds(int left, int top, int right, int bottom) const override;
    bool safeMove(int dx, int dy, int left, int top, int right, int bottom) override;
    bool canChangeSize(int left, int top, int right, int bottom, int margin) const override;
    bool getSelected() const override;
    void setSelected(bool selected) override;
    QColor getColor() const override;
    void setColor(const QColor& color) override;
    int getX() const override;
    int getY() const override;
    void setPosition(int x, int y) override;
    void setPositionRelative(int dx, int dy) override;
    const std::vector<CompositeElement*>& getChildren() const override;
    bool isGroup() const override { return false; }
    std::string getTypeName() const override { return "Arrow"; }

    // Observer interface
    void update(const std::string& eventType, void* data) override;

    // Arrow specific
    CompositeElement* getSource() const { return source_; }
    CompositeElement* getTarget() const { return target_; }
    bool isBidirectional() const { return bidirectional_; }

    // Serializable
    std::string save() const override;
    void load(const std::string& data) override;

private:
    QPoint getSourceCenter() const;
    QPoint getTargetCenter() const;
    void drawArrowHead(QPainter& painter, const QPoint& from, const QPoint& to) const;
    bool isPointNearLine(int px, int py, int threshold) const;
};

#endif // ARROW_H
