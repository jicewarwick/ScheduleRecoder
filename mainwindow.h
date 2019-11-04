#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCoreApplication>
#include <QTabWidget>
#include <QString>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QCloseEvent>
#include <QTableView>
#include <QWidget>
#include <QSettings>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QGroupBox>

#include "ui_mainwindow.h"

const QString DBLOC = "data.sqlite";


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::mainwindow ui;

    bool _gonna_close = false;

    QSettings _settings;
    QIcon* _icon;
    QSystemTrayIcon* _tray_icon;
    QMenu* _tray_menu;

    QSqlTableModel* _records_model;

    QSqlTableModel* _log;

    void _init_db();
    void _init_gui();

private slots:
    void start_job();
    void finish_job();

};

#endif // MAINWINDOW_H
