#include "mainwindow.h"

void MainWindow::_init_gui() {
    _icon = new QIcon(":/icon.png");
    this->setWindowIcon(*_icon);

    _tray_menu = new QMenu();
    _tray_menu->addAction(_ui.actionStart_Job);
    _tray_menu->addAction(_ui.actionFinish_Job);
    _tray_menu->addAction(_ui.action_Quit);

    _tray_icon = new QSystemTrayIcon(*_icon, this);
    _tray_icon->setContextMenu(_tray_menu);
    _tray_icon->show();

    _ui._records_view->setModel(_records_model);
    _ui._records_view->hideColumn(0);
    _ui._records_view->show();

}

void MainWindow::_create_connections() {
    connect(_ui.action_Quit, &QAction::triggered,
            this, &MainWindow::quit_win);
    connect(_tray_icon, &QSystemTrayIcon::activated, [this](auto reason){
        if (reason == QSystemTrayIcon::Trigger) {
            if (isVisible()) {
                hide();
            } else {
                show();
                activateWindow();
            }
        }
    });
}


void MainWindow::start_job() {

}

void MainWindow::finish_job() {

}

void MainWindow::quit_win() {
    _gonna_close = true;
    this->close();

}

void MainWindow::activate_hide_window() {

}

void MainWindow::_init_db() {
    QSqlDatabase conn = QSqlDatabase::addDatabase("QSQLITE");
    conn.setDatabaseName(_settings.value("database/db_loc", "db.sqlite").toString());
    _records_model = new QSqlTableModel(this, conn);
    _records_model->setTable("Records");
    _log = new QSqlTableModel(this, conn);

    _records_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _records_model->select();
    _records_model->setHeaderData(0, Qt::Horizontal, tr("Start Time"));
    _records_model->setHeaderData(1, Qt::Horizontal, tr("End Time"));
    _records_model->setHeaderData(2, Qt::Horizontal, tr("Job"));

}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QCoreApplication::setOrganizationName("CJSoft");
    QCoreApplication::setOrganizationDomain("CJSoft.com");
    QCoreApplication::setApplicationName("ScheduleRecorder");
    _ui.setupUi(this);

    _init_db();
    _init_gui();
}

MainWindow::~MainWindow() {
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (_gonna_close) {
        event->accept();
    } else {
        this->hide();
        _tray_icon->showMessage("Hiding", "Programe is hiding");
        event->ignore();
    }

}
