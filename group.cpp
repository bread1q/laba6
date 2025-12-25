#include "group.h"
#include <QPainter>
#include <algorithm>

Group::Group() : selected_(false), color_(Qt::gray), boundsValid_(false) {}

Group::~Group()
{
    for (auto child : children_) {
        delete child;
    }
    children_.clear();
}

void Group::draw(QPainter &painter) const
{
    // Сначала рисуем всех детей
    for (auto child : children_) {
        child->draw(painter);
    }

    // Если группа выделена, рисуем рамку вокруг всей группы
    if (selected_) {
        painter.save();
        QPen pen(Qt::blue, 2, Qt::DashLine);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        QRect bounds = getBorderRect();
        painter.drawRect(bounds);

        // Рисуем угловые маркеры
        painter.setBrush(Qt::blue);
        int markerSize = 6;
        painter.drawRect(bounds.left() - markerSize/2, bounds.top() - markerSize/2, markerSize, markerSize);
        painter.drawRect(bounds.right() - markerSize/2, bounds.top() - markerSize/2, markerSize, markerSize);
        painter.drawRect(bounds.left() - markerSize/2, bounds.bottom() - markerSize/2, markerSize, markerSize);
        painter.drawRect(bounds.right() - markerSize/2, bounds.bottom() - markerSize/2, markerSize, markerSize);

        painter.restore();
    }
}

bool Group::contains(int x, int y) const
{
    // Проверяем, попадает ли точка в границы группы
    return getBorderRect().contains(x, y);
}

QRect Group::getBorderRect() const
{
    if (!boundsValid_) {
        calculateBounds();
    }
    return cachedBounds_;
}

void Group::calculateBounds() const
{
    if (children_.empty()) {
        cachedBounds_ = QRect(0, 0, 0, 0);
    } else {
        QRect bounds = children_[0]->getBorderRect();
        for (size_t i = 1; i < children_.size(); ++i) {
            bounds = bounds.united(children_[i]->getBorderRect());
        }
        cachedBounds_ = bounds;
    }
    boundsValid_ = true;
}

QRect Group::getSafeBorderRect(int margin) const
{
    QRect bounds = getBorderRect();
    return bounds.adjusted(-margin, -margin, margin, margin);
}

void Group::move(int dx, int dy)
{
    // Перемещаем всех детей
    for (auto child : children_) {
        child->move(dx, dy);
    }
    boundsValid_ = false;
}

bool Group::checkBounds(int left, int top, int right, int bottom) const
{
    QRect bounds = getBorderRect();
    return (bounds.left() >= left &&
            bounds.right() <= right &&
            bounds.top() >= top &&
            bounds.bottom() <= bottom);
}

bool Group::safeMove(int dx, int dy, int left, int top, int right, int bottom)
{
    // Сохраняем старые позиции всех детей
    std::vector<std::pair<int, int>> oldPositions;
    for (auto child : children_) {
        oldPositions.emplace_back(child->getX(), child->getY());
    }

    // Пытаемся переместить
    move(dx, dy);

    // Проверяем границы
    if (!checkBounds(left, top, right, bottom)) {
        // Возвращаем на старые позиции
        for (size_t i = 0; i < children_.size(); ++i) {
            children_[i]->setPosition(oldPositions[i].first, oldPositions[i].second);
        }
        boundsValid_ = false;
        return false;
    }

    return true;
}

bool Group::canChangeSize(int left, int top, int right, int bottom, int margin) const
{
    return checkBounds(left, top, right, bottom);
}

bool Group::getSelected() const
{
    return selected_;
}

void Group::setSelected(bool selected)
{
    selected_ = selected;
    // Также выделяем/снимаем выделение со всех детей
    for (auto child : children_) {
        child->setSelected(selected);
    }
}

QColor Group::getColor() const
{
    return color_;
}

void Group::setColor(const QColor& color)
{
    color_ = color;
    // Устанавливаем цвет всем детям
    for (auto child : children_) {
        child->setColor(color);
    }
}

int Group::getX() const
{
    QRect bounds = getBorderRect();
    return bounds.center().x();
}

int Group::getY() const
{
    QRect bounds = getBorderRect();
    return bounds.center().y();
}

void Group::setPosition(int x, int y)
{
    QRect bounds = getBorderRect();
    int dx = x - bounds.center().x();
    int dy = y - bounds.center().y();
    move(dx, dy);
}

void Group::setPositionRelative(int dx, int dy)
{
    move(dx, dy);
}

void Group::addChild(CompositeElement* child)
{
    if (child) {
        children_.push_back(child);
        // Сохраняем относительное смещение
        QRect bounds = getBorderRect();
        QRect childBounds = child->getBorderRect();
        childOffsets_.emplace_back(childBounds.center().x() - bounds.center().x(),
                                   childBounds.center().y() - bounds.center().y());
        boundsValid_ = false;
    }
}

void Group::removeChild(CompositeElement* child)
{
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        size_t index = it - children_.begin();
        children_.erase(it);
        childOffsets_.erase(childOffsets_.begin() + index);
        boundsValid_ = false;
    }
}

const std::vector<CompositeElement*>& Group::getChildren() const
{
    return children_;
}

void Group::updateChildOffsets()
{
    childOffsets_.clear();
    QRect bounds = getBorderRect();
    for (auto child : children_) {
        QRect childBounds = child->getBorderRect();
        childOffsets_.emplace_back(childBounds.center().x() - bounds.center().x(),
                                   childBounds.center().y() - bounds.center().y());
    }
}
