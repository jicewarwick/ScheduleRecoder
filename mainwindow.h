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

const QString DBLOC = "data.sqlite";


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    bool _gonna_close = false;

    QSettings _settings;
    QIcon* _icon;
    QSystemTrayIcon* _tray_icon;
    QMenu* _tray_menu;
    QMenuBar* _menubar;

    QSqlTableModel* _records_model;
    QTableView* _records_view;

    QSqlTableModel* _log;
    QTableView* _log_view;

    void _init_db();
    void _init_gui();
    void _create_menu();

private slots:
    void start_job();
    void finish_job();

};


class StartJobMessageBox : public QWidget {
    Q_OBJECT
public:
    StartJobMessageBox(){}
    ~StartJobMessageBox(){}

private:

};

#endif // MAINWINDOW_H
