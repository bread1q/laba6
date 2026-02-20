#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "shapecontainer.h"
#include "objecttreewidget.h"
#include <QSplitter>

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
    void handleKeyEvent(QKeyEvent* event);  // Для обработки клавиш из дерева

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
    void groupSelected();
    void ungroupSelected();

    void saveToFile();
    void loadFromFile();

    void testSelection();

    void addArrow(bool bidirectional);
    void setArrowMode(bool enabled);

private:
    Ui::MainWindow *ui;
    ShapeContainer shapes_;
    ObjectTreeWidget* treeWidget_;
    QSplitter* splitter_;
    ShapeType currentShapeType_ = CIRCLE;
    bool arrowMode_;

    void createMenu();
    void createToolBar();
    void updateWindowTitle();
    void resizeSelected(int delta);
    void applyResize(CompositeElement* element, int delta);
    void applyResizeWithBounds(CompositeElement* element, int delta, int maxX, int maxY, int topMargin);
    bool canResizeWithBounds(CompositeElement* element, int delta, int maxX, int maxY, int topMargin);
    void createNewShape(int x, int y);  // Убрали topMargin

    void collectAllElementsForResize(CompositeElement* element, std::vector<CompositeElement*>& result);
    void collectNonGroupElementsFromGroup(CompositeElement* element, std::vector<CompositeElement*>& result);
    void resizeGroupElements(CompositeElement* group, int delta, int maxX, int maxY, int topMargin);
    bool canResizeElement(CompositeElement* element, int delta, int maxX, int maxY, int topMargin);
};

#endif // MAINWINDOW_H
