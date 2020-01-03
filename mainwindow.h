#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QCompleter>
#include <QMainWindow>
#include <QSettings>
#include <QSqlTableModel>
#include <QStringList>
#include <QStringListModel>
#include <QSystemTrayIcon>
#include <map>
#include <QTimer>

#include "hashtagcompleter.h"
#include "pomodoro.h"
#include "ui_mainwindow.h"

using std::map;

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
    void onMinimumLoggingDurationChange(int);

    void onQuitWindow();
    void onClickTray(QSystemTrayIcon::ActivationReason);

    void onPoromodoTimeLeftStr(QString s);
    void onStatusChange(Pomodoro::Status s);
    void onStartPorodomoPorcess();
    void onPuaseUnpause();
    void onStopPorodomoPorcess();
    void onStatusChangedNotification(Pomodoro::Status s);

    void onLogEntryChange(QModelIndex index);
    void onReminderPopup();

private:
    Ui::mainwindow ui_;

    bool gonna_close_ = false;

    // settings
    bool sound_effect_;
    bool tray_popup_;
    bool start_minimized_;
    QSettings* settings_;

    const QIcon* title_icon_ = new QIcon(":/icons/icon.png");
    const QIcon* start_icon_ = new QIcon(":/icons/start.png");
    const QIcon* pause_icon_ = new QIcon(":/icons/pause.png");
    QSystemTrayIcon* tray_icon_;
    QMenu* tray_menu_;

    QSqlTableModel* records_model_;
    QSqlTableModel* log_model_;

    Pomodoro* poromodo_;
    QTimer* timer_;
    minutes reminder_interval{5};

    QStringList activities_;
    QStringList categories_;
    QStringListModel* hashtags_;
    HashtagCompleter* hashtag_completer_;

    const map<Pomodoro::Status, QString> StatusInfo = {{Pomodoro::Status::NONE, tr("READY")},
                                                       {Pomodoro::Status::POMODORO, tr("POMODORO")},
                                                       {Pomodoro::Status::SHORT_BREAK, tr("SHORT BREAK")},
                                                       {Pomodoro::Status::LONG_BREAK, tr("LONG BREAK")},
                                                       {Pomodoro::Status::PAUSE, tr("(PAUSED)")}};

    // functions
    void InitDB();
    void InitGUI();
    void CreateConnections();

    void ReadSettings();
    void WriteSettings();
    static void SettingUpTableView(QTableView* view);
};

#endif	// MAINWINDOW_H
