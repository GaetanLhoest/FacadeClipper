#ifndef FACADECLIPPER_H
#define FACADECLIPPER_H

#include <QMainWindow>
#include <QStringListModel>
#include <QGraphicsScene>
#include <iostream>


namespace Ui {
class FacadeClipper;
}

/**
 * @brief The FacadeClipper class
 *
 * The FacadeClipper class contain a GUI proposing different methods that aim to
 * segment facades images in order to separate different elements.
 */
class FacadeClipper : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief FacadeClipper
     *
     * Constructor of the FacadeClipper class.
     *
     * @param parent QWidget parent
     */
    explicit FacadeClipper(QWidget *parent = 0);
    ~FacadeClipper();

    void setSaveDirectory(std::string directory);

private slots:
    void on_mainApplyButton_clicked();

    void on_actionOuvrir_triggered();

    void on_listView_clicked(const QModelIndex &index);

    void on_listView_pressed(const QModelIndex &index);

    virtual void resizeEvent(QResizeEvent *event);

    void on_actionSettings_triggered();

private:
    Ui::FacadeClipper *ui;
    QString *currentName;
    QStringList *namelist;
    QStringListModel *model;
    QGraphicsScene *scene;
    std::string *savedirectory;

    /**
     * @brief Display Image
     *
     * Display the given opencv Mat in the QDialog
     *
     * @param m Opencv Mat to display
     */
    void displayImage(const QString s);

};


#endif // FACADECLIPPER_H
