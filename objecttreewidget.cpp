#include "objecttreewidget.h"
#include "mainwindow.h"
#include "arrow.h"
#include <QMouseEvent>

ObjectTreeWidget::ObjectTreeWidget(QWidget* parent)
    : QTreeWidget(parent), container_(nullptr), ignoreSelection_(false) {
    setHeaderLabel("Объекты");
    setMinimumWidth(200);
    setMaximumWidth(300);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(this, &QTreeWidget::itemSelectionChanged,
            this, &ObjectTreeWidget::onItemSelectionChanged);
}

void ObjectTreeWidget::setContainer(ShapeContainer* container) {
    container_ = container;
    rebuildTree();
}

void ObjectTreeWidget::rebuildTree() {
    clear();
    if (!container_) return;

    for (int i = 0; i < container_->getCount(); ++i) {
        CompositeElement* element = container_->getElement(i);
        if (!element) continue;

        QString text;
        if (element->isGroup()) {
            text = "Группа";
        } else {
            if (auto adapter = dynamic_cast<ShapeAdapter*>(element)) {
                text = QString::fromStdString(adapter->getTypeName());
            } else {
                text = "Объект";
            }
        }

        QTreeWidgetItem* item = new QTreeWidgetItem(this);
        item->setText(0, text);
        item->setData(0, Qt::UserRole, i);
    }
    expandAll();
    syncSelectionFromContainer();
}

void ObjectTreeWidget::syncSelectionFromContainer() {
    if (!container_ || ignoreSelection_) return;

    ignoreSelection_ = true;

    for (int i = 0; i < container_->getCount(); ++i) {
        CompositeElement* element = container_->getElement(i);
        if (!element) continue;

        // Ищем соответствующий элемент в дереве
        for (int j = 0; j < topLevelItemCount(); ++j) {
            QTreeWidgetItem* item = topLevelItem(j);
            if (item->data(0, Qt::UserRole).toInt() == i) {
                if (element->getSelected()) {
                    item->setSelected(true);
                    item->setForeground(0, QBrush(Qt::blue));
                } else {
                    item->setSelected(false);
                    item->setForeground(0, QBrush(Qt::black));
                }
                break;
            }
        }
    }

    ignoreSelection_ = false;
}

void ObjectTreeWidget::onItemSelectionChanged() {
    if (!container_ || ignoreSelection_) return;

    ignoreSelection_ = true;

    // Снимаем выделение со всех объектов
    for (int i = 0; i < container_->getCount(); ++i) {
        if (auto e = container_->getElement(i)) {
            e->setSelected(false);
        }
    }

    // Выделяем выбранные в дереве
    for (auto* item : selectedItems()) {
        int index = item->data(0, Qt::UserRole).toInt();
        if (auto e = container_->getElement(index)) {
            e->setSelected(true);
        }
    }

    ignoreSelection_ = false;

    // Обновляем отображение
    if (auto* main = qobject_cast<MainWindow*>(window())) {
        main->update();
    }
}

void ObjectTreeWidget::mousePressEvent(QMouseEvent* event) {
    QTreeWidget::mousePressEvent(event);
}
