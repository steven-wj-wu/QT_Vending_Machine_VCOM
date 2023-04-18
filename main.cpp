#include "widget.h"
#include "QScreen"
#include <QFile>

#include <QApplication>
void msgOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
 QString typeName;
 switch (type) {
 case QtDebugMsg:
   typeName = "Debug";
   break;
 case QtInfoMsg:
   typeName = "Info";
   break;
 case QtWarningMsg:
   typeName = "Warn";
   break;
 case QtCriticalMsg:
   typeName = "Error";
   break;
 case QtFatalMsg:
   typeName = "Fatal";
   abort();
 }

 QString typeInfo = typeName.leftJustified(5, ' ');
 QString fileInfo = QString("%1:%2").arg(context.file).arg(context.line);
 fileInfo = fileInfo.right(20).rightJustified(20,' ', true);
 QString timeInfo = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
 QString text = QString("%1 | %2 | %3 | %4").arg(typeInfo, timeInfo, fileInfo, msg);

 fprintf(stderr, "%s\n", text.toLocal8Bit().constData());
 fflush(stderr);

 QString path = QCoreApplication::applicationDirPath();
 QFile file("/home/steven/log.txt");
 if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
   QTextStream tStream(&file);
   tStream << text << endl;
   file.close();
 }else{
  qDebug()<< "File Error: " << file.errorString();
 }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInstallMessageHandler(msgOutput);

    qDebug("This is a debug message");          //  qDebug：除錯資訊
    qInfo("This is a information message");     //  Info:   資訊訊息
    qWarning("This is a warning message");      //  qWarning：警告資訊
    qCritical("This is a critical message");    //  qCritical：嚴重錯誤


    QCursor cursor(Qt::BlankCursor);
    QApplication::setOverrideCursor(cursor);
    QApplication::changeOverrideCursor(cursor);
    Widget w;
    //QRect screenrect = a.primaryScreen()->geometry();
    w.move(0, -2);
    //w.setWindowFlags(Qt::WindowStaysOnBottomHint);
    //w.setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    w.show();

    return a.exec();
}
