#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QCompleter>
#include <QCoreApplication>
#include <QDebug>
#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <QSound>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QStringList>
#include <QSystemTrayIcon>
#include <QWidget>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "poromodo.h"
#include "ui_mainwindow.h"

using std::map;
using std::set;
using std::string;
using std::vector;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    // settings
    void onPoromodoDurationChange(int);
    void onShortBreakDurationChange(int);
    void onLongBreakdurationChange(int);

    void onQuitWindow();
    void onClickTray(QSystemTrayIcon::ActivationReason);

    void onStatusChange(Poromodo::Status s);
    void onStartPorodomoPorcess();
    void onPuaseUnpause();
    void onStopPorodomoPorcess();
    void onStatusChangedNotification(Poromodo::Status s);

private:
    Ui::mainwindow ui_;

    bool gonna_close_ = false;

    // settings
    bool sound_effect_;
    bool popup_messagebox_;
    bool tray_popup_;
    QSettings* settings_;

    const QIcon* title_icon_ = new QIcon(":/icons/icon.png");
    const QIcon* start_icon_ = new QIcon(":/icons/start.png");
    const QIcon* pause_icon_ = new QIcon(":/icons/pause.png");
    QSystemTrayIcon* tray_icon_;
    QMenu* tray_menu_;

    QSqlTableModel* records_model_;
    QSqlTableModel* log_model_;

    Poromodo* poromodo_;

    QStringList activities_;
    QStringList categories_;
    QStringList hashtags_;
    QCompleter* hashtag_completer_;

    const map<Poromodo::Status, QString> StatusInfo = {{Poromodo::Status::NONE, tr("READY")},
                                                       {Poromodo::Status::POROMODO, tr("POROMODO")},
                                                       {Poromodo::Status::SHORT_BREAK, tr("SHORT BREAK")},
                                                       {Poromodo::Status::LONG_BREAK, tr("LONG BREAK")},
                                                       {Poromodo::Status::PAUSE, tr("(PAUSED)")}};

    // functions
    void InitDB();
    void InitGUI();
    void CreateConnections();

    void ReadSettings();
    void WriteSettings();
};

#endif	// MAINWINDOW_H
