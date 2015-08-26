#include "settings.h"
#include "ui_settings.h"

#include <QFileDialog>


using namespace std;

Settings::Settings(string *directory, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    this->savedirectory = directory;

    ui->lineEdit->setText(QString::fromStdString(*directory));
}

Settings::~Settings()
{
    delete ui;
}



void Settings::on_chooseDirectorypushButton_clicked()
{
    QString dname = QFileDialog::getExistingDirectory(this, "Save directory", "/home", QFileDialog::ShowDirsOnly);
    ui->lineEdit->setText(dname);
}

void Settings::on_okpushButton_clicked()
{
    QString d = ui->lineEdit->text();
    *savedirectory = d.toUtf8().constData();
    this->close();
}

void Settings::on_cancelpushButton_clicked()
{
    this->close();
}
