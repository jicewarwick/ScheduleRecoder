#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCoreApplication>
#include <QString>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QCloseEvent>
#include <QWidget>
#include <QSettings>
#include <QMessageBox>

#include "ui_mainwindow.h"

const QString DBLOC = "./db.sqlite";


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::mainwindow _ui;

    bool _gonna_close = false;

    QSettings _settings;
    QIcon* _icon;
    QSystemTrayIcon* _tray_icon;
    QMenu* _tray_menu;

    QSqlTableModel* _records_model;
    QSqlTableModel* _log;

    void _init_db();
    void _init_gui();
    void _create_connections();

signals:
    void config_changed();

private slots:
    void start_job();
    void finish_job();
    void quit_win();
    void activate_hide_window();

};

#endif // MAINWINDOW_H
