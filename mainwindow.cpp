#include "mainwindow.h"
#include <QToolBar>
#include <QCoreApplication>
#include <QMenu>
#include <QStyleOptionGraphicsItem>

EllipseItem::EllipseItem(const QRectF &rect, QColor color, QGraphicsItem *parent)
    : QGraphicsEllipseItem(rect, parent), m_rect(rect), m_color(color) {
    setFlag(ItemIsMovable, true);
    setFlag(ItemIsSelectable, true);
    m_pen = QPen(Qt::NoPen); // Установите начальный стиль контура
}

void MainWindow::addEllipse() {
    m_ellipseCreationMode = true; // Установите режим создания эллипса

}

QRectF EllipseItem::boundingRect() const {
    return m_rect;
}

void EllipseItem::setSelectedBorder(bool selected) {
    m_selected = selected;
    update();
}

void EllipseItem::setSelected(bool selected) {
    if (m_selected != selected) {
        m_selected = selected;
        update();
    }
}

void EllipseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

        QPen dashPen(Qt::black);
        dashPen.setStyle(Qt::DashLine);
        painter->setPen(dashPen);
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(m_borderColor, 3)); // Устанавливаем стиль контура эллипса
        painter->setBrush(m_color); // Устанавливаем заливку
        painter->drawEllipse(boundingRect()); // Рисуем сам эллипс
}

void EllipseItem::setBorderColor(const QColor& color) {
    m_borderColor = color;
}

QColor EllipseItem::borderColor() const {
    return m_borderColor;
}

void EllipseItem::setColor(QColor color) {
    m_color = color;
    update();
}


GraphicsItem::GraphicsItem(QRectF rect, QColor color, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_rect(rect), m_color(color) {
    setFlag(ItemIsMovable, true);
    setFlag(ItemIsSelectable, true);
}


QRectF GraphicsItem::boundingRect() const {
    return m_rect;
}



void GraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

        QPen dashPen(Qt::black);
        dashPen.setStyle(Qt::DashLine);
        painter->setPen(dashPen);
        painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(m_borderColor, 3)); // Устанавливаем стиль контура прямоугольника
    painter->setBrush(m_color); // Устанавливаем цвет заливки
    painter->drawRect(boundingRect()); // Рисуем прямоугольник

}


void GraphicsItem::setColor(QColor color) {
    m_color = color;
}



void GraphicsItem::setPenColor(QColor color) {
    if (m_pen.style() == Qt::NoPen) {
        m_pen.setStyle(Qt::SolidLine); // устанавливаем стиль контура, если ранее не был установлен
    }
    m_pen.setColor(color);
    update();
}
void EllipseItem::setPenColor(QColor color) {
    if (m_pen.style() == Qt::NoPen) {
        m_pen.setStyle(Qt::SolidLine); // устанавливаем стиль контура, если ранее не был установлен
    }
    m_pen.setColor(color);
    setPen(m_pen);
    update();
}


void MainWindow::changePenColor() {
    QColor color = QColorDialog::getColor(Qt::white, this, tr("Выберите цвет контура"));
    if (color.isValid()) {
        QList<QGraphicsItem*> items = m_scene->selectedItems();
        for (QGraphicsItem* item : items) {
            GraphicsItem* graphicsItem = dynamic_cast<GraphicsItem*>(item);
            if (graphicsItem) {
                QPen pen = graphicsItem->pen();
                pen.setColor(color);
                graphicsItem->setPen(pen);
                graphicsItem->update();
            } else {
                EllipseItem* ellipseItem = dynamic_cast<EllipseItem*>(item);
                if (ellipseItem) {
                    ellipseItem->setBorderColor(color);
                    ellipseItem->update();
                }
            }
        }
    }
}

void GraphicsItem::setPen(const QPen& pen) {
    m_pen = pen;
}


TextItem::TextItem(QString text, QColor color, QGraphicsItem *parent)
    : QGraphicsTextItem(text, parent) {
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setDefaultTextColor(color);
}

void TextItem::setColor(QColor color) {
    setDefaultTextColor(color);
}

void TextItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    m_mousePressPos = event->scenePos();
    QGraphicsTextItem::mousePressEvent(event);
}

void TextItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QPointF newPos = event->scenePos();
    moveBy(newPos.x() - m_mousePressPos.x(), newPos.y() - m_mousePressPos.y());
    m_mousePressPos = newPos;
    QGraphicsTextItem::mouseMoveEvent(event);
}


void TextItem::handleColorChange() {
    QColor color = QColorDialog::getColor(defaultTextColor(), nullptr, "Выберите цвет текста");
    if (color.isValid())
        setColor(color);
}

LineItem::LineItem(QLineF line, QColor color, QGraphicsItem *parent)
    : QGraphicsLineItem(line, parent) {
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setPen(QPen(color));
}

void LineItem::setColor(QColor color) {
    setPen(QPen(color));
}

void LineItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    //m_mousePressPos = event->scenePos();
    //m_mousePressOffset = line().p1() - m_mousePressPos;

    if (event->button() == Qt::LeftButton) {
        m_resizeRect = m_rect;
        m_mousePressPos = event->scenePos();
    }
    QGraphicsItem::mousePressEvent(event);
}

void LineItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QPointF newPos = event->scenePos() + m_mousePressOffset;
    setLine(QLineF(line().p1(), newPos));
}


GraphicsScene::GraphicsScene(QObject *parent)
    : QGraphicsScene(parent) {m_startPos = QPointF();
    m_endPos = QPointF();}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (MainWindow *mainWindow = qobject_cast<MainWindow*>(this->parent())) {
        if (mainWindow->getRectCreationMode() && event->button() == Qt::LeftButton) {
            if (m_startPos.isNull()) {
                m_startPos = event->scenePos();
                addRectItem(QRectF(m_startPos, QSizeF(0, 0)), Qt::white, Qt::DashLine);
            }
        }

            else if (MainWindow *mainWindow = qobject_cast<MainWindow*>(this->parent())) {
                if (mainWindow->getEllipseCreationMode() && event->button() == Qt::LeftButton) {
                    if (m_startPos.isNull()) {
                        m_startPos = event->scenePos();
                        addEllipseItem(QRectF(m_startPos, QSizeF(0, 0)), Qt::white, Qt::DashLine); // Добавление временного эллипса
                    }
                }
            }
    }
    QGraphicsScene::mousePressEvent(event);
}

void GraphicsItem::resize(const QRectF &newRect) {
    m_rect = newRect; // Обновляем размеры прямоугольника
    update(); // Вызываем update для перерисовки
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (MainWindow *mainWindow = qobject_cast<MainWindow*>(this->parent())) {
        if (mainWindow->getRectCreationMode() && event->buttons() == Qt::LeftButton) {
            m_endPos = event->scenePos();
            QRectF rect(QPointF(qMin(m_startPos.x(), m_endPos.x()), qMin(m_startPos.y(), m_endPos.y())),
                        QPointF(qMax(m_startPos.x(), m_endPos.x()), qMax(m_startPos.y(), m_endPos.y())));

            QList<QGraphicsItem*> items = this->items();
            if (!items.empty()) {
                GraphicsItem* item = dynamic_cast<GraphicsItem*>(items.last());
                if (item) {
                    item->resize(rect);
                }
            }
        }

        else if (mainWindow->getEllipseCreationMode() && event->buttons() == Qt::LeftButton) {
                m_endPos = event->scenePos();
                QRectF ellipseRect(m_startPos, m_endPos); // Обновление размеров эллипса до текущей позиции мыши

                QList<QGraphicsItem*> items = this->items();
                if (!items.empty()) {
                    EllipseItem *item = dynamic_cast<EllipseItem*>(items.last());
                    if (item) {
                        item->resize(ellipseRect);
                    }
                }
            }
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (MainWindow *mainWindow = qobject_cast<MainWindow*>(this->parent())) {
        if (mainWindow->getRectCreationMode()) {
            if (event->button() == Qt::LeftButton) {
                m_endPos = event->scenePos();
                QRectF rect(QPointF(qMin(m_startPos.x(), m_endPos.x()), qMin(m_startPos.y(), m_endPos.y())),
                            QPointF(qMax(m_startPos.x(), m_endPos.x()), qMax(m_startPos.y(), m_endPos.y())));
                addRectItem(rect, Qt::white, Qt::SolidLine);
                m_startPos = QPointF();
                m_endPos = QPointF();

                // Удаление временного пунктирного прямоугольника
                QList<QGraphicsItem*> items = this->items();
                if (!items.empty()) {
                    this->removeItem(items.last());
                }

                mainWindow->setRectCreationMode(false);
            }
        }
        else if (mainWindow->getEllipseCreationMode()) {
            if (event->button() == Qt::LeftButton) {
                m_endPos = event->scenePos();
                QRectF ellipseRect(m_startPos, m_endPos); // Окончательные размеры эллипса
                addEllipseItem(ellipseRect, Qt::white, Qt::DashLine); // Добавление окончательного эллипса на сцену

                m_startPos = QPointF(); // Сброс начальной точки
                m_endPos = QPointF();   // Сброс конечной точки

                // Удаление временного эллипса
                QList<QGraphicsItem*> items = this->items();
                if (!items.empty()) {
                    this->removeItem(items.last());
                }

                mainWindow->setEllipseCreationMode(false); // Отключение режима создания эллипса
            }
        }
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void GraphicsScene::addRectItem(QRectF rect, QColor color, Qt::PenStyle penStyle) {
    auto item = new GraphicsItem(rect, color);

    QPen pen = item->pen();
    pen.setStyle(penStyle); // Устанавливаем стиль линии - пунктир или сплошная
    item->setPen(pen);



    addItem(item);
}



void GraphicsScene::addEllipseItem(QRectF rect, QColor color, Qt::PenStyle penStyle) { // Правильная сигнатура метода addEllipseItem
    auto item = new EllipseItem(rect, color);

    QPen pen = item->pen();
    pen.setStyle(penStyle); // Устанавливаем стиль линии - пунктир или сплошная
    item->setPen(pen);

    // Устанавливаем стиль контура для начального обвода эллипса
    item->setBorderColor(Qt::black);  // Устанавливаем цвет контура
    item->setSelectedBorder(true); // Выделяем обвод эллипса пунктирной линией при создании

    addItem(item);
}

void EllipseItem::resize(const QRectF &newRect) {
    prepareGeometryChange(); // Подготовка к изменению геометрии
    m_rect = newRect; // Установка новых размеров эллипса
    update(); // Обновление элемента для прорисовки
}

void GraphicsScene::addTextItem(QPointF pos, QString text, QColor color) {
    auto item = new TextItem(text, color);
    item->setPos(pos);
    addItem(item);
}

void GraphicsScene::addLineItem(QLineF line, QColor color) {
    auto item = new LineItem(line, color);
    addItem(item);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    QGraphicsScene *scene = new QGraphicsScene(this);
    m_view = new QGraphicsView(scene);
    createScene();
    createView();
    createToolbar();
    setCentralWidget(m_view);
    setWindowTitle(tr("Графический редактор"));
    setWindowIcon(QIcon("C:/Users/itisk/Pictures/IIK.ico"));

    setGeometry(100, 100, 800, 600); // Set the initial position and size of the window
    setFixedSize(800, 600); // Set the fixed size of the window to 800x600 pixels

}

void MainWindow::addRect() {
    m_rectCreationMode = true;
}


void MainWindow::addText() {
    QString text = QInputDialog::getText(this, tr("Ввод текста"), tr("Введите текст:"));
    if (!text.isEmpty()) {
        m_scene->addTextItem(QPointF(0, 0), text, Qt::black);
    }
}

void MainWindow::addLine() {
    QLineF line(QPointF(0, 0), QPointF(100, 100));
    m_scene->addLineItem(line, Qt::blue);
}

void MainWindow::changeColor() {
    QColor color = QColorDialog::getColor(Qt::white, this, tr("Выбрать цвет"));
    if (color.isValid()) {
        QList<QGraphicsItem*> items = m_scene->selectedItems();
        for (QGraphicsItem* item : items) {
            GraphicsItem* graphicsItem = dynamic_cast<GraphicsItem*>(item);
            if (graphicsItem)
                graphicsItem->setColor(color);
            TextItem* textItem = dynamic_cast<TextItem*>(item);
            if (textItem)
                textItem->setColor(color);
            LineItem* lineItem = dynamic_cast<LineItem*>(item);
            if (lineItem)
                lineItem->setColor(color);
            EllipseItem* ellipseItem = dynamic_cast<EllipseItem*>(item);
            if (ellipseItem)
                ellipseItem->setColor(color);
        }
    }
}


void MainWindow::deleteItem() {
    QList<QGraphicsItem*> items = m_scene->selectedItems();
    for (QGraphicsItem* item : items) {
        m_scene->removeItem(item);
        delete item;
    }
}

void MainWindow::createScene() {
    m_scene = new GraphicsScene(this);
}

void MainWindow::createView() {
    m_view = new QGraphicsView(m_scene, this);
    m_view->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::createToolbar() {
    QToolBar *toolbar = addToolBar(tr("Toolbar"));

    // Создание кнопки для прямоугольника с изображением
    QAction *rectAction = new QAction(QIcon(":/images/icon_square.png"), tr("Прямоугольник"), this);
    connect(rectAction, &QAction::triggered, this, &MainWindow::addRect);
    toolbar->addAction(rectAction);

    // Создание кнопки для круга с изображением
    QAction *ellipseAction = new QAction(QIcon(":/images/icon_circle.png"), tr("Круг"), this);
    connect(ellipseAction, &QAction::triggered, this, &MainWindow::addEllipse);
    toolbar->addAction(ellipseAction);

    // Создание кнопки для линии с изображением
    QAction *lineAction = new QAction(QIcon(":/images/icon_line.png"), tr("Линия"), this);
    connect(lineAction, &QAction::triggered, this, &MainWindow::addLine);
    toolbar->addAction(lineAction);

    QAction *textAction = new QAction(QIcon(":/images/icon_text.png"), tr("Текст"), this);
    connect(textAction, &QAction::triggered, this, &MainWindow::addText);
    toolbar->addAction(textAction);

    QAction *colorAction = new QAction(QIcon(":/images/icon_color.png"), tr("Изменить цвет"), this);
    connect(colorAction, &QAction::triggered, this, &MainWindow::changeColor);
    toolbar->addAction(colorAction);

    QAction *penColorAction = new QAction(QIcon(":/images/contur.png"),tr("Цвет контура"), this);
    connect(penColorAction, &QAction::triggered, this, &MainWindow::changePenColor);
    toolbar->addAction(penColorAction);

    QAction *deleteAction = new QAction(QIcon(":/images/icon_delete.png"), tr("Удалить"), this);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::deleteItem);
    toolbar->addAction(deleteAction);

    //QAction *squareAction = new QAction(QIcon(":/images/icon_square.png"), tr("Прямоугольник"), this);
    //connect(squareAction, &QAction::triggered, this, &MainWindow::addSquare);
    //toolbar->addAction(squareAction);


}

void TextItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    QMenu contextMenu;
    QAction editAction("Редактировать текст", this);
    connect(&editAction, &QAction::triggered, this, &TextItem::editText);
    contextMenu.addAction(&editAction);
    contextMenu.exec(event->screenPos());
}

void TextItem::editText() {
    bool ok;
    QString newText = QInputDialog::getText(nullptr, "Редактировать текст", "Введите новый текст:", QLineEdit::Normal, this->toPlainText(), &ok);
    if (ok && !newText.isEmpty()) {
        this->setPlainText(newText);
    }
}




