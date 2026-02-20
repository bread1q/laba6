#include "arrow.h"
#include <QDebug>

Arrow::Arrow(CompositeElement* source, CompositeElement* target, bool bidirectional)
    : source_(source), target_(target), selected_(false), bidirectional_(bidirectional) {

    if (source_) {
        // Подписываемся на перемещение source
        // В реальном проекте нужно, чтобы CompositeElement был Observable
    }
    if (target_) {
        // Подписываемся на перемещение target
    }
}

Arrow::~Arrow() {
    // Не удаляем source и target - они принадлежат контейнеру
}

void Arrow::draw(QPainter &painter) const {
    if (!source_ || !target_) return;

    QPoint sourceCenter = getSourceCenter();
    QPoint targetCenter = getTargetCenter();

    painter.save();

    // Настройка пера
    if (selected_) {
        painter.setPen(QPen(Qt::blue, 3, Qt::DashLine));
        painter.setBrush(Qt::blue);
    } else {
        painter.setPen(QPen(Qt::darkGreen, 2));
        painter.setBrush(Qt::darkGreen);
    }

    // Рисуем линию
    painter.drawLine(sourceCenter, targetCenter);

    // Рисуем стрелку (наконечник) от source к target
    drawArrowHead(painter, sourceCenter, targetCenter);

    // Если двунаправленная, рисуем вторую стрелку
    if (bidirectional_) {
        drawArrowHead(painter, targetCenter, sourceCenter);
    }

    painter.restore();
}

bool Arrow::contains(int x, int y) const {
    if (!source_ || !target_) return false;
    return isPointNearLine(x, y, 5);
}

QRect Arrow::getBorderRect() const {
    if (!source_ || !target_) return QRect();

    QPoint sourceCenter = getSourceCenter();
    QPoint targetCenter = getTargetCenter();

    int left = std::min(sourceCenter.x(), targetCenter.x());
    int right = std::max(sourceCenter.x(), targetCenter.x());
    int top = std::min(sourceCenter.y(), targetCenter.y());
    int bottom = std::max(sourceCenter.y(), targetCenter.y());

    return QRect(left - 5, top - 5, right - left + 10, bottom - top + 10);
}

QRect Arrow::getSafeBorderRect(int margin) const {
    QRect bounds = getBorderRect();
    return bounds.adjusted(-margin, -margin, margin, margin);
}

void Arrow::move(int dx, int dy) {
    // Стрелка не перемещается самостоятельно
    // Она следует за объектами
}

bool Arrow::checkBounds(int left, int top, int right, int bottom) const {
    return true; // Стрелка не проверяет границы
}

bool Arrow::safeMove(int dx, int dy, int left, int top, int right, int bottom) {
    return true; // Стрелка не перемещается
}

bool Arrow::canChangeSize(int left, int top, int right, int bottom, int margin) const {
    return false;
}

bool Arrow::getSelected() const {
    return selected_;
}

void Arrow::setSelected(bool selected) {
    selected_ = selected;
}

QColor Arrow::getColor() const {
    return Qt::darkGreen;
}

void Arrow::setColor(const QColor& color) {
    // Стрелка не меняет цвет
}

int Arrow::getX() const {
    if (!source_) return 0;
    return getSourceCenter().x();
}

int Arrow::getY() const {
    if (!source_) return 0;
    return getSourceCenter().y();
}

void Arrow::setPosition(int x, int y) {
    // Стрелка не меняет позицию напрямую
}

void Arrow::setPositionRelative(int dx, int dy) {
    // Стрелка не меняет позицию напрямую
}

const std::vector<CompositeElement*>& Arrow::getChildren() const {
    static const std::vector<CompositeElement*> empty;
    return empty;
}

void Arrow::update(const std::string& eventType, void* data) {
    if (eventType == "moved" || eventType == "element_changed") {
        // При перемещении source или target нужно перерисовать стрелку
        // В реальном проекте здесь будет уведомление контейнера
    }
}

std::string Arrow::save() const {
    std::ostringstream oss;
    oss << "Arrow ";
    // Сохраняем указатели как адреса (для простоты)
    oss << reinterpret_cast<uintptr_t>(source_) << " "
        << reinterpret_cast<uintptr_t>(target_) << " "
        << (bidirectional_ ? "1" : "0") << " "
        << (selected_ ? "1" : "0");
    return oss.str();
}

void Arrow::load(const std::string& data) {
    std::istringstream iss(data);
    std::string type;
    uintptr_t sourceAddr, targetAddr;
    int bidir, selected;

    iss >> type >> sourceAddr >> targetAddr >> bidir >> selected;

    bidirectional_ = (bidir != 0);
    selected_ = (selected != 0);

    // Адреса будут восстановлены позже
}

QPoint Arrow::getSourceCenter() const {
    if (!source_) return QPoint(0, 0);
    QRect bounds = source_->getBorderRect();
    return bounds.center();
}

QPoint Arrow::getTargetCenter() const {
    if (!target_) return QPoint(0, 0);
    QRect bounds = target_->getBorderRect();
    return bounds.center();
}

void Arrow::drawArrowHead(QPainter& painter, const QPoint& from, const QPoint& to) const {
    const int arrowSize = 10;

    double angle = std::atan2(to.y() - from.y(), to.x() - from.x());

    QPoint arrowP1 = to - QPoint(arrowSize * std::cos(angle - M_PI/6),
                                 arrowSize * std::sin(angle - M_PI/6));
    QPoint arrowP2 = to - QPoint(arrowSize * std::cos(angle + M_PI/6),
                                 arrowSize * std::sin(angle + M_PI/6));

    QPolygon arrowHead;
    arrowHead << to << arrowP1 << arrowP2;

    painter.drawPolygon(arrowHead);
}

bool Arrow::isPointNearLine(int px, int py, int threshold) const {
    QPoint p1 = getSourceCenter();
    QPoint p2 = getTargetCenter();

    double dx = p2.x() - p1.x();
    double dy = p2.y() - p1.y();

    if (dx == 0 && dy == 0) return false;

    double t = ((px - p1.x()) * dx + (py - p1.y()) * dy) / (dx*dx + dy*dy);

    if (t < 0) t = 0;
    if (t > 1) t = 1;

    double projX = p1.x() + t * dx;
    double projY = p1.y() + t * dy;

    double dist = std::sqrt((px - projX)*(px - projX) + (py - projY)*(py - projY));

    return dist <= threshold;
}
