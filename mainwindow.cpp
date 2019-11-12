#include "mainwindow.h"

////////////////////////////////////////////////////////////////////////////////
/// public functions
////////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    QCoreApplication::setOrganizationName("CJSoft");
    QCoreApplication::setOrganizationDomain("CJSoft.com");
    QCoreApplication::setApplicationName("ScheduleRecorder");
    _ui.setupUi(this);

    _poromodo = new Poromodo(this);
    _read_settings();
    _init_db();
    _init_gui();
    _create_connections();
}

////////////////////////////////////////////////////////////////////////////////
/// protected functions
////////////////////////////////////////////////////////////////////////////////
void MainWindow::closeEvent(QCloseEvent* event) {
    if (_gonna_close) {
        qDebug() << "Really closing";
        _write_settings();
        event->accept();
    } else {
        qDebug() << "Not really closing";
        this->hide();
        _tray_icon->showMessage("Hiding", "Programe is hiding");
        event->ignore();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// private functions
////////////////////////////////////////////////////////////////////////////////
void MainWindow::_init_db() {
    QSqlDatabase conn = QSqlDatabase::addDatabase("QSQLITE");
    conn.setDatabaseName(_db_loc);
    _records_model = new QSqlTableModel(this, conn);
    _records_model->setTable("Records");
    _log = new QSqlTableModel(this, conn);

    _records_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _records_model->select();
    _records_model->setHeaderData(0, Qt::Horizontal, tr("Start Time"));
    _records_model->setHeaderData(1, Qt::Horizontal, tr("End Time"));
    _records_model->setHeaderData(2, Qt::Horizontal, tr("Job"));
}

void MainWindow::_init_gui() {
    // new stuff
    _icon = new QIcon(":/icon.png");
    this->setWindowIcon(*_icon);

    _tray_menu = new QMenu();
    _tray_menu->addAction(_ui.action_start);
    _tray_menu->addAction(_ui.action_pause);
    _tray_menu->addAction(_ui.action_stop);
    _tray_menu->addAction(_ui.action_quit);

    _tray_icon = new QSystemTrayIcon(*_icon, this);
    _tray_icon->setContextMenu(_tray_menu);
    _tray_icon->show();

    _ui._records_view->setModel(_records_model);
    _ui._records_view->hideColumn(0);
    _ui._records_view->show();

    // existing stuff
    _ui.db_loc_lineedit->setText(_db_loc);
    _ui.sound_effect_checkbox->setChecked(_sound_effect);

    //    _ui.lcdNumber->display(QString(_poromodo_duration) + QString(":00"));
}

void MainWindow::_create_connections() {
    connect(_tray_icon, &QSystemTrayIcon::activated, this, &MainWindow::activate_or_hide_window);
    connect(_ui.action_quit, &QAction::triggered, this, &MainWindow::quit_win);
    connect(_ui.poromodo_duration_slider, &QSlider::valueChanged,
            [=](int n) { _ui.long_duration_slider->setMaximum(n); });
    connect(_ui.long_duration_slider, &QSlider::valueChanged, [this](int n) {
        _ui.short_break_duration_slider->setMaximum(std::min(n, this->_ui.poromodo_duration_slider->value()));
    });

    connect(_ui.poromodo_duration_slider, &QSlider::valueChanged, _poromodo, &Poromodo::setPoromodoDurationMin);
    connect(_ui.short_break_duration_slider, &QSlider::valueChanged, _poromodo, &Poromodo::setShortBreakDurationMin);
    connect(_ui.long_duration_slider, &QSlider::valueChanged, _poromodo, &Poromodo::setLongBreakDurationMin);
    connect(_poromodo, &Poromodo::TimeLeftStr, [this](QString s) { _ui.lcdNumber->display(s);});

    connect(_ui.action_start, &QAction::triggered, _poromodo, &Poromodo::StartPoromodo);
    connect(_ui.start_buttom, &QPushButton::pressed, _poromodo, &Poromodo::StartPoromodo);
    connect(_ui.action_pause, &QAction::triggered, this, &MainWindow::pause_unpause_resp);
    connect(_ui.pause_buttom, &QPushButton::pressed, this, &MainWindow::pause_unpause_resp);
    connect(_ui.action_stop, &QAction::triggered, _poromodo, &Poromodo::Stop);
    connect(_ui.stop_buttom, &QPushButton::pressed, _poromodo, &Poromodo::Stop);

}

void MainWindow::_read_settings() {
    _settings = new QSettings();
    int val = _settings->value("poromodo/poromodo_duration", 45).toInt();
    _ui.poromodo_duration_slider->setValue(val);
    _poromodo->setPoromodoDurationMin(val);

    val = _settings->value("poromodo/short_break_duration", 5).toInt();
    _ui.short_break_duration_slider->setValue(val);
    _poromodo->setShortBreakDurationMin(val);

    val = _settings->value("poromodo/long_break_duration", 15).toInt();
    _ui.long_duration_slider->setValue(val);
    _poromodo->setLongBreakDurationMin(val);

    _sound_effect = (_settings->value("other/sound_effect", Qt::Checked).toInt() == Qt::Checked);
    _db_loc = _settings->value("other/db_loc", "./db.sqlite").toString();
}

void MainWindow::_write_settings() {
    _settings->setValue("poromodo/poromodo_duration", _ui.poromodo_duration_slider->value());
    _settings->setValue("poromodo/short_break_duration", _ui.short_break_duration_slider->value());
    _settings->setValue("poromodo/long_break_duration", _ui.long_duration_slider->value());
    _settings->setValue("other/db_loc", _db_loc);

    int se;
    _sound_effect ? se = Qt::Checked : se = Qt::Unchecked;
    _settings->setValue("other/sound_effect", se);

    _settings->sync();
}


////////////////////////////////////////////////////////////////////////////////
/// private slots
////////////////////////////////////////////////////////////////////////////////
void MainWindow::poromodo_duration_change(int n) {
    QString text = QString::fromStdString("Poromodo Duration: " + std::to_string(n) + "min");
    _ui.pomorodo_duration_label->setText(text);
    _settings->setValue("poromodo/poromodo_duration", n);
}

void MainWindow::short_break_duration_change(int n) {
    QString text = QString::fromStdString("Short Break Duration: " + std::to_string(n) + "min");
    _ui.short_break_duration_label->setText(text);
    _settings->setValue("poromodo/short_break_duration", n);
}

void MainWindow::long_break_duration_change(int n) {
    QString text = QString::fromStdString("Long Break Duration: " + std::to_string(n) + "min");
    _ui.long_break_duration_label->setText(text);
    _settings->setValue("poromodo/long_break_duration", n);
}

void MainWindow::db_loc_change(QString str) {
    _db_loc = str;
    _settings->setValue("other/db_loc", str);
}

void MainWindow::sound_effect_change(int s) {
    _sound_effect = (s == Qt::Checked);
    _settings->setValue("other/sound_effect", s);
}

void MainWindow::pause_unpause_resp() {
    if (_ui.pause_buttom->text() == QString("Pause")) {
        _ui.action_pause->setText("Unpause");
        _ui.pause_buttom->setText("Unpause");
        _poromodo->Pause();
    } else {
        _ui.action_pause->setText("Pause");
        _ui.pause_buttom->setText("Pause");
        _poromodo->Unpause();
    }
}

void MainWindow::start_job() {}

void MainWindow::finish_job() {}

void MainWindow::quit_win() {
    _gonna_close = true;
    close();
}

void MainWindow::activate_or_hide_window(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
        if (isVisible()) {
            hide();
        } else {
            show();
            activateWindow();
        }
    }
}
