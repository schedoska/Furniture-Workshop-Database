#include "okno.h"
#include <QApplication>
#include <QtDebug>

#define DATABASE_FILE_NAME_STRING "/cuttingMaterialDataBase.db"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString currentApplicationPath = a.applicationDirPath();

    MainWindow w(currentApplicationPath.append(DATABASE_FILE_NAME_STRING));
    w.show();
    return a.exec();
}
