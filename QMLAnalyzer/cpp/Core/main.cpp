
// Qt
#include <QApplication>
#include <QDebug>

// Foundations
#include "QMLTree/QMLAnalyzer.h"

// Application
#include "CMainWindow.h"

// Unit tests
#include <QtTest>

//-------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qRegisterMetaType<QMLAnalyzerError>("QMLAnalyzerError");

    // Get arguments
    QStringList lArguments;

    for (int index = 0; index < argc; index++)
    {
        lArguments << argv[index];
    }

    if (argc == 2 && QString(argv[1]) == "--test")
    {
//        CTests tc;
//        return QTest::qExec(&tc, argc - 2, argv + 2);
        return 0;
    }
    else
    {
        // Launch application:
        CMainWindow mainWindow(lArguments, nullptr);
        mainWindow.show();

        return app.exec();
    }

    return 0;
}
