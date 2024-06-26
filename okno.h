#ifndef OKNO_H
#define OKNO_H

#include <QMainWindow>
#include <QtDebug>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <list>
#include "cutter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QString dataBasePath, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSqlDatabase m_dataBase;

    Cutter cutter;

    void fillMagazynListFromDataBase();
    void fillMaterialListFromDataBase();
    void fillMebelListFromDataBase();
    void fillCuttingMebelListFromDataBase();
    void updateAllViews();

    void lockDB();
    void unlockDB();

public slots:
    // -------------------------------------------- MAGAZAYN
    void viewSelectedRowMagazyn();
    void modifySelectedRowMagazyn();
    void addNewRowMagazyn();
    void deleteRowMagazyn();
    // -------------------------------------------- MATERIALY
    void viewSelectedRowMaterial();
    void modifySelectedRowMaterial();
    void addNewRowMaterial();
    void deleteRowMaterial();
    //---------------------------------------------- PROJEKTY
    void viewSelectedRowMebel();
    void viewSelectedRowElement();
    void modifySelectedRowElementAndMebel();
    void addNewRowMebel();
    void addNewRowElement();
    void deleteRowMebel();
    void deleteRowElement();
    //---------------------------------------------- CIĘCIE
    void viewSelectedRowCuttingMebel(int);
    void planCutting();
    void executeCutting();
    void viewPlannedCutting(std::list<Component_with_cuts> cutData);
    //---------------------------------------------- LOGOWANIE
    void logInDB();
};

#endif // OKNO_H
