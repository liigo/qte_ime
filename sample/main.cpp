#include "mainwindow.h"
#include "../ime/inputcontext.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    (void)argc, (void)argv;
    //int myargc = 2;
    //const char* myargv[] = {"", "-qws"};
    QApplication app(argc, (char**)argv);

    app.setStyle(new CProxyStyle());
    app.setInputContext(new CInputContext());

    QTextCodec* utf8 = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForCStrings(utf8);

    MainWindow w;
    w.show();
    
    return app.exec();
}
