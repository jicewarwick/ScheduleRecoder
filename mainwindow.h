#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QCoreApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>
#include <string>

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

   protected:
    void closeEvent(QCloseEvent* event) override;

   private:
    Ui::mainwindow _ui;

    bool _gonna_close = true;
    bool _sound_effect;
    int _poromodo_duration;
    int _short_break_duration;
    int _long_break_duration;
    QString _db_loc;

    QSettings* _settings;
    QIcon* _icon;
    QSystemTrayIcon* _tray_icon;
    QMenu* _tray_menu;

    QSqlTableModel* _records_model;
    QSqlTableModel* _log;

    // functions
    void _init_db();
    void _init_gui();
    void _create_connections();

    void _read_settings();
    void _write_settings();

   private slots:
    // settings
    void poromodo_duration_change(int);
    void short_break_duration_change(int);
    void long_break_duration_change(int);
    void db_loc_change(QString);
    void sound_effect_change(int);

    void start_job();
    void finish_job();
    void quit_win();
    void activate_or_hide_window(QSystemTrayIcon::ActivationReason);
};

#endif	// MAINWINDOW_H
