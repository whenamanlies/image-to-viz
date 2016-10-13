#include <QtWidgets>
#include "imagetoviz.h"

ImageToViz::ImageToViz()
   : imageLabel(new AspectRatioPixmapLabel)
{
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    imageLabel->setText("Open, drag'n'drop or paste an image");
    setCentralWidget(imageLabel);
    setAcceptDrops(true);

    createActions();

    // initialize safe colors table
    const QVector<QString> safeColors = {
        "#000000","#000033","#000066","#000099","#0000CC","#0000FF","#003300","#003333","#003366","#003399","#0033CC","#0033FF","#006600","#006633","#006666","#006699","#0066CC","#0066FF",
        "#009900","#009933","#009966","#009999","#0099CC","#0099FF","#00CC00","#00CC33","#00CC66","#00CC99","#00CCCC","#00CCFF","#00FF00","#00FF33","#00FF66","#00FF99","#00FFCC","#00FFFF",
        "#330000","#330033","#330066","#330099","#3300CC","#3300FF","#333300","#333333","#333366","#333399","#3333CC","#3333FF","#336600","#336633","#336666","#336699","#3366CC","#3366FF",
        "#339900","#339933","#339966","#339999","#3399CC","#3399FF","#33CC00","#33CC33","#33CC66","#33CC99","#33CCCC","#33CCFF","#33FF00","#33FF33","#33FF66","#33FF99","#33FFCC","#33FFFF",
        "#660000","#660033","#660066","#660099","#6600CC","#6600FF","#663300","#663333","#663366","#663399","#6633CC","#6633FF","#666600","#666633","#666666","#666699","#6666CC","#6666FF",
        "#669900","#669933","#669966","#669999","#6699CC","#6699FF","#66CC00","#66CC33","#66CC66","#66CC99","#66CCCC","#66CCFF","#66FF00","#66FF33","#66FF66","#66FF99","#66FFCC","#66FFFF",
        "#990000","#990033","#990066","#990099","#9900CC","#9900FF","#993300","#993333","#993366","#993399","#9933CC","#9933FF","#996600","#996633","#996666","#996699","#9966CC","#9966FF",
        "#999900","#999933","#999966","#999999","#9999CC","#9999FF","#99CC00","#99CC33","#99CC66","#99CC99","#99CCCC","#99CCFF","#99FF00","#99FF33","#99FF66","#99FF99","#99FFCC","#99FFFF",
        "#CC0000","#CC0033","#CC0066","#CC0099","#CC00CC","#CC00FF","#CC3300","#CC3333","#CC3366","#CC3399","#CC33CC","#CC33FF","#CC6600","#CC6633","#CC6666","#CC6699","#CC66CC","#CC66FF",
        "#CC9900","#CC9933","#CC9966","#CC9999","#CC99CC","#CC99FF","#CCCC00","#CCCC33","#CCCC66","#CCCC99","#CCCCCC","#CCCCFF","#CCFF00","#CCFF33","#CCFF66","#CCFF99","#CCFFCC","#CCFFFF",
        "#FF0000","#FF0033","#FF0066","#FF0099","#FF00CC","#FF00FF","#FF3300","#FF3333","#FF3366","#FF3399","#FF33CC","#FF33FF","#FF6600","#FF6633","#FF6666","#FF6699","#FF66CC","#FF66FF",
        "#FF9900","#FF9933","#FF9966","#FF9999","#FF99CC","#FF99FF","#FFCC00","#FFCC33","#FFCC66","#FFCC99","#FFCCCC","#FFCCFF","#FFFF00","#FFFF33","#FFFF66","#FFFF99","#FFFFCC","#FFFFFF"
    };

    colorTable = new QVector<QRgb>();
    foreach (QString color, safeColors)
    {
        colorTable->append(QRgb(QColor(color).rgb()));
    }

    resize(QGuiApplication::primaryScreen()->availableSize() / 2);

    //loadFile("sample.jpg");
}


bool ImageToViz::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 QString("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }

    setImage(newImage);
    originalImage = newImage;

    setWindowFilePath(fileName);

    const QString message = QString("Opened \"%1\", %2x%3, Depth: %4")
        .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());

    reduceColors();

    statusBar()->showMessage(message);
    return true;
}

void ImageToViz::setImage(const QImage &newImage)
{
    image = newImage;
    imageLabel->setPixmap(QPixmap::fromImage(image));

    reduceColorsAct->setEnabled(true);
    updateActions();
}

static void initializeImageFileDialog(QFileDialog &dialog)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    dialog.setNameFilter("Image files (*.png *.bmp *.jpg *.jpeg *.gif *.tiff)");
}

void ImageToViz::open()
{
    QFileDialog dialog(this, QString("Open File"));
    initializeImageFileDialog(dialog);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void ImageToViz::saveAs()
{
    statusBar()->showMessage(QString("Saving workbook..."));

    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QFileInfo fi(windowFilePath());
    if (!fi.baseName().isNull() && !fi.baseName().isEmpty())
    {
        desktopPath += QDir::separator() + fi.baseName() + QString(".twb");
    }
    else
    {
        desktopPath += QDir::separator() + QString("workbook.twb");
    }

    QString fileName = QFileDialog::getSaveFileName(this, QString("Save File"), desktopPath, QString("Tableau Workbook (*.twb)"));
    QApplication::setOverrideCursor(Qt::WaitCursor);
    qApp->processEvents();
    if (fileName.isNull())
    {
        statusBar()->showMessage(QString("Saving workbook canceled..."), 3000);
        QApplication::restoreOverrideCursor();
        return;
    }

    statusBar()->showMessage(QString("Saving workbook into \"%1\"").arg(fileName));

    if (!reduceColorsAct->isChecked())
    {
        reduceColorsAct->activate(QAction::Trigger);
    }
    getImageData(fileName);
    getTwbString(fileName);
    statusBar()->showMessage(QString("Saved workbook into \"%1\"").arg(fileName), 5000);
    QApplication::restoreOverrideCursor();
}

uint ImageToViz::colorToUInt(QColor color)
{
    return (uint)((color.red() << 16) | (color.green() << 8) | (color.blue() << 0));
}

void ImageToViz::getImageData(QString fileName)
{
    QFile file(fileName + ".csv");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "X,Y,Color\n";

    foreach (auto color, *colorTable)
    {
        out << QString("0,0,%1\n").arg(colorToUInt(color));
    }

    for (int y = 0; y < image.height(); y++)
    {
        for (int x = 0; x < image.width(); x++)
        {
            out << QString("%1,%2,%3\n").arg(x).arg(image.height() - y).arg(colorToUInt(image.pixelColor(x, y)));
        }
    }
}

void ImageToViz::getTwbString(QString fileName)
{
    // load template
    QFile file(":/template.twb");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    QString templateContent = in.readAll();

    templateContent = templateContent.replace("{IMAGEDATAFILENAME}", QFileInfo(fileName).fileName());
    templateContent = templateContent.replace("{AXISWIDTH}", QString::number(image.width()));
    templateContent = templateContent.replace("{AXISHEIGHT}", QString::number(image.height()));
    templateContent = templateContent.replace("{DASHBOARDWIDTH}", QString::number(getDashboardWidth(image)));
    templateContent = templateContent.replace("{DASHBOARDHEIGHT}", QString::number(getDashboardHeight(image)));
    templateContent = templateContent.replace("directory='Data'", "directory='.'");

    QFile fileTwb(fileName);
    if (!fileTwb.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&fileTwb);
    out << templateContent;
}

double ImageToViz::getDashboardScale(QImage img)
{
    auto DESIRED_WIDTH = 1600;
    auto DESIRED_HEIGHT = 900;
    auto scale = qMin((double)DESIRED_WIDTH / img.width(), (double)DESIRED_HEIGHT / img.height());
    return qMin(1.0, scale);
}
int ImageToViz::getDashboardWidth(QImage img)
{
    auto scale = getDashboardScale(img);
    return (int)(img.width() * scale);
}
int ImageToViz::getDashboardHeight(QImage img)
{
    auto scale = getDashboardScale(img);
    return (int)(img.height() * scale);
}



void ImageToViz::copy()
{
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(image);
#endif // !QT_NO_CLIPBOARD
}

#ifndef QT_NO_CLIPBOARD
static QImage clipboardImage()
{
    if (const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData()) {
        if (mimeData->hasImage()) {
            const QImage image = qvariant_cast<QImage>(mimeData->imageData());
            if (!image.isNull())
                return image;
        }
    }
    return QImage();
}
#endif // !QT_NO_CLIPBOARD

void ImageToViz::paste()
{
#ifndef QT_NO_CLIPBOARD
    const QImage newImage = clipboardImage();
    if (newImage.isNull()) {
        statusBar()->showMessage(QString("No image in clipboard"));
    } else {
        setImage(newImage);
        originalImage = newImage;
        reduceColors();
        setWindowFilePath(QString());
        const QString message = QString("Obtained image from clipboard, %1x%2, Depth: %3")
            .arg(newImage.width()).arg(newImage.height()).arg(newImage.depth());
        statusBar()->showMessage(message);
    }
#endif // !QT_NO_CLIPBOARD
}

void ImageToViz::reduceColors()
{
    bool fReduce = reduceColorsAct->isChecked();

    //const QString message = QString("reduceColors = %1, path = %2")
    //    .arg(fReduce).arg(windowFilePath());
    //statusBar()->showMessage(message);

    if (fReduce)
    {
        auto tmp = image.convertToFormat(QImage::Format_Indexed8);
        //setImage(tmp);
        tmp.setColorCount(colorTable->size());
        tmp.setColorTable(*colorTable);
        setImage(tmp.convertToFormat(QImage::Format_Indexed8, Qt::DiffuseDither|Qt::DiffuseAlphaDither|Qt::PreferDither));
    }
    else
    {
        setImage(originalImage);
    }

    updateActions();
}

void ImageToViz::about()
{
    QMessageBox::about(this, QString("About Image Viewer"),
            QString("Image-to-viz lets you take an <b>image</b>, reduce its color palette and produce a Tableau viz<br><br>"
               "Dmitry Chirkov, 2016"));
}

void ImageToViz::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(QString("&File"));

    QAction *openAct = fileMenu->addAction(QString("&Open..."), this, &ImageToViz::open);
    openAct->setShortcut(QKeySequence::Open);

    saveAsAct = fileMenu->addAction(QString("&Save As..."), this, &ImageToViz::saveAs);
    saveAsAct->setEnabled(false);
    saveAsAct->setShortcut(QString("Ctrl+S"));

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(QString("E&xit"), this, &QWidget::close);
    exitAct->setShortcut(QString("Ctrl+Q"));

    QMenu *editMenu = menuBar()->addMenu(QString("&Edit"));

    copyAct = editMenu->addAction(QString("&Copy"), this, &ImageToViz::copy);
    copyAct->setShortcut(QKeySequence::Copy);
    copyAct->setEnabled(false);

    QAction *pasteAct = editMenu->addAction(QString("&Paste"), this, &ImageToViz::paste);
    pasteAct->setShortcut(QKeySequence::Paste);

    editMenu->addSeparator();

    reduceColorsAct = editMenu->addAction(QString("&Reduce Colors"), this, &ImageToViz::reduceColors);
    reduceColorsAct->setEnabled(false);
    reduceColorsAct->setCheckable(true);
    reduceColorsAct->setChecked(true);
    reduceColorsAct->setShortcut(QString("Ctrl+R"));

    QMenu *helpMenu = menuBar()->addMenu(QString("&Help"));
    helpMenu->addAction(QString("&About"), this, &ImageToViz::about);
}

void ImageToViz::updateActions()
{
    saveAsAct->setEnabled(!image.isNull());
    copyAct->setEnabled(!image.isNull());
}

void ImageToViz::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls() && e->mimeData()->urls().length() < 2) {
        e->acceptProposedAction();
    }
}

void ImageToViz::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}

void ImageToViz::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString fileName = url.toLocalFile();
        qDebug() << "Dropped file:" << fileName;
        loadFile(fileName);
    }
}
