#include "shapecontainer.h"
#include "line.h"

ShapeContainer::ShapeContainer() {}

ShapeContainer::~ShapeContainer() {
    clear();
}

void ShapeContainer::addShape(Shape *shape) {
    if (shape != nullptr) shapes_.push_back(shape);
}

void ShapeContainer::removeShape(int i) {
    if (i >= 0 && i < (int)shapes_.size()) {
        delete shapes_[i];
        shapes_.erase(shapes_.begin() + i);
    }
}

void ShapeContainer::clear() {
    for (Shape* shape : shapes_) {
        delete shape;
    }
    shapes_.clear();
}

void ShapeContainer::clearSelection() {
    for (Shape* shape : shapes_) {
        shape->setSelected(false);
    }
}

void ShapeContainer::removeSelected() {
    for (int i = (int)shapes_.size() - 1; i >= 0; i--) {
        if (shapes_[i]->getSelected()) {
            delete shapes_[i];
            shapes_.erase(shapes_.begin() + i);
        }
    }
}

void ShapeContainer::selectAll() {
    for (Shape* shape : shapes_) {
        shape->setSelected(true);
    }
}

Shape* ShapeContainer::getShape(int i) const {
    if (i >= 0 && i < (int)shapes_.size()) {
        return shapes_[i];
    }
    return nullptr;
}

int ShapeContainer::getCount() const {
    return (int)shapes_.size();
}

void ShapeContainer::moveSelected(int dx, int dy, int maxX, int maxY, int topMargin) {
    int left = 0;
    int top = topMargin;
    int right = maxX;
    int bottom = maxY;

    for (auto* shape : shapes_) {
        if (shape->getSelected()) {
            int oldX = shape->getX();
            int oldY = shape->getY();

            if (Line* line = dynamic_cast<Line*>(shape)) {
                int oldX2 = line->getX2();
                int oldY2 = line->getY2();

                line->move(dx, dy);

                QRect newBounds = line->getBorderRect();
                if (newBounds.left() < left || newBounds.right() > right ||
                    newBounds.top() < top || newBounds.bottom() > bottom) {
                    line->setPosition(oldX, oldY);
                    line->setEndPoint(oldX2, oldY2);
                }
            } else {
                shape->move(dx, dy);

                QRect newBounds = shape->getBorderRect();
                if (newBounds.left() < left || newBounds.right() > right ||
                    newBounds.top() < top || newBounds.bottom() > bottom) {
                    shape->setPosition(oldX, oldY);
                }
            }
        }
    }
}

void ShapeContainer::setSelectedColor(const QColor &color) {
    for (Shape* shape : shapes_) {
        if (shape->getSelected()) {
            shape->setColor(color);
        }
    }
}
