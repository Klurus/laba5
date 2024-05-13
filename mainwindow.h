#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QColorDialog>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QInputDialog>

class GraphicsItem : public QGraphicsItem {
public:
    explicit GraphicsItem(QRectF rect, QColor color, QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setColor(QColor color);
    void resize(const QRectF &newRect);
    void setPenColor(QColor color);
    void changePenColor();
    void setPen(const QPen& pen);
    QPen pen() const {
        return m_pen;
    }


private:
    QRectF m_rect;
    QColor m_color;
    QPointF m_mousePressPos;
    QRectF m_resizeRect;
    QPen m_pen;
    QColor m_borderColor;
};

class EllipseItem : public QObject, public QGraphicsEllipseItem {
    Q_OBJECT
public:
    EllipseItem(const QRectF &rect, QColor color, QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setColor(QColor color);
    void setPenColor(QColor color);
    void setSelectedBorder(bool selected);
    void setSelected(bool selected);
    void setBorderColor(const QColor& color);
    QColor borderColor() const;
    void resize(const QRectF &newRect); // Объявление метода resize

private:
    QRectF m_rect;
    QColor m_color;
    QPen m_pen;
    bool m_selected;
    QColor m_borderColor;
};


class TextItem : public QGraphicsTextItem {
    Q_OBJECT
public:
    explicit TextItem(QString text, QColor color, QGraphicsItem *parent = nullptr);
    void setColor(QColor color);
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void resize(const QRectF &newRect);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

public slots:
    void handleColorChange();
    void editText();

private:
    QPointF m_mousePressPos;
    QRectF m_resizeRect;
    QRectF m_rect;

};

class LineItem : public QGraphicsLineItem {
public:
    explicit LineItem(QLineF line, QColor color, QGraphicsItem *parent = nullptr);
    void setColor(QColor color);
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void resize(const QRectF &newRect);

private:
    QPointF m_mousePressPos;
    QPointF m_mousePressOffset;
    QRectF m_resizeRect;
    QRectF m_rect;
};

class GraphicsScene : public QGraphicsScene {
    Q_OBJECT
public:
    void addEllipseItem(QRectF rect, QColor color, Qt::PenStyle penStyle); // Правильная сигнатура метода addEllipseItem
    explicit GraphicsScene(QObject *parent = nullptr);
    void addRectItem(QRectF rect, QColor color, Qt::PenStyle penStyle);
    void addTextItem(QPointF pos, QString text, QColor color);
    void addLineItem(QLineF line, QColor color);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
private:
    QPointF m_startPos;
    QPointF m_endPos;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    void setBorderColor(const QColor& color);
    void setEllipseCreationMode(bool mode) { m_ellipseCreationMode = mode; }
    bool getEllipseCreationMode() const { return m_ellipseCreationMode; }
    void setRectCreationMode(bool mode) { m_rectCreationMode = mode; } // Добавьте в public MainWindow
    bool getRectCreationMode() const { return m_rectCreationMode; }
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void addRect();
    void addText();
    void addLine();
    void changeColor();
    void deleteItem();
    void changePenColor();
    void addEllipse();


private:
    bool m_ellipseCreationMode = false; // Добавление переменной m_ellipseCreationMode в класс MainWindow
    bool m_rectCreationMode = false;
    void createScene();
    void createView();
    void createToolbar();
    GraphicsScene *m_scene;
    QGraphicsView *m_view;
};

#endif // MAINWINDOW_H
