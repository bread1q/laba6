#include "mainwindow.h"
#include "./ui_mainwindow.h"
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Визуальный редактор - Круг (1)");
    resize(800, 600);
    setFocusPolicy(Qt::StrongFocus);
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
    });
    editMenu->addAction(deleteAction);

    QAction *selectAllAction = new QAction("Выделить все", this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    connect(selectAllAction, &QAction::triggered, [this]() {
        shapes_.selectAll();
        update();
    });
    editMenu->addAction(selectAllAction);

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
    });
    toolBar->addAction(deleteAction);

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

    painter.fillRect(rect(), Qt::white);

    for (int i = 0; i < shapes_.getCount(); i++) {
        Shape* shape = shapes_.getShape(i);
        if (shape) {
            shape->draw(painter);
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int x = event->pos().x();
    int y = event->pos().y();

    QToolBar* toolBar = findChild<QToolBar*>();
    int toolBarHeight = toolBar ? toolBar->height() : 30;
    int usableTop = toolBarHeight;

    if (event->button() == Qt::LeftButton) {
        bool ctrlPressed = event->modifiers() & Qt::ControlModifier;

        if (ctrlPressed) {
            bool foundAny = false;
            for (int i = shapes_.getCount() - 1; i >= 0; i--) {
                Shape* shape = shapes_.getShape(i);
                if (shape && shape->contains(x, y)) {
                    shape->setSelected(!shape->getSelected());
                    foundAny = true;
                }
            }

            if (!foundAny) {
                Shape* newShape = nullptr;
                int margin = 0;

                switch (currentShapeType_) {
                case CIRCLE: {
                    int radius = 40;
                    if (x - radius >= margin && x + radius <= width() - margin &&
                        y - radius >= usableTop && y + radius <= height() - margin) {
                        newShape = new Circle(x, y, radius);
                    } else {
                        int safeX = std::max(margin + radius, std::min(x, width() - margin - radius));
                        int safeY = std::max(usableTop + radius, std::min(y, height() - margin - radius));
                        newShape = new Circle(safeX, safeY, radius);
                    }
                    break;
                }
                case RECTANGLE: {
                    int rectWidth = 80;
                    int rectHeight = 50;
                    if (x >= margin && x <= width() - margin - rectWidth &&
                        y >= usableTop && y <= height() - margin - rectHeight) {
                        newShape = new Rectangle(x, y, rectWidth, rectHeight);
                    } else {
                        int safeX = std::max(margin, std::min(x, width() - margin - rectWidth));
                        int safeY = std::max(usableTop, std::min(y, height() - margin - rectHeight));
                        newShape = new Rectangle(safeX, safeY, rectWidth, rectHeight);
                    }
                    break;
                }
                case SQUARE: {
                    int size = 70;
                    if (x >= margin && x <= width() - margin - size &&
                        y >= usableTop && y <= height() - margin - size) {
                        newShape = new Square(x, y, size);
                    } else {
                        int safeX = std::max(margin, std::min(x, width() - margin - size));
                        int safeY = std::max(usableTop, std::min(y, height() - margin - size));
                        newShape = new Square(safeX, safeY, size);
                    }
                    break;
                }
                case TRIANGLE: {
                    int size = 70;
                    if (x >= margin && x <= width() - margin - size &&
                        y >= usableTop + size/2 && y <= height() - margin - size/2) {
                        newShape = new Triangle(x, y, size);
                    } else {
                        int safeX = std::max(margin, std::min(x, width() - margin - size));
                        int safeY = std::max(usableTop + size/2, std::min(y, height() - margin - size/2));
                        newShape = new Triangle(safeX, safeY, size);
                    }
                    break;
                }
                case LINE: {
                    int length = 70;
                    if (x >= margin && x <= width() - margin - length &&
                        y >= usableTop && y <= height() - margin) {
                        newShape = new Line(x, y, x + length, y);
                    } else {
                        int safeX = std::max(margin, std::min(x, width() - margin - length));
                        int safeY = std::max(usableTop, std::min(y, height() - margin));
                        newShape = new Line(safeX, safeY, safeX + length, safeY);
                    }
                    break;
                }
                }

                if (newShape) {
                    newShape->setSelected(false);
                    shapes_.addShape(newShape);
                }
            }
        } else {
            Shape* clickedShape = nullptr;

            for (int i = shapes_.getCount() - 1; i >= 0; i--) {
                Shape* shape = shapes_.getShape(i);
                if (shape && shape->contains(x, y)) {
                    clickedShape = shape;
                    break;
                }
            }

            if (clickedShape) {
                shapes_.clearSelection();
                clickedShape->setSelected(true);
            } else {
                Shape* newShape = nullptr;
                int margin = 0;

                switch (currentShapeType_) {
                case CIRCLE: {
                    int radius = 40;
                    if (x - radius >= margin && x + radius <= width() - margin &&
                        y - radius >= usableTop && y + radius <= height() - margin) {
                        newShape = new Circle(x, y, radius);
                    } else {
                        int safeX = std::max(margin + radius, std::min(x, width() - margin - radius));
                        int safeY = std::max(usableTop + radius, std::min(y, height() - margin - radius));
                        newShape = new Circle(safeX, safeY, radius);
                    }
                    break;
                }
                case RECTANGLE: {
                    int rectWidth = 80;
                    int rectHeight = 60;
                    if (x >= margin && x <= width() - margin - rectWidth &&
                        y >= usableTop && y <= height() - margin - rectHeight) {
                        newShape = new Rectangle(x, y, rectWidth, rectHeight);
                    } else {
                        int safeX = std::max(margin, std::min(x, width() - margin - rectWidth));
                        int safeY = std::max(usableTop, std::min(y, height() - margin - rectHeight));
                        newShape = new Rectangle(safeX, safeY, rectWidth, rectHeight);
                    }
                    break;
                }
                case SQUARE: {
                    int size = 70;
                    if (x >= margin && x <= width() - margin - size &&
                        y >= usableTop && y <= height() - margin - size) {
                        newShape = new Square(x, y, size);
                    } else {
                        int safeX = std::max(margin, std::min(x, width() - margin - size));
                        int safeY = std::max(usableTop, std::min(y, height() - margin - size));
                        newShape = new Square(safeX, safeY, size);
                    }
                    break;
                }
                case TRIANGLE: {
                    int size = 70;
                    if (x >= margin && x <= width() - margin - size &&
                        y >= usableTop + size/2 && y <= height() - margin - size/2) {
                        newShape = new Triangle(x, y, size);
                    } else {
                        int safeX = std::max(margin, std::min(x, width() - margin - size));
                        int safeY = std::max(usableTop + size/2, std::min(y, height() - margin - size/2));
                        newShape = new Triangle(safeX, safeY, size);
                    }
                    break;
                }
                case LINE: {
                    int length = 70;
                    if (x >= margin && x <= width() - margin - length &&
                        y >= usableTop && y <= height() - margin) {
                        newShape = new Line(x, y, x + length, y);
                    } else {
                        int safeX = std::max(margin, std::min(x, width() - margin - length));
                        int safeY = std::max(usableTop, std::min(y, height() - margin));
                        newShape = new Line(safeX, safeY, safeX + length, safeY);
                    }
                    break;
                }
                }

                if (newShape) {
                    newShape->setSelected(false);
                    shapes_.addShape(newShape);
                }
            }
        }

        update();
    }
    else if (event->button() == Qt::RightButton) {
        shapes_.clearSelection();
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
        needUpdate = true;
        break;

    case Qt::Key_A:
        if (event->modifiers() & Qt::ControlModifier) {
            shapes_.selectAll();
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
        QToolBar* toolBar = findChild<QToolBar*>();
        int toolBarHeight = toolBar ? toolBar->height() : 30;
        int usableTop = toolBarHeight;

        shapes_.moveSelected(dx, dy, width(), height(), usableTop);
        needUpdate = true;
    }

    if (needUpdate) {
        update();
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::resizeSelected(int delta) {
    QToolBar* toolBar = findChild<QToolBar*>();
    int toolBarHeight = toolBar ? toolBar->height() : 30;
    int usableTop = toolBarHeight;

    for (int i = 0; i < shapes_.getCount(); i++) {
        Shape* shape = shapes_.getShape(i);
        if (shape && shape->getSelected()) {
            applyResizeWithBounds(shape, delta, width(), height(), usableTop);
        }
    }
    update();
}

void MainWindow::applyResize(Shape* shape, int delta) {
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

void MainWindow::applyResizeWithBounds(Shape* shape, int delta, int maxX, int maxY, int topMargin) {
    if (delta > 0) {
        if (!canResizeWithBounds(shape, delta, maxX, maxY, topMargin)) {
            return;
        }
    }

    if (Circle* circle = dynamic_cast<Circle*>(shape)) {
        int newRadius = circle->getRadius() + delta;
        if (newRadius >= 5) {
            circle->setRadius(newRadius);
        }
    }
    else if (Rectangle* rect = dynamic_cast<Rectangle*>(shape)) {
        int newWidth = rect->getWidth() + delta;
        int newHeight = rect->getHeight() + delta;
        if (newWidth >= 10 && newHeight >= 10) {
            rect->setSize(newWidth, newHeight);
        }
    }
    else if (Square* square = dynamic_cast<Square*>(shape)) {
        int newSize = square->getSide() + delta;
        if (newSize >= 10) {
            square->setSide(newSize);
        }
    }
    else if (Triangle* triangle = dynamic_cast<Triangle*>(shape)) {
        int oldSize = triangle->getSize();
        int newSize = oldSize + delta;

        if (newSize >= 10) {
            QRect newBounds = QRect(
                triangle->getX(),
                triangle->getY() - newSize/2,
                newSize,
                newSize
                );

            if (newBounds.top() >= topMargin) {
                triangle->setSize(newSize);
            }
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

            if (newLength >= 10) {
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

                int left = std::min(newX1, newX2);
                int right = std::max(newX1, newX2);
                int top = std::min(newY1, newY2);
                int bottom = std::max(newY1, newY2);

                if (left >= 0 && right <= maxX && top >= topMargin && bottom <= maxY) {
                    line->setPosition(newX1, newY1);
                    line->setEndPoint(newX2, newY2);
                }
            }
        }
    }
}

bool MainWindow::canResizeWithBounds(Shape* shape, int delta, int maxX, int maxY, int topMargin) {
    if (delta <= 0) return true;

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

MainWindow::~MainWindow()
{
    delete ui;
}
