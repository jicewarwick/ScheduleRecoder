#include "mainwindow.h"

////////////////////////////////////////////////////////////////////////////////
/// public functions
////////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    QCoreApplication::setOrganizationName("CJSoft");
    QCoreApplication::setOrganizationDomain("CJSoft.com");
    QCoreApplication::setApplicationName("ScheduleRecorder");
    ui_.setupUi(this);

    poromodo_ = new Poromodo(this);
    InitDB();
    InitGUI();
    CreateConnections();
    ReadSettings();
}

////////////////////////////////////////////////////////////////////////////////
/// protected functions
////////////////////////////////////////////////////////////////////////////////
void MainWindow::closeEvent(QCloseEvent* event) {
    if (gonna_close_) {
        qDebug() << "Really closing";
        poromodo_->Stop();
        WriteSettings();
        event->accept();
    } else {
        qDebug() << "Not really closing";
        this->hide();
        tray_icon_->showMessage("Hiding", "Programe is hiding");
        event->ignore();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// private functions
////////////////////////////////////////////////////////////////////////////////
void MainWindow::InitDB() {
    QSqlDatabase conn = QSqlDatabase::database();
    conn.setDatabaseName(kDBLoc);
    if (!conn.open()) {
        QMessageBox::critical(this, tr("Cannot open database"),
                              tr("Unable to establish a database connection.\n"
                                 "Click Cancel to exit."),
                              QMessageBox::Cancel);
        abort();
    }

    records_model_ = new QSqlTableModel(this, conn);
    records_model_->setTable(kTableName);
    //    log_ = new QSqlTableModel(this, conn);

    records_model_->setEditStrategy(QSqlTableModel::OnManualSubmit);
    records_model_->select();
    vector<string> header{"ID", "Start Time", "End Time", "Duration", "Category", "Job", "Hashtags"};
    for (size_t i = 0; i < header.size(); ++i) {
        records_model_->setHeaderData(int(i), Qt::Horizontal, tr(header[i].c_str()));
    }
}

void MainWindow::InitGUI() {
    // new stuff
    setWindowTitle(tr("Schedule"));
    icon_ = new QIcon(":/icon.png");
    this->setWindowIcon(*icon_);

    tray_menu_ = new QMenu();
    tray_menu_->addAction(ui_.action_start);
    tray_menu_->addAction(ui_.action_pause);
    tray_menu_->addAction(ui_.action_stop);
    tray_menu_->addAction(ui_.action_quit);

    tray_icon_ = new QSystemTrayIcon(*icon_, this);
    tray_icon_->setContextMenu(tray_menu_);
    tray_icon_->show();

    ui_.records_view->setModel(records_model_);
    ui_.records_view->hideColumn(0);
    ui_.records_view->show();
//    ui_.records_view->resizeColumnsToContents();

    // existing stuff
    ui_.lcdNumber->display("00:00");
    ui_.sound_effect_checkbox->setChecked(sound_effect_);
}

void MainWindow::CreateConnections() {
    connect(tray_icon_, &QSystemTrayIcon::activated, this, &MainWindow::onClickTray);
    connect(ui_.action_quit, &QAction::triggered, this, &MainWindow::onQuitWindow);
    connect(ui_.poromodo_duration_slider, &QSlider::valueChanged,
            [this](int n) { ui_.long_duration_slider->setMaximum(n); });
    connect(ui_.long_duration_slider, &QSlider::valueChanged, [this](int n) {
        ui_.short_break_duration_slider->setMaximum(std::min(n, this->ui_.poromodo_duration_slider->value()));
    });

    connect(ui_.poromodo_duration_slider, &QSlider::valueChanged, this, &MainWindow::onPoromodoDurationChange);
    connect(ui_.short_break_duration_slider, &QSlider::valueChanged, this, &MainWindow::onShortBreakDurationChange);
    connect(ui_.long_duration_slider, &QSlider::valueChanged, this, &MainWindow::onLongBreakdurationChange);
    connect(poromodo_, &Poromodo::TimeLeftStr, [this](QString s) { ui_.lcdNumber->display(s); });

//    connect(ui_.action_start, &QAction::triggered, poromodo_, &Poromodo::StartPoromodo);
    connect(ui_.start_buttom, &QPushButton::pressed, [this]() {poromodo_->StartPoromodo(ui_.category_combo->currentText(), ui_.activity_combo->currentText(), ui_.hashtags_lineedit->text());});
    connect(ui_.action_pause, &QAction::triggered, this, &MainWindow::onPuaseUnpause);
    connect(ui_.pause_buttom, &QPushButton::pressed, this, &MainWindow::onPuaseUnpause);
    connect(ui_.action_stop, &QAction::triggered, poromodo_, &Poromodo::Stop);
    connect(ui_.stop_buttom, &QPushButton::pressed, poromodo_, &Poromodo::Stop);

    connect(poromodo_, &Poromodo::StatusChanged, this, &MainWindow::onStatusChange);
}

void MainWindow::ReadSettings() {
    settings_ = new QSettings();
    int val = settings_->value("poromodo/poromodo_duration", 45).toInt();
    ui_.poromodo_duration_slider->setValue(val);
    poromodo_->setPoromodoDurationMin(val);

    val = settings_->value("poromodo/short_break_duration", 5).toInt();
    ui_.short_break_duration_slider->setValue(val);
    poromodo_->setShortBreakDurationMin(val);

    val = settings_->value("poromodo/long_break_duration", 15).toInt();
    ui_.long_duration_slider->setValue(val);
    poromodo_->setLongBreakDurationMin(val);

    sound_effect_ = (settings_->value("other/sound_effect", Qt::Checked).toInt() == Qt::Checked);
}

void MainWindow::WriteSettings() {
    settings_->setValue("poromodo/poromodo_duration", ui_.poromodo_duration_slider->value());
    settings_->setValue("poromodo/short_break_duration", ui_.short_break_duration_slider->value());
    settings_->setValue("poromodo/long_break_duration", ui_.long_duration_slider->value());

    int se;
    sound_effect_ ? se = Qt::Checked : se = Qt::Unchecked;
    settings_->setValue("other/sound_effect", se);

    settings_->sync();
}

////////////////////////////////////////////////////////////////////////////////
/// private slots
////////////////////////////////////////////////////////////////////////////////
void MainWindow::onPoromodoDurationChange(int n) {
    QString text = QString::fromStdString("Poromodo Duration: " + std::to_string(n) + "min");
    ui_.pomorodo_duration_label->setText(text);
    settings_->setValue("poromodo/poromodo_duration", n);
    poromodo_->setPoromodoDurationMin(n);
}

void MainWindow::onShortBreakDurationChange(int n) {
    QString text = QString::fromStdString("Short Break Duration: " + std::to_string(n) + "min");
    ui_.short_break_duration_label->setText(text);
    settings_->setValue("poromodo/short_break_duration", n);
    poromodo_->setShortBreakDurationMin(n);
}

void MainWindow::onLongBreakdurationChange(int n) {
    QString text = QString::fromStdString("Long Break Duration: " + std::to_string(n) + "min");
    ui_.long_break_duration_label->setText(text);
    settings_->setValue("poromodo/long_break_duration", n);
    poromodo_->setLongBreakDurationMin(n);
}

void MainWindow::onSoundEffectStatusChange(int s) {
    sound_effect_ = (s == Qt::Checked);
    settings_->setValue("other/sound_effect", s);
}

void MainWindow::onPuaseUnpause() {
    if (ui_.pause_buttom->text() == QString("Pause")) {
        ui_.action_pause->setText("Unpause");
        ui_.pause_buttom->setText("Unpause");
        poromodo_->Pause();
    } else {
        ui_.action_pause->setText("Pause");
        ui_.pause_buttom->setText("Pause");
        poromodo_->Unpause();
    }
}

void MainWindow::onQuitWindow() {
    gonna_close_ = true;
    close();
}

void MainWindow::onClickTray(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
        if (isVisible()) {
            hide();
        } else {
            show();
            activateWindow();
        }
    }
}

void MainWindow::PlaySound() {
    if (sound_effect_) {
        QSound::play(":/sounds/loud-bell.wav");
    }
}

void MainWindow::onStatusChange(Poromodo::Status s) {
    switch (s) {
        case Poromodo::Status::NONE:
            ui_.status_label->setText(tr("READY"));
            break;
        case Poromodo::Status::POROMODO:
            ui_.status_label->setText(tr("POROMODO"));
            break;
        case Poromodo::Status::SHORT_BREAK:
            ui_.status_label->setText(tr("SHORT BREAK"));
            break;
        case Poromodo::Status::LONG_BREAK:
            ui_.status_label->setText(tr("LONG BREAK"));
            break;
        case Poromodo::Status::PAUSE:
            ui_.status_label->setText(ui_.status_label->text() + tr("(PAUSED)"));
    }
    PlaySound();
}
