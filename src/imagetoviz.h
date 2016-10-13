#ifndef ImageToViz_H
#define ImageToViz_H

#include <QMainWindow>
#include <QImage>
#include "aspectratiopixmaplabel.h"

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class ImageToViz : public QMainWindow
{
    Q_OBJECT

public:
    ImageToViz();
    bool loadFile(const QString &);

    void dragEnterEvent(QDragEnterEvent *e);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *e);

private slots:
    void open();
    void saveAs();
    void copy();
    void paste();
    void reduceColors();
    void about();

private:
    void createActions();
    void createMenus();
    void updateActions();
    bool saveFile(const QString &fileName);
    void setImage(const QImage &newImage);
    uint colorToUInt(QColor color);
    void getImageData(QString fileName);
    void getTwbString(QString fileName);
    double getDashboardScale(QImage img);
    int getDashboardWidth(QImage img);
    int getDashboardHeight(QImage img);

    QString templateContent;

    QImage image;
    QImage originalImage;
    AspectRatioPixmapLabel *imageLabel;
    QVector<QRgb> *colorTable;

    QAction *saveAsAct;
    QAction *copyAct;
    QAction *reduceColorsAct;
    QString getWorkbookName(QString desktopPath);
};

#endif
