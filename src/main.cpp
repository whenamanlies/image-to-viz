#include <QApplication>
#include <QCommandLineParser>

#include "imagetoviz.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationDisplayName("image-to-viz");
    app.setApplicationVersion(APP_VERSION);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

    QCommandLineParser commandLineParser;
    commandLineParser.addHelpOption();
    commandLineParser.addPositionalArgument("[file]", "Input image.");
    commandLineParser.process(QCoreApplication::arguments());

    ImageToViz ImageToViz;
    if (!commandLineParser.positionalArguments().isEmpty()
        && !ImageToViz.loadFile(commandLineParser.positionalArguments().front())) {
        return -1;
    }
    ImageToViz.show();
    return app.exec();
}
