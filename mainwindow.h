#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "shapecontainer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum ShapeType {
    CIRCLE,
    RECTANGLE,
    SQUARE,
    TRIANGLE,
    LINE
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void selectCircle();
    void selectRectangle();
    void selectSquare();
    void selectTriangle();
    void selectLine();
    void changeColor();
    void clearWindow();
    void increaseSize();
    void decreaseSize();

private:
    Ui::MainWindow *ui;
    ShapeContainer shapes_;
    ShapeType currentShapeType_ = CIRCLE;

    void createMenu();
    void createToolBar();
    void updateWindowTitle();
    void resizeSelected(int delta);
    bool canResizeSelected(int delta) const;
    void applyResize(Shape* shape, int delta);
    void applyResizeWithBounds(Shape* shape, int delta, int maxX, int maxY, int topMargin);
    bool canResizeWithBounds(Shape* shape, int delta, int maxX, int maxY, int topMargin);
};

#endif // MAINWINDOW_H
