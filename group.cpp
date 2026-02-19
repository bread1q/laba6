#include "group.h"
#include "shapefactory.h"
#include <QPainter>
#include <algorithm>
#include <sstream>

Group::Group() : selected_(false), color_(Qt::gray) {}

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
    // ВСЕГДА вычисляем границы заново
    if (children_.empty()) {
        return QRect(0, 0, 0, 0);
    }

    QRect bounds = children_[0]->getBorderRect();
    for (size_t i = 1; i < children_.size(); ++i) {
        bounds = bounds.united(children_[i]->getBorderRect());
    }
    return bounds;
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
}

bool Group::checkBounds(int left, int top, int right, int bottom) const
{
    // Проверяем, что ВСЕ дети находятся в пределах границ
    for (auto child : children_) {
        if (!child->checkBounds(left, top, right, bottom)) {
            return false;
        }
    }
    return true;
}

bool Group::safeMove(int dx, int dy, int left, int top, int right, int bottom)
{
    // Сохраняем старые позиции всех детей
    std::vector<std::pair<int, int>> oldPositions;

    for (auto child : children_) {
        oldPositions.emplace_back(child->getX(), child->getY());
    }

    // Пытаемся переместить всех детей
    for (auto child : children_) {
        child->move(dx, dy);
    }

    // Проверяем границы для ВСЕХ детей
    bool allInBounds = checkBounds(left, top, right, bottom);

    if (!allInBounds) {
        // Возвращаем всех детей на старые позиции
        for (size_t i = 0; i < children_.size(); ++i) {
            children_[i]->setPosition(oldPositions[i].first, oldPositions[i].second);
        }
        return false;
    }

    return true;
}

bool Group::canChangeSize(int left, int top, int right, int bottom, int margin) const
{
    // Группы не могут менять размер напрямую
    return false;
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
    }
}

void Group::removeChild(CompositeElement* child)
{
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        children_.erase(it);
    }
}

const std::vector<CompositeElement*>& Group::getChildren() const
{
    return children_;
}

// Методы сохранения/загрузки для Group
std::string Group::save() const
{
    std::ostringstream oss;

    // Сохраняем основные свойства группы
    oss << getTypeName() << " "
        << children_.size() << " "
        << selected_ << " "
        << color_.red() << " "
        << color_.green() << " "
        << color_.blue() << " "
        << color_.alpha() << " ";

    // Сохраняем детей
    saveChildren(oss);

    return oss.str();
}

void Group::load(const std::string& data)
{
    std::istringstream iss(data);
    std::string type;
    int childCount;

    iss >> type >> childCount >> selected_;

    int r, g, b, a;
    iss >> r >> g >> b >> a;
    color_ = QColor(r, g, b, a);

    // Очищаем текущих детей
    clearChildren();

    // Загружаем детей
    loadChildren(iss);
}

void Group::saveChildren(std::ostream& os) const
{
    for (auto child : children_) {
        // Сохраняем каждый child
        std::string childData = child->save();
        os << childData.length() << " ";  // Длина данных ребенка
        os << childData << " ";           // Данные ребенка
    }
}

void Group::loadChildren(std::istream& is)
{
    // Теперь используем фабрику для создания детей
    for (size_t i = 0; i < children_.size(); ++i) {
        int dataLength;
        is >> dataLength;

        // Читаем данные ребенка
        std::string childData(dataLength, ' ');
        is.read(&childData[0], dataLength);

        // Создаем ребенка с помощью фабрики
        CompositeElement* child = ShapeFactory::createFromString(childData);
        if (child) {
            // Заменяем существующего ребенка
            delete children_[i];
            children_[i] = child;
        }
    }
}

void Group::clearChildren()
{
    for (auto child : children_) {
        delete child;
    }
    children_.clear();
}

