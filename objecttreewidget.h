#ifndef OBJECTTREEWIDGET_H
#define OBJECTTREEWIDGET_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "shapecontainer.h"

class Arrow;

class ObjectTreeWidget : public QTreeWidget {
    Q_OBJECT

private:
    ShapeContainer* container_;
    bool ignoreSelection_;

public:
    explicit ObjectTreeWidget(QWidget* parent = nullptr);
    void setContainer(ShapeContainer* container);
    void rebuildTree();
    void syncSelectionFromContainer();

protected:
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void onItemSelectionChanged();
};

#endif
