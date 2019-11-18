#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QCoreApplication>
#include <QDebug>
#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <QSound>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QSystemTrayIcon>
#include <QWidget>
#include <string>
#include <vector>

#include "poromodo.h"
#include "ui_mainwindow.h"

using std::string;
using std::vector;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::mainwindow ui_;

    bool gonna_close_ = true;
    bool sound_effect_;

    QSettings* settings_;
    QIcon* icon_;
    QSystemTrayIcon* tray_icon_;
    QMenu* tray_menu_;

    QSqlTableModel* records_model_;
    QSqlTableModel* log_;

    Poromodo* poromodo_;

    // functions
    void InitDB();
    void InitGUI();
    void CreateConnections();

    void ReadSettings();
    void WriteSettings();

    void PlaySound();

private slots:
    // settings
    void onPoromodoDurationChange(int);
    void onShortBreakDurationChange(int);
    void onLongBreakdurationChange(int);
    void onSoundEffectStatusChange(int);

    void onPuaseUnpause();

    void onQuitWindow();
    void onClickTray(QSystemTrayIcon::ActivationReason);

    void onStatusChange(Poromodo::Status s);
};

#endif	// MAINWINDOW_H
