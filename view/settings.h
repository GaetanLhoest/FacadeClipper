#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QWidget>

#include <iostream>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings( std::string *directory, QWidget *parent = 0);
    ~Settings();

private slots:


    void on_chooseDirectorypushButton_clicked();

    void on_okpushButton_clicked();

    void on_cancelpushButton_clicked();

private:
    Ui::Settings *ui;
    QWidget *parent;
    std::string *savedirectory;
};

#endif // SETTINGS_H
