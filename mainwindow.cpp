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
        qDebug() << "Hide to tray";
        this->hide();
        tray_icon_->showMessage(tr("Minimized to Tray"), tr("Working in background!"));
        event->ignore();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// private slots
////////////////////////////////////////////////////////////////////////////////
void MainWindow::onPoromodoDurationChange(int n) {
    QString text = tr("Poromodo Duration: ") + QString::number(n) + tr("min");
    ui_.pomorodo_duration_label->setText(text);
    poromodo_->setPoromodoDurationMin(n);
}

void MainWindow::onShortBreakDurationChange(int n) {
    QString text = tr("Short Break Duration: ") + QString::number(n) + tr("min");
    ui_.short_break_duration_label->setText(text);
    poromodo_->setShortBreakDurationMin(n);
}

void MainWindow::onLongBreakdurationChange(int n) {
    QString text = tr("Long Break Duration: ") + QString::number(n) + tr("min");
    ui_.long_break_duration_label->setText(text);
    poromodo_->setLongBreakDurationMin(n);
}

void MainWindow::onPuaseUnpause() {
    if (ui_.pause_buttom->text() == tr("Pause")) {
        ui_.action_pause->setText(tr("Unpause"));
        ui_.action_pause->setIcon(*start_icon_);
        ui_.pause_buttom->setText(tr("Unpause"));
        ui_.pause_buttom->setIcon(*start_icon_);
        poromodo_->Pause();
    } else {
        ui_.action_pause->setText(tr("Pause"));
        ui_.action_pause->setIcon(*pause_icon_);
        ui_.pause_buttom->setText(tr("Pause"));
        ui_.pause_buttom->setIcon(*pause_icon_);
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

void MainWindow::onStatusChange(Poromodo::Status s) {
    QString status_info = StatusInfo.at(s);
    switch (s) {
        case Poromodo::Status::PAUSE:
            ui_.status_label->setText(ui_.status_label->text() + status_info);
            break;
        case Poromodo::Status::NONE:
            ui_.status_label->setText(status_info);
            break;
        default:
            ui_.status_label->setText(status_info);
    }
}

void MainWindow::onStartPorodomoPorcess() {
    QString new_activity = ui_.activity_combo->currentText();
    QString new_category = ui_.category_combo->currentText();
    QString new_hashtag = ui_.hashtags_lineedit->text();
    if (new_activity.isEmpty() | new_category.isEmpty()) {
        QMessageBox::warning(this, tr("MORE INFO NEEDED!"), tr("Please filling infomation about current task."),
                             QMessageBox::Cancel);
        return;
    }
    poromodo_->StartPoromodo(new_category, new_activity, new_hashtag);

    if (!activities_.contains(new_activity)) {
        activities_ << new_activity;
        ui_.activity_combo->insertItem(0, new_activity);
    }
    if (!categories_.contains(new_category)) {
        categories_ << new_category;
        ui_.category_combo->insertItem(0, new_category);
    }
    for (QString it : new_hashtag.split(" ")) {
        if (!hashtags_.contains(it)) {
            hashtags_ << it;
        }
    }

    ui_.start_buttom->setEnabled(false);
    ui_.pause_buttom->setEnabled(true);
    ui_.stop_buttom->setEnabled(true);
}

void MainWindow::onStopPorodomoPorcess() {
    ui_.start_buttom->setEnabled(true);
    ui_.pause_buttom->setEnabled(false);
    ui_.stop_buttom->setEnabled(false);

    poromodo_->Stop();
    records_model_->select();
    log_model_->select();
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
    records_model_->setEditStrategy(QSqlTableModel::OnManualSubmit);
    records_model_->select();

    log_model_ = new QSqlTableModel(this, conn);
    log_model_->setTable(kTableName);
    log_model_->setEditStrategy(QSqlTableModel::OnManualSubmit);
    QString begin_of_day = QDateTime::currentDateTime().date().toString(Qt::ISODate);
    log_model_->setFilter("start_time > \"" + begin_of_day + "\"");
    log_model_->sort(1, Qt::SortOrder::DescendingOrder);
    log_model_->select();

    QStringList header{tr("ID"),	   tr("Start Time"), tr("End Time"), tr("Duration(min)"),
                       tr("Category"), tr("Activity"),	 tr("Hashtags")};
    for (int i = 0; i < header.size(); ++i) {
        records_model_->setHeaderData(int(i), Qt::Horizontal, header[i]);
        log_model_->setHeaderData(int(i), Qt::Horizontal, header[i]);
    }
    activities_ = poromodo_->getAllActivities();
    categories_ = poromodo_->getAllCategories();
    hashtags_ = poromodo_->getAllHashtags();
}

void MainWindow::InitGUI() {
    // new stuff
    setWindowTitle(tr("Schedule"));
    this->setWindowIcon(*title_icon_);

    tray_menu_ = new QMenu(this);
    tray_menu_->addAction(ui_.action_pause);
    tray_menu_->addAction(ui_.action_stop);
    tray_menu_->addSeparator();
    tray_menu_->addAction(ui_.action_quit);

    tray_icon_ = new QSystemTrayIcon(*title_icon_, this);
    tray_icon_->setContextMenu(tray_menu_);
    tray_icon_->show();

    ui_.records_view->setModel(records_model_);
    ui_.records_view->hideColumn(0);
    ui_.records_view->show();
    ui_.records_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui_.log_view->setModel(log_model_);
    ui_.log_view->hideColumn(0);
    ui_.log_view->show();
    ui_.log_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // existing stuff
    ui_.lcdNumber->display("00:00");
    ui_.category_combo->addItem("");
    ui_.category_combo->addItems(categories_);
    ui_.activity_combo->addItem("");
    ui_.activity_combo->addItems(activities_);
    hashtag_completer_ = new QCompleter(hashtags_, this);
    ui_.hashtags_lineedit->setCompleter(hashtag_completer_);
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
    connect(ui_.sound_effect_checkbox, &QCheckBox::stateChanged, [this](int s) { sound_effect_ = (s == Qt::Checked); });
    connect(ui_.message_checkbox, &QCheckBox::stateChanged, [this](int s) { popup_messagebox_ = (s == Qt::Checked); });
    connect(ui_.tray_popup_checkbox, &QCheckBox::stateChanged, [this](int s) { tray_popup_ = (s == Qt::Checked); });
    connect(poromodo_, &Poromodo::TimeLeftStr, [this](QString s) { ui_.lcdNumber->display(s); });

    connect(ui_.start_buttom, &QPushButton::pressed, this, &MainWindow::onStartPorodomoPorcess);
    connect(ui_.action_pause, &QAction::triggered, this, &MainWindow::onPuaseUnpause);
    connect(ui_.pause_buttom, &QPushButton::pressed, this, &MainWindow::onPuaseUnpause);
    connect(ui_.action_stop, &QAction::triggered, poromodo_, &Poromodo::Stop);
    connect(ui_.stop_buttom, &QPushButton::pressed, this, &MainWindow::onStopPorodomoPorcess);

    connect(poromodo_, &Poromodo::StatusChangedAuto, this, &MainWindow::onStatusChange);
    connect(poromodo_, &Poromodo::StatusChangedManual, this, &MainWindow::onStatusChange);
    connect(poromodo_, &Poromodo::StatusChangedAuto, this, &MainWindow::onStatusChangedNotification);
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

    sound_effect_ = settings_->value("other/sound_effect", true).toBool();
    ui_.sound_effect_checkbox->setChecked(sound_effect_);

    popup_messagebox_ = settings_->value("other/popup_messagebox", true).toBool();
    ui_.message_checkbox->setChecked(popup_messagebox_);

    tray_popup_ = settings_->value("other/tray_popup", true).toBool();
    ui_.tray_popup_checkbox->setChecked(tray_popup_);
}

void MainWindow::WriteSettings() {
    settings_->setValue("poromodo/poromodo_duration", ui_.poromodo_duration_slider->value());
    settings_->setValue("poromodo/short_break_duration", ui_.short_break_duration_slider->value());
    settings_->setValue("poromodo/long_break_duration", ui_.long_duration_slider->value());

    settings_->setValue("other/sound_effect", sound_effect_);
    settings_->setValue("other/popup_messagebox", popup_messagebox_);
    settings_->setValue("other/tray_popup", tray_popup_);

    settings_->sync();
}

void MainWindow::onStatusChangedNotification(Poromodo::Status s) {
    QString status_info = StatusInfo.at(s);
    if (tray_popup_) {
        tray_icon_->showMessage(status_info, status_info);
    }
    if (sound_effect_) {
        QSound::play(":/sounds/bell.wav");
    }
    if (popup_messagebox_) {
        QMessageBox::information(this, tr("Status Changed"), status_info);
    }
}
