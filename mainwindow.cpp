#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "composite.h"
#include "group.h"
#include "circle.h"
#include "rectangle.h"
#include "square.h"
#include "triangle.h"
#include "line.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <algorithm>
#include <cmath>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QColorDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <fstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentShapeType_(CIRCLE)
{
    ui->setupUi(this);
    setWindowTitle("Визуальный редактор - Круг (1)");
    resize(800, 600);
    setFocusPolicy(Qt::StrongFocus);

    // Создаем сплиттер
    splitter_ = new QSplitter(this);
    setCentralWidget(splitter_);

    // Создаем дерево объектов
    treeWidget_ = new ObjectTreeWidget(splitter_);
    treeWidget_->setContainer(&shapes_);

    // Создаем рабочую область
    QWidget* workArea = new QWidget(splitter_);
    workArea->setMinimumWidth(400);
    workArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    splitter_->addWidget(treeWidget_);
    splitter_->addWidget(workArea);
    splitter_->setSizes(QList<int>() << 200 << 600);

    createMenu();
    createToolBar();
    updateWindowTitle();
}

void MainWindow::createMenu() {
    QMenu *fileMenu = menuBar()->addMenu("Файл");

    QAction *clearAction = new QAction("Очистить окно", this);
    clearAction->setShortcut(QKeySequence::New);
    connect(clearAction, &QAction::triggered, this, &MainWindow::clearWindow);
    fileMenu->addAction(clearAction);

    QAction *exitAction = new QAction("Выход", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);

    QMenu *shapesMenu = menuBar()->addMenu("Фигуры");

    QAction *circleAction = new QAction("Круг", this);
    circleAction->setShortcut(Qt::Key_1);
    connect(circleAction, &QAction::triggered, this, &MainWindow::selectCircle);
    shapesMenu->addAction(circleAction);

    QAction *rectAction = new QAction("Прямоугольник", this);
    rectAction->setShortcut(Qt::Key_2);
    connect(rectAction, &QAction::triggered, this, &MainWindow::selectRectangle);
    shapesMenu->addAction(rectAction);

    QAction *squareAction = new QAction("Квадрат", this);
    squareAction->setShortcut(Qt::Key_3);
    connect(squareAction, &QAction::triggered, this, &MainWindow::selectSquare);
    shapesMenu->addAction(squareAction);

    QAction *triangleAction = new QAction("Треугольник", this);
    triangleAction->setShortcut(Qt::Key_4);
    connect(triangleAction, &QAction::triggered, this, &MainWindow::selectTriangle);
    shapesMenu->addAction(triangleAction);

    QAction *lineAction = new QAction("Линия", this);
    lineAction->setShortcut(Qt::Key_5);
    connect(lineAction, &QAction::triggered, this, &MainWindow::selectLine);
    shapesMenu->addAction(lineAction);

    QMenu *editMenu = menuBar()->addMenu("Правка");

    QAction *deleteAction = new QAction("Удалить", this);
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, &QAction::triggered, [this]() {
        shapes_.removeSelected();
        update();
        treeWidget_->rebuildTree();
    });
    editMenu->addAction(deleteAction);

    QAction *selectAllAction = new QAction("Выделить все", this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    connect(selectAllAction, &QAction::triggered, [this]() {
        shapes_.selectAll();
        update();
        treeWidget_->rebuildTree();
    });
    editMenu->addAction(selectAllAction);

    QAction *groupAction = new QAction("Сгруппировать", this);
    groupAction->setShortcut(Qt::CTRL | Qt::Key_G);
    connect(groupAction, &QAction::triggered, this, &MainWindow::groupSelected);
    editMenu->addAction(groupAction);

    QAction *ungroupAction = new QAction("Разгруппировать", this);
    ungroupAction->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_G);
    connect(ungroupAction, &QAction::triggered, this, &MainWindow::ungroupSelected);
    editMenu->addAction(ungroupAction);

    QAction *increaseSizeAction = new QAction("Увеличить размер", this);
    increaseSizeAction->setShortcut(Qt::CTRL | Qt::Key_Plus);
    connect(increaseSizeAction, &QAction::triggered, this, &MainWindow::increaseSize);
    editMenu->addAction(increaseSizeAction);

    QAction *decreaseSizeAction = new QAction("Уменьшить размер", this);
    decreaseSizeAction->setShortcut(Qt::CTRL | Qt::Key_Minus);
    connect(decreaseSizeAction, &QAction::triggered, this, &MainWindow::decreaseSize);
    editMenu->addAction(decreaseSizeAction);
}

void MainWindow::createToolBar() {
    QToolBar *toolBar = addToolBar("Инструменты");
    toolBar->setMovable(false);

    QAction *circleAction = new QAction("Круг", this);
    circleAction->setToolTip("Круг - клавиша 1");
    connect(circleAction, &QAction::triggered, this, &MainWindow::selectCircle);
    toolBar->addAction(circleAction);

    QAction *rectAction = new QAction("Прямоугольник", this);
    rectAction->setToolTip("Прямоугольник - клавиша 2");
    connect(rectAction, &QAction::triggered, this, &MainWindow::selectRectangle);
    toolBar->addAction(rectAction);

    QAction *squareAction = new QAction("Квадрат", this);
    squareAction->setToolTip("Квадрат - клавиша 3");
    connect(squareAction, &QAction::triggered, this, &MainWindow::selectSquare);
    toolBar->addAction(squareAction);

    QAction *triangleAction = new QAction("Треугольник", this);
    triangleAction->setToolTip("Треугольник - клавиша 4");
    connect(triangleAction, &QAction::triggered, this, &MainWindow::selectTriangle);
    toolBar->addAction(triangleAction);

    QAction *lineAction = new QAction("Линия", this);
    lineAction->setToolTip("Линия - клавиша 5");
    connect(lineAction, &QAction::triggered, this, &MainWindow::selectLine);
    toolBar->addAction(lineAction);

    toolBar->addSeparator();

    QAction *colorAction = new QAction("Цвет", this);
    colorAction->setToolTip("Изменить цвет выделенных фигур");
    connect(colorAction, &QAction::triggered, this, &MainWindow::changeColor);
    toolBar->addAction(colorAction);

    QAction *deleteAction = new QAction("Удалить", this);
    deleteAction->setToolTip("Удалить выделенные фигуры (Delete)");
    connect(deleteAction, &QAction::triggered, [this]() {
        shapes_.removeSelected();
        update();
        treeWidget_->rebuildTree();
    });
    toolBar->addAction(deleteAction);

    QAction *groupAction = new QAction("Группа", this);
    groupAction->setToolTip("Сгруппировать выделенные фигуры (Ctrl+G)");
    connect(groupAction, &QAction::triggered, this, &MainWindow::groupSelected);
    toolBar->addAction(groupAction);

    QAction *ungroupAction = new QAction("Разгруппировать", this);
    ungroupAction->setToolTip("Разгруппировать выделенные группы (Ctrl+Shift+G)");
    connect(ungroupAction, &QAction::triggered, this, &MainWindow::ungroupSelected);
    toolBar->addAction(ungroupAction);

    // Добавляем кнопки для сохранения/загрузки
    QAction *saveAction = new QAction("Сохранить", this);
    saveAction->setToolTip("Сохранить проект");
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveToFile);
    toolBar->addAction(saveAction);

    QAction *loadAction = new QAction("Загрузить", this);
    loadAction->setToolTip("Загрузить проект");
    connect(loadAction, &QAction::triggered, this, &MainWindow::loadFromFile);
    toolBar->addAction(loadAction);

    QAction *clearAction = new QAction("Очистить", this);
    clearAction->setToolTip("Очистить все окно");
    connect(clearAction, &QAction::triggered, this, &MainWindow::clearWindow);
    toolBar->addAction(clearAction);

    toolBar->addSeparator();

    QAction *increaseAction = new QAction("Увеличить", this);
    increaseAction->setToolTip("Увеличить размер фигур (Ctrl++)");
    connect(increaseAction, &QAction::triggered, this, &MainWindow::increaseSize);
    toolBar->addAction(increaseAction);

    QAction *decreaseAction = new QAction("Уменьшить", this);
    decreaseAction->setToolTip("Уменьшить размер фигур (Ctrl+-)");
    connect(decreaseAction, &QAction::triggered, this, &MainWindow::decreaseSize);
    toolBar->addAction(decreaseAction);

    QAction *testAction = new QAction("Тест", this);
    connect(testAction, &QAction::triggered, this, &MainWindow::testSelection);
    toolBar->addAction(testAction);
}

void MainWindow::selectCircle() {
    currentShapeType_ = CIRCLE;
    updateWindowTitle();
}

void MainWindow::selectRectangle() {
    currentShapeType_ = RECTANGLE;
    updateWindowTitle();
}

void MainWindow::selectSquare() {
    currentShapeType_ = SQUARE;
    updateWindowTitle();
}

void MainWindow::selectTriangle() {
    currentShapeType_ = TRIANGLE;
    updateWindowTitle();
}

void MainWindow::selectLine() {
    currentShapeType_ = LINE;
    updateWindowTitle();
}

void MainWindow::changeColor() {
    QColorDialog colorDialog(this);
    colorDialog.setWindowTitle("Выберите цвет");
    colorDialog.setCurrentColor(Qt::red);

    if (colorDialog.exec() == QDialog::Accepted) {
        QColor color = colorDialog.selectedColor();
        shapes_.setSelectedColor(color);
        update();
    }
}

void MainWindow::clearWindow() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Очистка холста",
                                  "Вы уверены, что хотите удалить все фигуры?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        shapes_.clear();
        update();
        treeWidget_->rebuildTree();
    }
}

void MainWindow::increaseSize() {
    resizeSelected(5);
}

void MainWindow::decreaseSize() {
    resizeSelected(-5);
}

void MainWindow::updateWindowTitle() {
    QString shapeName;
    switch (currentShapeType_) {
    case CIRCLE: shapeName = "Круг"; break;
    case RECTANGLE: shapeName = "Прямоугольник"; break;
    case SQUARE: shapeName = "Квадрат"; break;
    case TRIANGLE: shapeName = "Треугольник"; break;
    case LINE: shapeName = "Линия"; break;
    }

    setWindowTitle("Визуальный редактор - " + shapeName);
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);

    // Рисуем фон всего окна
    painter.fillRect(rect(), Qt::lightGray);

    // Получаем рабочую область
    QWidget* workArea = splitter_->widget(1);
    QRect workRect = workArea->geometry();

    // Рисуем белую рабочую область
    painter.fillRect(workRect, Qt::white);

    // Сохраняем состояние
    painter.save();

    // Смещаем начало координат в левый верхний угол рабочей области
    painter.translate(workRect.topLeft());

    // Рисуем все фигуры
    for (int i = 0; i < shapes_.getCount(); i++) {
        CompositeElement* element = shapes_.getElement(i);
        if (element) {
            element->draw(painter);
        }
    }

    painter.restore();
}

void MainWindow::createNewShape(int x, int y) {
    CompositeElement* newElement = nullptr;
    int margin = 10;

    QWidget* workArea = splitter_->widget(1);
    int workWidth = workArea->width();
    int workHeight = workArea->height();

    switch (currentShapeType_) {
    case CIRCLE: {
        int radius = 40;
        if (x - radius >= margin && x + radius <= workWidth - margin &&
            y - radius >= margin && y + radius <= workHeight - margin) {
            newElement = new ShapeAdapter(new Circle(x, y, radius));
        } else {
            int safeX = std::max(margin + radius, std::min(x, workWidth - margin - radius));
            int safeY = std::max(margin + radius, std::min(y, workHeight - margin - radius));
            newElement = new ShapeAdapter(new Circle(safeX, safeY, radius));
        }
        break;
    }

    case RECTANGLE: {
        int rectWidth = 80;
        int rectHeight = 50;
        int leftX = x - rectWidth/2;
        int topY = y - rectHeight/2;

        if (leftX >= margin && leftX + rectWidth <= workWidth - margin &&
            topY >= margin && topY + rectHeight <= workHeight - margin) {
            newElement = new ShapeAdapter(new Rectangle(leftX, topY, rectWidth, rectHeight));
        } else {
            int safeLeftX = std::max(margin, std::min(leftX, workWidth - margin - rectWidth));
            int safeTopY = std::max(margin, std::min(topY, workHeight - margin - rectHeight));
            newElement = new ShapeAdapter(new Rectangle(safeLeftX, safeTopY, rectWidth, rectHeight));
        }
        break;
    }

    case SQUARE: {
        int size = 70;
        int leftX = x - size/2;
        int topY = y - size/2;

        if (leftX >= margin && leftX + size <= workWidth - margin &&
            topY >= margin && topY + size <= workHeight - margin) {
            newElement = new ShapeAdapter(new Square(leftX, topY, size));
        } else {
            int safeLeftX = std::max(margin, std::min(leftX, workWidth - margin - size));
            int safeTopY = std::max(margin, std::min(topY, workHeight - margin - size));
            newElement = new ShapeAdapter(new Square(safeLeftX, safeTopY, size));
        }
        break;
    }

    case TRIANGLE: {
        int size = 70;
        // Центр треугольника находится в (x + size/2, y)
        // Чтобы центр был в точке клика, передаем (x - size/2, y)
        int newX = x - size/2;
        int newY = y;

        // Проверяем границы треугольника
        if (newX >= margin && newX + size <= workWidth - margin &&
            newY - size/2 >= margin && newY + size/2 <= workHeight - margin) {
            newElement = new ShapeAdapter(new Triangle(newX, newY, size));
        } else {
            int safeX = std::max(margin, std::min(newX, workWidth - margin - size));
            int safeY = std::max(margin + size/2, std::min(newY, workHeight - margin - size/2));
            newElement = new ShapeAdapter(new Triangle(safeX, safeY, size));
        }
        break;
    }

    case LINE: {
        int length = 70;
        // Центр линии в середине, поэтому начало = x - length/2
        int startX = x - length/2;
        int startY = y;
        int endX = x + length/2;
        int endY = y;

        if (startX >= margin && endX <= workWidth - margin &&
            startY >= margin && startY <= workHeight - margin) {
            newElement = new ShapeAdapter(new Line(startX, startY, endX, endY));
        } else {
            int safeStartX = std::max(margin, std::min(startX, workWidth - margin - length));
            int safeY = std::max(margin, std::min(y, workHeight - margin));
            newElement = new ShapeAdapter(new Line(safeStartX, safeY, safeStartX + length, safeY));
        }
        break;
    }
    }

    if (newElement) {
        newElement->setSelected(false);
        shapes_.addElement(newElement);
        shapes_.notifySelectionChanged();
        treeWidget_->rebuildTree();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QWidget* workArea = splitter_->widget(1);
        QPoint localPos = event->pos();
        QRect workAreaGeometry = workArea->geometry();

        int x = localPos.x() - workAreaGeometry.x();
        int y = localPos.y() - workAreaGeometry.y();

        if (x < 0 || x >= workAreaGeometry.width() ||
            y < 0 || y >= workAreaGeometry.height()) {
            QMainWindow::mousePressEvent(event);
            return;
        }

        bool ctrlPressed = event->modifiers() & Qt::ControlModifier;

        CompositeElement* clicked = nullptr;
        for (int i = shapes_.getCount() - 1; i >= 0; i--) {
            CompositeElement* element = shapes_.getElement(i);
            if (element && element->contains(x, y)) {
                clicked = element;
                break;
            }
        }

        if (ctrlPressed) {
            if (clicked) {
                // С Ctrl: инвертируем выделение у кликнутого объекта
                clicked->setSelected(!clicked->getSelected());
                treeWidget_->syncSelectionFromContainer();
            } else {
                // С Ctrl на пустом месте: создаем новый объект, выделение не меняем
                createNewShape(x, y);
            }
        } else {
            if (clicked) {
                if (!clicked->getSelected()) {
                    // Клик на невыделенный объект без Ctrl: снимаем всё и выделяем его
                    shapes_.clearSelection();
                    clicked->setSelected(true);
                    treeWidget_->syncSelectionFromContainer();
                }
                // Если объект уже выделен - ничего не делаем
            } else {
                // Клик на пустом месте без Ctrl: снимаем выделение
                shapes_.clearSelection();
                treeWidget_->syncSelectionFromContainer();
                createNewShape(x, y);
            }
        }

        update();
    }
    else if (event->button() == Qt::RightButton) {
        // Правый клик - всегда снимаем выделение
        shapes_.clearSelection();
        treeWidget_->syncSelectionFromContainer();
        update();
    }

    QMainWindow::mousePressEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    bool needUpdate = false;
    int dx = 0, dy = 0;

    switch (event->key()) {
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        shapes_.removeSelected();
        needUpdate = true;
        treeWidget_->rebuildTree();
        break;

    case Qt::Key_1:
        currentShapeType_ = CIRCLE;
        updateWindowTitle();
        break;

    case Qt::Key_2:
        currentShapeType_ = RECTANGLE;
        updateWindowTitle();
        break;

    case Qt::Key_3:
        currentShapeType_ = SQUARE;
        updateWindowTitle();
        break;

    case Qt::Key_4:
        currentShapeType_ = TRIANGLE;
        updateWindowTitle();
        break;

    case Qt::Key_5:
        currentShapeType_ = LINE;
        updateWindowTitle();
        break;

    case Qt::Key_Left:
        dx = -5;
        break;

    case Qt::Key_Right:
        dx = 5;
        break;

    case Qt::Key_Up:
        dy = -5;
        break;

    case Qt::Key_Down:
        dy = 5;
        break;

    case Qt::Key_Escape:
        shapes_.clearSelection();
        treeWidget_->syncSelectionFromContainer();
        needUpdate = true;
        break;

    case Qt::Key_A:
        if (event->modifiers() & Qt::ControlModifier) {
            shapes_.selectAll();
            treeWidget_->syncSelectionFromContainer();
            needUpdate = true;
        }
        break;

    case Qt::Key_G:
        if (event->modifiers() & Qt::ControlModifier) {
            if (event->modifiers() & Qt::ShiftModifier) {
                ungroupSelected();
            } else {
                groupSelected();
            }
            needUpdate = true;
        }
        break;

    case Qt::Key_Plus:
    case Qt::Key_Equal:
        if (event->modifiers() & Qt::ControlModifier) {
            resizeSelected(5);
            needUpdate = true;
        }
        break;

    case Qt::Key_Minus:
        if (event->modifiers() & Qt::ControlModifier) {
            resizeSelected(-5);
            needUpdate = true;
        }
        break;
    }

    if (dx != 0 || dy != 0) {
        QWidget* workArea = splitter_->widget(1);
        int workWidth = workArea->width();
        int workHeight = workArea->height();

        shapes_.moveSelected(dx, dy, workWidth, workHeight, 0);
        treeWidget_->syncSelectionFromContainer();
        needUpdate = true;
    }

    if (needUpdate) {
        update();
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::groupSelected() {
    shapes_.groupSelected();
    treeWidget_->rebuildTree();
    update();
}

void MainWindow::ungroupSelected() {
    shapes_.ungroupSelected();
    treeWidget_->rebuildTree();
    update();
}

void MainWindow::resizeSelected(int delta) {
    QToolBar* toolBar = findChild<QToolBar*>();
    int toolBarHeight = toolBar ? toolBar->height() : 30;
    int usableTop = toolBarHeight;

    for (int i = 0; i < shapes_.getCount(); i++) {
        CompositeElement* element = shapes_.getElement(i);
        if (element && element->getSelected()) {
            if (element->isGroup()) {
                resizeGroupElements(element, delta, width(), height(), usableTop);
            } else {
                applyResizeWithBounds(element, delta, width(), height(), usableTop);
            }
        }
    }
    update();
}

void MainWindow::resizeGroupElements(CompositeElement* group, int delta, int maxX, int maxY, int topMargin) {
    if (!group->isGroup()) return;

    std::vector<CompositeElement*> allElements;
    collectNonGroupElementsFromGroup(group, allElements);

    bool canResizeAll = true;
    for (auto element : allElements) {
        if (!canResizeElement(element, delta, maxX, maxY, topMargin)) {
            canResizeAll = false;
            break;
        }
    }

    if (canResizeAll) {
        for (auto element : allElements) {
            applyResize(element, delta);
        }
    }
}

void MainWindow::collectNonGroupElementsFromGroup(CompositeElement* element, std::vector<CompositeElement*>& result) {
    if (element->isGroup()) {
        const std::vector<CompositeElement*>& children = element->getChildren();
        for (auto child : children) {
            collectNonGroupElementsFromGroup(child, result);
        }
    } else {
        result.push_back(element);
    }
}

bool MainWindow::canResizeElement(CompositeElement* element, int delta, int maxX, int maxY, int topMargin) {
    if (!element || element->isGroup()) return true;
    return canResizeWithBounds(element, delta, maxX, maxY, topMargin);
}

void MainWindow::applyResize(CompositeElement* element, int delta) {
    ShapeAdapter* adapter = dynamic_cast<ShapeAdapter*>(element);
    if (!adapter) return;

    Shape* shape = adapter->getShape();
    if (!shape) return;

    if (Circle* circle = dynamic_cast<Circle*>(shape)) {
        int newRadius = circle->getRadius() + delta;
        if (newRadius >= 5 && newRadius <= 100) {
            circle->setRadius(newRadius);
        }
    }
    else if (Rectangle* rect = dynamic_cast<Rectangle*>(shape)) {
        int newWidth = rect->getWidth() + delta;
        int newHeight = rect->getHeight() + delta;
        if (newWidth >= 10 && newWidth <= 200 && newHeight >= 10 && newHeight <= 200) {
            rect->setSize(newWidth, newHeight);
        }
    }
    else if (Square* square = dynamic_cast<Square*>(shape)) {
        int newSize = square->getSide() + delta;
        if (newSize >= 10 && newSize <= 200) {
            square->setSide(newSize);
        }
    }
    else if (Triangle* triangle = dynamic_cast<Triangle*>(shape)) {
        int newSize = triangle->getSize() + delta;
        if (newSize >= 10 && newSize <= 150) {
            triangle->setSize(newSize);
        }
    }
    else if (Line* line = dynamic_cast<Line*>(shape)) {
        int x1 = line->getX();
        int y1 = line->getY();
        int x2 = line->getX2();
        int y2 = line->getY2();

        int dx = x2 - x1;
        int dy = y2 - y1;
        double currentLength = sqrt(dx*dx + dy*dy);

        if (currentLength > 0) {
            double newLength = currentLength + delta;

            if (newLength >= 10 && newLength <= 200) {
                if (delta < 0 && currentLength + delta < 10) {
                    newLength = 10;
                }

                int centerX = (x1 + x2) / 2;
                int centerY = (y1 + y2) / 2;

                double scale = newLength / currentLength;

                int newX1 = centerX + (int)((x1 - centerX) * scale);
                int newY1 = centerY + (int)((y1 - centerY) * scale);
                int newX2 = centerX + (int)((x2 - centerX) * scale);
                int newY2 = centerY + (int)((y2 - centerY) * scale);

                line->setPosition(newX1, newY1);
                line->setEndPoint(newX2, newY2);
            }
        }
    }
}

void MainWindow::collectAllElementsForResize(CompositeElement* element, std::vector<CompositeElement*>& result) {
    collectNonGroupElementsFromGroup(element, result);
}

void MainWindow::applyResizeWithBounds(CompositeElement* element, int delta, int maxX, int maxY, int topMargin) {
    if (delta > 0) {
        if (!canResizeWithBounds(element, delta, maxX, maxY, topMargin)) {
            return;
        }
    }
    applyResize(element, delta);
}

bool MainWindow::canResizeWithBounds(CompositeElement* element, int delta, int maxX, int maxY, int topMargin) {
    if (delta <= 0) return true;

    ShapeAdapter* adapter = dynamic_cast<ShapeAdapter*>(element);
    if (!adapter) return true;

    Shape* shape = adapter->getShape();
    if (!shape) return true;

    if (Circle* circle = dynamic_cast<Circle*>(shape)) {
        int newRadius = circle->getRadius() + delta;

        QRect newBounds(circle->getX() - newRadius,
                        circle->getY() - newRadius,
                        2 * newRadius, 2 * newRadius);

        return (newBounds.left() >= 0 &&
                newBounds.right() <= maxX &&
                newBounds.top() >= topMargin &&
                newBounds.bottom() <= maxY);
    }
    else if (Rectangle* rect = dynamic_cast<Rectangle*>(shape)) {
        int newWidth = rect->getWidth() + delta;
        int newHeight = rect->getHeight() + delta;

        QRect newBounds(rect->getX(), rect->getY(), newWidth, newHeight);

        return (newBounds.left() >= 0 &&
                newBounds.right() <= maxX &&
                newBounds.top() >= topMargin &&
                newBounds.bottom() <= maxY);
    }
    else if (Square* square = dynamic_cast<Square*>(shape)) {
        int newSize = square->getSide() + delta;

        QRect newBounds(square->getX(), square->getY(), newSize, newSize);

        return (newBounds.left() >= 0 &&
                newBounds.right() <= maxX &&
                newBounds.top() >= topMargin &&
                newBounds.bottom() <= maxY);
    }
    else if (Triangle* triangle = dynamic_cast<Triangle*>(shape)) {
        int newSize = triangle->getSize() + delta;

        QRect newBounds(triangle->getX(),
                        triangle->getY() - newSize/2,
                        newSize, newSize);

        return (newBounds.left() >= 0 &&
                newBounds.right() <= maxX &&
                newBounds.top() >= topMargin &&
                newBounds.bottom() <= maxY);
    }
    else if (Line* line = dynamic_cast<Line*>(shape)) {
        int x1 = line->getX();
        int y1 = line->getY();
        int x2 = line->getX2();
        int y2 = line->getY2();

        double currentLength = sqrt(pow(x2-x1, 2) + pow(y2-y1, 2));
        double newLength = currentLength + delta;

        int centerX = (x1 + x2) / 2;
        int centerY = (y1 + y2) / 2;
        double scale = newLength / currentLength;

        int newX1 = centerX + (int)((x1 - centerX) * scale);
        int newY1 = centerY + (int)((y1 - centerY) * scale);
        int newX2 = centerX + (int)((x2 - centerX) * scale);
        int newY2 = centerY + (int)((y2 - centerY) * scale);

        int left = std::min(newX1, newX2);
        int right = std::max(newX1, newX2);
        int top = std::min(newY1, newY2);
        int bottom = std::max(newY1, newY2);

        return (left >= 0 &&
                right <= maxX &&
                top >= topMargin &&
                bottom <= maxY);
    }

    return true;
}

void MainWindow::saveToFile()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Сохранить проект",
        "",
        "Текстовые файлы (*.txt);;Все файлы (*.*)"
        );

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".txt", Qt::CaseInsensitive)) {
        fileName += ".txt";
    }

    if (shapes_.saveToFile(fileName.toStdString())) {
        QMessageBox::information(this, "Сохранение", "Проект успешно сохранен в файл " + fileName);
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить проект!");
    }
}

void MainWindow::loadFromFile()
{
    if (shapes_.getCount() > 0) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Загрузка проекта",
            "Текущий проект будет потерян. Продолжить?",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply != QMessageBox::Yes) {
            return;
        }
    }

    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Загрузить проект",
        "",
        "Текстовые файлы (*.txt);;Все файлы (*.*)"
        );

    if (fileName.isEmpty()) {
        return;
    }

    if (shapes_.loadFromFile(fileName.toStdString())) {
        QMessageBox::information(this, "Загрузка", "Проект успешно загружен из файла " + fileName);
        update();
        treeWidget_->rebuildTree();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить проект из файла " + fileName);
    }
}

void MainWindow::handleKeyEvent(QKeyEvent* event) {
    keyPressEvent(event);
}

void MainWindow::testSelection() {
    if (shapes_.getCount() > 0) {
        // Снимаем выделение со всех
        for (int i = 0; i < shapes_.getCount(); i++) {
            shapes_.getElement(i)->setSelected(false);
        }
        // Выделяем первый элемент
        shapes_.getElement(0)->setSelected(true);
        shapes_.notifySelectionChanged();
        update();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
