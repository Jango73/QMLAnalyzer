
HEADERS += \
    cpp/Core/CConstants.h \
    cpp/Core/CMainWindow.h \
    cpp/Core/CUtils.h \
    cpp/DOMView/domitem.h \
    cpp/DOMView/dommodel.h \
    cpp/Processors/CImportProcessor.h \
    cpp/Views/CCodeAnalyzerView.h \
    cpp/Views/CDOMView.h \
    cpp/Views/CFolderView.h \
    cpp/Views/CHelpView.h \
    cpp/Views/CHTMLView.h \
    cpp/Widgets/CCodeEditor.h \
    cpp/Widgets/CRecentFileMenu.h \
    cpp/Widgets/CStatusWidget.h

SOURCES += \
    cpp/Core/CMainWindow.cpp \
    cpp/Core/CUtils.cpp \
    cpp/Core/main.cpp \
    cpp/DOMView/domitem.cpp \
    cpp/DOMView/dommodel.cpp \
    cpp/Processors/CImportProcessor.cpp \
    cpp/Views/CCodeAnalyzerView.cpp \
    cpp/Views/CDOMView.cpp \
    cpp/Views/CFolderView.cpp \
    cpp/Views/CHelpView.cpp \
    cpp/Views/CHTMLView.cpp \
    cpp/Widgets/CCodeEditor.cpp \
    cpp/Widgets/CRecentFileMenu.cpp \
    cpp/Widgets/CStatusWidget.cpp

FORMS += \
    forms/CCodeAnalyzerView.ui \
    forms/CDOMView.ui \
    forms/CStatusWidget.ui \
    forms/CFolderView.ui \
    forms/MainWindow.ui

RESOURCES += \
    resources.qrc
