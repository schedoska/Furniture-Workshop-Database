#include "okno.h"
#include "ui_okno.h"
#include "tilecutview.h"
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QString>

#include <iostream>

MainWindow::MainWindow(QString dataBasePath, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //setGeometry(400,200,1200,800);
    setMinimumSize(1400, 700);

    // Hide disabled tabs
    for(unsigned int i = 1; i < 5; ++i)
        ui->tabWidget->setTabEnabled(i, false);

    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);

    // --------------- LOGOWANIE
    QObject::connect(ui->log_in_btn, SIGNAL(pressed()), this, SLOT(logInDB()));

    // --------------- MAGAZYN
    QObject::connect(ui->list_magazyn, SIGNAL(itemSelectionChanged()), this, SLOT(viewSelectedRowMagazyn()));
    QObject::connect(ui->modifyButton_magazyn, SIGNAL(pressed()), this, SLOT(modifySelectedRowMagazyn()));
    QObject::connect(ui->addNewButton_magazyn, SIGNAL(pressed()),this, SLOT(addNewRowMagazyn()));
    QObject::connect(ui->removeButton_magazyn, SIGNAL(pressed()), this, SLOT(deleteRowMagazyn()));

    // --------------- MATERIAL
    QObject::connect(ui->list_material, SIGNAL(itemSelectionChanged()), this, SLOT(viewSelectedRowMaterial()));
    QObject::connect(ui->modifyButton_material, SIGNAL(pressed()), this, SLOT(modifySelectedRowMaterial()));
    QObject::connect(ui->addNewButton_material, SIGNAL(pressed()),this, SLOT(addNewRowMaterial()));
    QObject::connect(ui->removeButton_material, SIGNAL(pressed()), this, SLOT(deleteRowMaterial()));

    // --------------- PROJEKTY
    QObject::connect(ui->listMebel, SIGNAL(itemSelectionChanged()), this, SLOT(viewSelectedRowMebel()));
    QObject::connect(ui->listElement, SIGNAL(itemSelectionChanged()), this, SLOT(viewSelectedRowElement()));
    QObject::connect(ui->modifyButton_mebel, SIGNAL(pressed()), this, SLOT(modifySelectedRowElementAndMebel()));
    QObject::connect(ui->addButton_mebel, SIGNAL(pressed()),this, SLOT(addNewRowMebel()));
    QObject::connect(ui->addButton_element, SIGNAL(pressed()),this, SLOT(addNewRowElement()));
    QObject::connect(ui->deleteButton_mebel, SIGNAL(pressed()), this, SLOT(deleteRowMebel()));
    QObject::connect(ui->removeButton_element, SIGNAL(pressed()), this, SLOT(deleteRowElement()));

    // ---------------- CUTTING
    QObject::connect(ui->mebelComboBox_cut, SIGNAL(currentIndexChanged(int)), this, SLOT(viewSelectedRowCuttingMebel(int)));
    QObject::connect(ui->planCutButton_cut, SIGNAL(pressed()), this, SLOT(planCutting()));
    QObject::connect(ui->executeCutButton_cut, SIGNAL(pressed()), this, SLOT(executeCutting()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fillMagazynListFromDataBase()
{
    ui->list_magazyn->clear();
    QSqlQuery query;
    query.prepare(QString("SELECT id_plyta, Rodzaj_materialu.nazwa, wysokosc, szerokosc FROM Plyta INNER JOIN Rodzaj_materialu on Rodzaj_materialu.id_rodzaj_materialu = Plyta.id_rodzaj_materialu;"));
    query.exec();
    query.first();
    if(!query.isValid()) return;
    unsigned numerator = 0;
    do{
        QString position_name = QString::number(++numerator) + ".  " + query.value("Rodzaj_materialu.nazwa").toString() + "  -  " +
                                query.value("wysokosc").toString() + " cm x " + query.value("szerokosc").toString() + " cm";
        QListWidgetItem *it = new QListWidgetItem(position_name);
        it->setWhatsThis(query.value("id_plyta").toString()); // ukryta zmienna z ID płyty
        ui->list_magazyn->insertItem(ui->list_magazyn->count(), it);
    }while(query.next());
    ui->MaterialComboBox_magazyn->clear();
    query.prepare(QString("SELECT id_rodzaj_materialu, nazwa FROM Rodzaj_materialu;"));
    query.exec();
    query.first();
    do{
        ui->MaterialComboBox_magazyn->insertItem(0, query.value("nazwa").toString(), query.value("id_rodzaj_materialu").toInt());
    }while(query.next());
}

void MainWindow::fillMaterialListFromDataBase()
{
    ui->list_material->clear();
    QSqlQuery query;
    query.prepare(QString("SELECT * FROM Rodzaj_materialu;"));
    query.exec();
    query.first();
    if(!query.isValid()) return;
    unsigned numerator = 0;
    do{
        QString position_name = QString::number(++numerator) + ".  " + query.value("nazwa").toString() + "  -  " +
                                query.value("kolor").toString() + "  -  " + query.value("grubosc").toString() + " cm  -  " +
                                query.value("cena_m2").toString() + " zł/m2";
        QListWidgetItem *it = new QListWidgetItem(position_name);
        it->setWhatsThis(query.value("id_rodzaj_materialu").toString()); // ukryta zmienna z ID płyty
        ui->list_material->insertItem(ui->list_material->count(), it);
    } while(query.next());
}

void MainWindow::fillMebelListFromDataBase()
{
    ui->listMebel->clear();
    ui->materialComboBox_element->clear();
    QSqlQuery query;
    query.prepare(QString("SELECT * FROM Mebel"));
    query.exec();
    query.first();
    if(!query.isValid()) return;
    unsigned numerator = 0;
    do{
        QListWidgetItem *it = new QListWidgetItem(QString::number(++numerator) + ".  " + query.value("nazwa").toString());
        it->setWhatsThis(query.value("id_mebel").toString()); // ukryta zmienna z ID płyty
        ui->listMebel->insertItem(ui->listMebel->count(), it);
    } while(query.next());
    query.prepare(QString("SELECT id_rodzaj_materialu, nazwa FROM Rodzaj_materialu;"));
    query.exec();
    query.first();
    if(!query.isValid()) return;
    do{
        ui->materialComboBox_element->insertItem(0, query.value("nazwa").toString(), query.value("id_rodzaj_materialu").toInt());
    }while(query.next());
}

void MainWindow::fillCuttingMebelListFromDataBase()
{
    ui->mebelComboBox_cut->clear();
    QSqlQuery query;
    query.prepare(QString("SELECT * FROM Mebel;"));
    query.exec();
    query.first();
    if(!query.isValid()) return;
    do{
        ui->mebelComboBox_cut->insertItem(0, query.value("nazwa").toString(), query.value("id_mebel").toInt());
    }while(query.next());

}

void MainWindow::updateAllViews()
{
    fillMagazynListFromDataBase();
    fillMaterialListFromDataBase();
    fillMebelListFromDataBase();
    fillCuttingMebelListFromDataBase();
}

//---------------------------------------------------------------------------------------------------------------------------MAGAYZN
void MainWindow::viewSelectedRowMagazyn()
{
    if(ui->list_magazyn->selectedItems().count() == 0) return;
    int selected_id = ui->list_magazyn->currentItem()->whatsThis().toInt();
    QSqlQuery query;
    query.prepare(QString("SELECT wysokosc, szerokosc, id_rodzaj_materialu FROM Plyta "
                          "WHERE id_plyta = :id_val;"));
    query.bindValue(":id_val", selected_id);
    query.exec();
    query.first();
    ui->HeightDoubleSpinBox_magazyn->setValue(query.value("wysokosc").toDouble());
    ui->widthDoubleSpinBox_magazyn->setValue(query.value("szerokosc").toDouble());
    ui->MaterialComboBox_magazyn->setCurrentIndex(ui->MaterialComboBox_magazyn->findData(query.value("id_rodzaj_materialu")));
}

void MainWindow::modifySelectedRowMagazyn()
{
    if(ui->list_magazyn->selectedItems().count() == 0) return;
    int selected_id = ui->list_magazyn->currentItem()->whatsThis().toInt();
    QSqlQuery query;
    query.prepare("UPDATE Plyta SET "
                  "wysokosc = :wysokosc_val, "
                  "szerokosc = :szerokosc_val, "
                  "id_rodzaj_materialu = :id_rodzaj_materialu_val "
                  "WHERE id_plyta = :plyta_id_val;");
    query.bindValue(":wysokosc_val", ui->HeightDoubleSpinBox_magazyn->value());
    query.bindValue(":szerokosc_val", ui->widthDoubleSpinBox_magazyn->value());
    query.bindValue(":id_rodzaj_materialu_val", ui->MaterialComboBox_magazyn->currentData().toInt());
    query.bindValue(":plyta_id_val", selected_id);
    query.exec();
    updateAllViews();
}

void MainWindow::addNewRowMagazyn()
{
    if(ui->MaterialComboBox_magazyn->count() < 1) return;
    int initMaterial_ID = ui->MaterialComboBox_magazyn->currentData().toInt();
    QSqlQuery query;
    query.prepare("INSERT INTO Plyta (wysokosc, szerokosc, id_rodzaj_materialu) VALUES (:wysokosc_val, :szerokosc_val, :id_rodzaj_materialu_val);");
    query.bindValue(":id_rodzaj_materialu_val", initMaterial_ID);
    query.bindValue(":wysokosc_val", ui->HeightDoubleSpinBox_magazyn->value());
    query.bindValue(":szerokosc_val", ui->widthDoubleSpinBox_magazyn->value());
    query.exec();
    updateAllViews();

}

void MainWindow::deleteRowMagazyn()
{
    if(ui->list_magazyn->selectedItems().count() == 0) return;
    int selected_id = ui->list_magazyn->currentItem()->whatsThis().toInt();
    QSqlQuery query;
    query.prepare("DELETE FROM Plyta WHERE id_plyta = :plyta_id_val;");
    query.bindValue(":plyta_id_val", selected_id);
    query.exec();
    updateAllViews();
}
//--------------------------------------------------------------------------------------------------------------------------MATERIALY
void MainWindow::viewSelectedRowMaterial()
{
    if(ui->list_material->selectedItems().count() == 0) return;
    int selected_id = ui->list_material->currentItem()->whatsThis().toInt();
    QSqlQuery query;
    query.prepare(QString("SELECT nazwa, grubosc, kolor, cena_m2 FROM Rodzaj_materialu "
                          "WHERE id_rodzaj_materialu = :id_val;"));
    query.bindValue(":id_val", selected_id);
    query.exec();
    query.first();
    ui->nameLineEdit_material->setText(query.value("nazwa").toString());
    ui->colorLineEdit_material->setText(query.value("kolor").toString());
    ui->thicnessSpinBox_material->setValue(query.value("grubosc").toDouble());
    ui->priceSpinBox_material->setValue(query.value("cena_m2").toDouble());
}

void MainWindow::modifySelectedRowMaterial()
{
    if(ui->list_material->selectedItems().count() == 0) return;
    int selected_id = ui->list_material->currentItem()->whatsThis().toInt();
    QSqlQuery query;
    query.prepare("UPDATE Rodzaj_materialu SET"
                  " nazwa = :nazwa_val,"
                  " grubosc = :grubosc_val,"
                  " kolor = :kolor_val,"
                  " cena_m2 = :cena_m2"
                  " WHERE id_rodzaj_materialu = :id_rodzaj_materialu_val;");
    query.bindValue(":nazwa_val", ui->nameLineEdit_material->text());
    query.bindValue(":grubosc_val", ui->thicnessSpinBox_material->value());
    query.bindValue(":kolor_val", ui->colorLineEdit_material->text());
    query.bindValue(":cena_m2", ui->priceSpinBox_material->value());
    query.bindValue(":id_rodzaj_materialu_val", selected_id);
    query.exec();
    updateAllViews();
}

void MainWindow::addNewRowMaterial()
{
    QSqlQuery query;
    query.prepare("INSERT INTO Rodzaj_materialu (nazwa, grubosc, kolor, cena_m2) VALUES (:nazwa_val, :grubosc_val, :kolor_val, :cena_m2_val);");
    query.bindValue(":nazwa_val", ui->nameLineEdit_material->text().count() > 0 ? ui->nameLineEdit_material->text() : "nowy materiał");
    query.bindValue(":grubosc_val", ui->thicnessSpinBox_material->value());
    query.bindValue(":kolor_val", ui->colorLineEdit_material->text());
    query.bindValue(":cena_m2_val", ui->priceSpinBox_material->value());
    query.exec();
    updateAllViews();
}

void MainWindow::deleteRowMaterial()
{
    if(ui->list_material->selectedItems().count() == 0) return;
    int selected_id = ui->list_material->currentItem()->whatsThis().toInt();
    QSqlQuery query;
    query.prepare("DELETE FROM Rodzaj_materialu WHERE id_rodzaj_materialu = :id_rodzaj_materialu_val;");
    query.bindValue(":id_rodzaj_materialu_val", selected_id);
    query.exec();
    updateAllViews();
}
//-----------------------------------------------------------------------------------------------------------------------------PROJEKTY
void MainWindow::viewSelectedRowMebel()
{
    if(ui->listMebel->selectedItems().count() == 0) return;qDebug() << ui->listMebel->currentItem()->text();
    int selected_id = ui->listMebel->currentItem()->whatsThis().toInt();
    QSqlQuery query;
    query.prepare("SELECT nazwa FROM Mebel WHERE id_mebel = :id_mebel_val;");
    query.bindValue(":id_mebel_val", selected_id);
    query.exec();
    query.first();
    ui->nameLineEdit_mebel->setText(query.value("nazwa").toString());
    query.prepare("SELECT * FROM Sciana WHERE id_mebel = :id_mebel_val;");
    query.bindValue(":id_mebel_val", selected_id);
    query.exec();
    query.first();

    ui->listElement->clear();
    if(query.record().isNull(0)) return;
    unsigned numerator = 0;
    do{
        QString materialName = ui->materialComboBox_element->itemText(ui->materialComboBox_element->findData(
            query.value("id_rodzaj_materialu").toInt()));
        QString position_name = QString::number(++numerator) + ".  " + query.value("nazwa").toString() + "  -  " +
                                query.value("wysokosc").toString() + " cm x " +
                                query.value("szerokosc").toString() + " cm  -  " + materialName;
        QListWidgetItem *it = new QListWidgetItem(position_name);
        it->setWhatsThis(query.value("id_sciana").toString()); // ukryta zmienna z ID sciany
        ui->listElement->insertItem(ui->listElement->count(), it);
    }while(query.next());
}

void MainWindow::viewSelectedRowElement()
{
    if(ui->listElement->selectedItems().count() == 0) return;
    int selected_id = ui->listElement->currentItem()->whatsThis().toInt();
    QSqlQuery query;
    query.prepare("SELECT nazwa, wysokosc, szerokosc, id_rodzaj_materialu FROM Sciana WHERE id_sciana = :id_sciana_val;");
    query.bindValue(":id_sciana_val", selected_id);
    query.exec();
    query.first();
    ui->nameLineEdit_element->setText(query.value("nazwa").toString());
    ui->heightSpinBox_element->setValue(query.value("wysokosc").toDouble());
    ui->widthSpinBox_element->setValue(query.value("szerokosc").toDouble());
    ui->materialComboBox_element->setCurrentIndex(ui->materialComboBox_element->findData(query.value("id_rodzaj_materialu").toInt()));
}

void MainWindow::modifySelectedRowElementAndMebel()
{
    if(ui->listMebel->selectedItems().count() == 0) return;
    int selectedMebel_id = ui->listMebel->currentItem()->whatsThis().toInt();
    int last_row_mebel = ui->listMebel->currentRow();
    QSqlQuery query;
    query.prepare("UPDATE Mebel SET "
                  "nazwa = :nazwa_val "
                  "WHERE id_mebel = :id_mebel_val;");
    query.bindValue(":nazwa_val", ui->nameLineEdit_mebel->text());
    query.bindValue(":id_mebel_val", selectedMebel_id);
    query.exec();
    if(ui->listElement->selectedItems().count() == 0){
        updateAllViews();
        return;
    }
    int selectedElement_id = ui->listElement->currentItem()->whatsThis().toInt();
    query.prepare("UPDATE Sciana SET "
                  "nazwa = :nazwa_val, "
                  "wysokosc = :wysokosc_val, "
                  "szerokosc = :szerokosc_val, "
                  "id_rodzaj_materialu = :id_rodzaj_materialu_val "
                  "WHERE id_sciana = :id_sciana_val;");
    query.bindValue(":nazwa_val", ui->nameLineEdit_element->text());
    query.bindValue(":wysokosc_val", ui->heightSpinBox_element->value());
    query.bindValue(":szerokosc_val", ui->widthSpinBox_element->value());
    query.bindValue(":id_rodzaj_materialu_val", ui->materialComboBox_element->currentData().toInt());
    query.bindValue(":id_sciana_val", selectedElement_id);
    query.exec();

    //viewSelectedRowMebel();
    updateAllViews();
    ui->listMebel->setCurrentRow(last_row_mebel);
}

void MainWindow::addNewRowMebel()
{
    QSqlQuery query;
    query.prepare("INSERT INTO Mebel"
                  " (nazwa)"
                  " VALUES (:nazwa_val);");
    query.bindValue(":nazwa_val", ui->nameLineEdit_mebel->text().count() > 0 ? ui->nameLineEdit_mebel->text() : "nowy mebel");
    query.exec();
    updateAllViews();
}

void MainWindow::addNewRowElement()
{
    if(ui->materialComboBox_element->count() < 1) return;
    if(ui->listMebel->currentItem() == nullptr) return;
    QSqlQuery query;
    query.prepare("INSERT INTO Sciana"
                  " (nazwa, wysokosc, szerokosc, id_mebel, id_rodzaj_materialu)"
                  " VALUES (:nazwa_val, :wysokosc_val, :szerokosc_val, :id_mebel_val, :id_rodzaj_materialu_val);");
    qDebug() << ui->materialComboBox_element->itemData(0).toInt();
    query.bindValue(":id_mebel_val", ui->listMebel->currentItem()->whatsThis().toInt());
    query.bindValue(":id_rodzaj_materialu_val", ui->materialComboBox_element->currentData().toInt());
    query.bindValue(":nazwa_val", ui->nameLineEdit_element->text().count() > 0 ? ui->nameLineEdit_element->text() : "nowy element");
    query.bindValue(":wysokosc_val", ui->heightSpinBox_element->value());
    query.bindValue(":szerokosc_val", ui->widthSpinBox_element->value());
    query.exec();
    viewSelectedRowMebel();
}

void MainWindow::deleteRowMebel()
{
    if(ui->listMebel->selectedItems().count() == 0) return;
    int selected_id = ui->listMebel->currentItem()->whatsThis().toInt();
    QSqlQuery query;
    query.prepare("DELETE FROM Mebel WHERE id_mebel= :id_mebel_val;");
    query.bindValue(":id_mebel_val", selected_id);
    query.exec();
    updateAllViews();
}

void MainWindow::deleteRowElement()
{
    if(ui->listElement->selectedItems().count() == 0) return;
    int selected_id = ui->listElement->currentItem()->whatsThis().toInt();
    QSqlQuery query;
    query.prepare("DELETE FROM Sciana WHERE id_sciana= :id_sciana_val;");
    query.bindValue(":id_sciana_val", selected_id);
    query.exec();
    viewSelectedRowMebel();
}
//----------------------------------------------------------------------------------------------------------------------------CUTTING
void MainWindow::viewSelectedRowCuttingMebel(int)
{
    if(ui->mebelComboBox_cut->currentData().isNull()) return;
    int selected_id = ui->mebelComboBox_cut->currentData().toInt();
    QSqlQuery query;
    query.prepare("SELECT id_sciana, Sciana.nazwa, wysokosc, szerokosc, Rodzaj_materialu.nazwa "
                  "FROM Sciana INNER JOIN Rodzaj_materialu ON "
                  "Rodzaj_materialu.id_rodzaj_materialu = Sciana.id_rodzaj_materialu "
                  "WHERE id_mebel = :id_mebel_val;");
    query.bindValue(":id_mebel_val", selected_id);
    query.exec();
    query.first();
    ui->elementListWidget_cut->clear();
    if(query.record().isNull(0)) return;
    unsigned numerator = 0;
    do{
        QString positionName = QString::number(++numerator) + ".  " + query.value("Sciana.nazwa").toString() + "  -  " +
                               query.value("wysokosc").toString() + " cm x " + query.value("szerokosc").toString() + " cm  -  " +
                               query.value("Rodzaj_materialu.nazwa").toString();
        QListWidgetItem *it = new QListWidgetItem(positionName);
        it->setWhatsThis(query.value("id_sciana").toString()); // ukryta zmienna z ID sciany
        ui->elementListWidget_cut->insertItem(ui->elementListWidget_cut->count(), it);
    }while(query.next());
    ui->elementListWidget_cut->setSelectionMode(QAbstractItemView::NoSelection);
    ui->elementListWidget_cut->setFocusPolicy(Qt::NoFocus);
}

void MainWindow::planCutting() {
    qDebug() << "Plan Cutting: Start";
    if(ui->mebelComboBox_cut->currentData().isNull()) {
        qDebug() << "Plan Cutting: Furniture to cut not selected";
        return;
    }
    unsigned int id_furniture = ui->mebelComboBox_cut->currentData().toInt();
    qDebug() << "Plan Cutting: Selected furniture ID: " << id_furniture;
    QSqlQuery query;
    /* Zresetowanie struktury do planowania rozkroju */
    cutter.start_new_planning(id_furniture);
    /* Pobranie listy elementów danego mebla */
    query.prepare(QString("SELECT * FROM Sciana WHERE id_mebel=") + QString::number(id_furniture) + QString(";"));
    query.exec();
    query.first();
    if(!query.isValid()) {
        qDebug() << "Plan Cutting: Invalid query - get furniture parts";
        return;
    }
    do {
        cutter.add_furniture_element(query.value("id_sciana").toInt(),
                                     query.value("nazwa").toString().toStdString(),
                                     query.value("szerokosc").toFloat(),
                                     query.value("wysokosc").toFloat(),
                                     query.value("id_rodzaj_materialu").toInt());
    } while(query.next());
    /* Pobranie stanów magazynowych materiałów wykorzystanych w analizowanym meblu */
    for(auto id_type : cutter.get_used_material_type_ids()) {
        query.prepare(QString("SELECT * FROM Plyta WHERE id_rodzaj_materialu=") + QString::number(id_type) + QString(";"));
        query.exec();
        query.first();
        if(!query.isValid())
            continue; // Brak podanego rodzaju płyty w magazynie
        do {
            cutter.add_stored_component(query.value("id_plyta").toInt(),
                                        query.value("szerokosc").toFloat(),
                                        query.value("wysokosc").toFloat(),
                                        query.value("id_rodzaj_materialu").toInt());
        } while(query.next());
    }
    /* Zaplanowanie cięć */
    qDebug() << "Plan Cutting: Cut planning algorithm - Start";
    cutter.plan();
    qDebug() << "Plan Cutting: Cut planning algorithm - Done";
    /* Wyświetlenie wszystkich pociętych płyt */
    if(cutter.is_planning_valid()) {
        qDebug() << "Plan Cutting: Cut visualization - Start";
        viewPlannedCutting(cutter.get_components_with_cuts());
        qDebug() << "Plan Cutting: Cut visualization - Done";
        ui->cuttind_info->setText("");
    } else {
        qDebug() << "Plan Cutting: Insufficient material in stock - Cancel visualization";
        ui->cuttind_info->setText("Niewystarczająca ilość materiałów w magazynie");
        ui->listWidget->clear();
    }
    qDebug() << "Plan Cutting: Done";
}

void MainWindow::executeCutting() {
    qDebug() << "Execute Cutting: Locking database";
    lockDB();
    qDebug() << "Execute Cutting: Locked database";
    qDebug() << "Execute Cutting: Start";
    unsigned int id_furniture = ui->mebelComboBox_cut->currentData().toInt();
    /* Sprawdzenie czy cięcie wybranego mebla zostało zaplanowane */
    if(id_furniture != cutter.get_furniture_id()) {
        qDebug() << "Execute Cutting: Cutting of current furniture not planned";
        ui->cuttind_info->setText("Cięcie wybranego mebla nie zostało zaplanowane");
        return;
    }
    qDebug() << "Execute Cutting: Furniture ID: " << id_furniture;
    /* Sprawdzenie czy planowanie zakończyło się powodzeniem - wszystkie potrzebne elementy znajdują się w magazynie*/
    if(!cutter.is_planning_valid()) {
        qDebug() << "Execute Cutting: Not all needed elements were in storage during planning - Add materials and rerun planning";
        ui->cuttind_info->setText("Niewystarczająca ilość materiałów w magazynie");
        return;
    } 
    /* Sprawdzenie czy wszystkie potrzebne elementy są dostępne w magazynie */
    std::list<unsigned int> required_elements_ids = cutter.get_used_stored_components_ids();
    QSqlQuery query;
    query.prepare(QString("SELECT id_plyta FROM Plyta;"));
    query.exec();
    query.first();
    if(!query.isValid()) return;
    do {
        required_elements_ids.remove(query.value("id_plyta").toInt());
    } while(query.next());
    if(!required_elements_ids.empty()) {
        qDebug() << "Execute Cutting: Not all elements needed for cutting are in storage";
        ui->cuttind_info->setText("Niewystarczająca ilość materiałów w magazynie");
        return;
    }
    ui->cuttind_info->setText("");
    /* Usunięcie wykorzystanych elementów z magazynu */
    required_elements_ids = cutter.get_used_stored_components_ids();
    for(auto id_material : required_elements_ids) {
        query.prepare("DELETE FROM Plyta WHERE id_plyta= :id_material;");
        query.bindValue(":id_material", id_material);
        query.exec();
    }
    /* Dodanie do magazynu wszystkich odpadów o odpowiednio dużej wielkości */
    std::list<Leftover> leftovers = cutter.get_created_leftovers();
    for(auto plate : leftovers) {
        query.prepare("INSERT INTO Plyta (wysokosc, szerokosc, id_rodzaj_materialu) VALUES ( :height, :width, :id_material_type);");
        query.bindValue(":height", plate.height);
        query.bindValue(":width", plate.width);
        query.bindValue(":id_material_type", plate.id_material_type);
        query.exec();
    }
    qDebug() << "Execute Cutting: Done";
    qDebug() << "Execute Cutting: Unlocking database";
    unlockDB();
    qDebug() << "Execute Cutting: Unlocked database";
    /* Zaktualizowanie widoków - aktualizowanie listy elementów w magazynie */
    updateAllViews();
 
}

void MainWindow::viewPlannedCutting(std::list<Component_with_cuts> cutData)
{
    ui->listWidget->clear();
    QSqlQuery query;
    unsigned numerator = 0;
    for(Component_with_cuts &component : cutData){
        query.prepare("SELECT nazwa, kolor FROM Rodzaj_materialu WHERE id_rodzaj_materialu = :id_rodzaj_materialu_val;");
        query.bindValue(":id_rodzaj_materialu_val", component.material_type_id);
        query.exec();
        query.first();
        QListWidgetItem *it = new QListWidgetItem();
        TileCutView *newTile = new TileCutView(ui->listWidget);
        newTile->setTileData(component, QString::number(++numerator) + ".  " + query.value("nazwa").toString() + "\n" +
                             "    " + QString::number(component.component_outline.height) + " cm x " +
                                      QString::number(component.component_outline.width) + " cm" + "\n" +
                             "    " + query.value("kolor").toString());
        it->setSizeHint(QSize(component.component_outline.width * 2.5,360));
        ui->listWidget->insertItem(ui->listWidget->count(), it);
        ui->listWidget->setItemWidget(it, newTile);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------LOGOWANIE
void MainWindow::logInDB() {
    m_dataBase = QSqlDatabase::addDatabase("QMYSQL");
    m_dataBase.setHostName(ui->address->text());
    m_dataBase.setDatabaseName("stolarnia");
    m_dataBase.setUserName(ui->login->text());
    m_dataBase.setPassword(ui->password->text());
    if(!m_dataBase.open()) {
        ui->login_info->setText("Nieprawidłowa nazwa użytkownika lub hasło");
        qDebug() << "Błąd przy łączeniu z bazą danych - Nieprawidłowa nazwa użytkownika, hasło lub adres";
        return;
    }

    ui->tabWidget->setTabEnabled(0, false);
    for(unsigned int i = 1; i < 5; ++i)
        ui->tabWidget->setTabEnabled(i, true);

    QSqlQuery query;
    query.prepare("PRAGMA foreign_keys = ON;");
    query.exec();

    updateAllViews();

    qDebug() << "Połączono z bazą danych";
}

//-----------------------------------------------------------------------------------------------------------------------------BLOKOWANIE/ODBLOKOWYWANIE
void MainWindow::lockDB() {
    QSqlQuery query;
    query.prepare("LOCK TABLE Mebel WRITE, Plyta WRITE, Rodzaj_materialu WRITE, Sciana WRITE;");
    query.exec();
}

void MainWindow::unlockDB() {
    QSqlQuery query;
    query.prepare("UNLOCK TABLES;");
    query.exec();
}