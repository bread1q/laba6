#ifndef COMPOSITE_H
#define COMPOSITE_H

#include "shape.h"
#include "serializable.h"
#include <vector>
#include <memory>

// Базовый класс для элементов композиции
class CompositeElement : public Serializable
{
public:
    virtual ~CompositeElement() = default;

    virtual void draw(QPainter &painter) const = 0;
    virtual bool contains(int x, int y) const = 0;
    virtual QRect getBorderRect() const = 0;
    virtual QRect getSafeBorderRect(int margin = 0) const = 0;

    virtual void move(int dx, int dy) = 0;
    virtual bool checkBounds(int left, int top, int right, int bottom) const = 0;
    virtual bool safeMove(int dx, int dy, int left, int top, int right, int bottom) = 0;

    virtual bool canChangeSize(int left, int top, int right, int bottom, int margin = 0) const = 0;

    virtual bool getSelected() const = 0;
    virtual void setSelected(bool selected) = 0;

    virtual QColor getColor() const = 0;
    virtual void setColor(const QColor& color) = 0;

    virtual int getX() const = 0;
    virtual int getY() const = 0;
    virtual void setPosition(int x, int y) = 0;

    virtual void setPositionRelative(int dx, int dy) = 0;

    // Методы для работы с композитом
    virtual void addChild(CompositeElement* child) { Q_UNUSED(child); }
    virtual void removeChild(CompositeElement* child) { Q_UNUSED(child); }
    virtual const std::vector<CompositeElement*>& getChildren() const = 0;
    virtual bool isGroup() const { return false; }

    // Методы для сохранения/загрузки (будем добавлять позже)
    virtual std::string getTypeName() const = 0;

    // В класс CompositeElement добавляем:
    virtual void scale(double factor) { Q_UNUSED(factor); }
    virtual bool canScale(double factor, int left, int top, int right, int bottom) const {
        Q_UNUSED(factor); Q_UNUSED(left); Q_UNUSED(top); Q_UNUSED(right); Q_UNUSED(bottom);
        return false;
    }
    virtual bool safeScale(double factor, int left, int top, int right, int bottom) {
        Q_UNUSED(factor); Q_UNUSED(left); Q_UNUSED(top); Q_UNUSED(right); Q_UNUSED(bottom);
        return false;
    }

    virtual void saveChildren(std::ostream& os) const;
    virtual void loadChildren(std::istream& is);
};

// Класс-адаптер для существующих Shape
class ShapeAdapter : public CompositeElement
{
private:
    Shape* shape_;

public:
    explicit ShapeAdapter(Shape* shape) : shape_(shape) {}
    ~ShapeAdapter() { delete shape_; }

    void draw(QPainter &painter) const override { shape_->draw(painter); }
    bool contains(int x, int y) const override { return shape_->contains(x, y); }
    QRect getBorderRect() const override { return shape_->getBorderRect(); }
    QRect getSafeBorderRect(int margin = 0) const override { return shape_->getSafeBorderRect(margin); }

    void move(int dx, int dy) override { shape_->move(dx, dy); }
    bool checkBounds(int left, int top, int right, int bottom) const override {
        return shape_->checkBounds(left, top, right, bottom);
    }
    bool safeMove(int dx, int dy, int left, int top, int right, int bottom) override {
        return shape_->safeMove(dx, dy, left, top, right, bottom);
    }

    bool canChangeSize(int left, int top, int right, int bottom, int margin = 0) const override {
        return shape_->canChangeSize(left, top, right, bottom, margin);
    }

    bool getSelected() const override { return shape_->getSelected(); }
    void setSelected(bool selected) override { shape_->setSelected(selected); }

    QColor getColor() const override { return shape_->getColor(); }
    void setColor(const QColor& color) override { shape_->setColor(color); }

    int getX() const override { return shape_->getX(); }
    int getY() const override { return shape_->getY(); }
    void setPosition(int x, int y) override { shape_->setPosition(x, y); }

    void setPositionRelative(int dx, int dy) override { shape_->move(dx, dy); }

    const std::vector<CompositeElement*>& getChildren() const override {
        static const std::vector<CompositeElement*> empty;
        return empty;
    }

    Shape* getShape() const { return shape_; }

    std::string getTypeName() const override;

    // В класс ShapeAdapter добавляем:
    void scale(double factor) override {
        // ShapeAdapter не поддерживает масштабирование напрямую
        // Масштабирование будет обрабатываться через applyResize в MainWindow
    }

    bool canScale(double factor, int left, int top, int right, int bottom) const override {
        Q_UNUSED(factor); Q_UNUSED(left); Q_UNUSED(top); Q_UNUSED(right); Q_UNUSED(bottom);
        return false; // ShapeAdapter не поддерживает масштабирование
    }

    bool safeScale(double factor, int left, int top, int right, int bottom) override {
        Q_UNUSED(factor); Q_UNUSED(left); Q_UNUSED(top); Q_UNUSED(right); Q_UNUSED(bottom);
        return false; // ShapeAdapter не поддерживает масштабирование
    }

    std::string save() const override;
    void load(const std::string& data) override;
};

#endif // COMPOSITE_H
