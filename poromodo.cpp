#include "poromodo.h"

Poromodo::Poromodo(QObject* parent) : QObject(parent) {
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &Poromodo::onTimeOut);
    QSqlDatabase conn = QSqlDatabase::addDatabase("QSQLITE");
    conn.setDatabaseName(kDBLoc);
    if (!conn.open()) {
        throw runtime_error("Cannot connect to database.");
    } else {
        qDebug() << "Database conncected.";
        QStringList tables = conn.tables();
        if (!tables.contains(kTableName)) {
            qDebug() << "Database does not contain table " << kTableName << ", Creating Manually.";
            QSqlQuery query;
            query.exec(
                "CREATE TABLE Records (`id` INTEGER, `start_time` DATETIME, `end_time` DATETIME, `duration` INTEGER, "
                "`category` TEXT, `job` TEXT, `hashtags` TEXT)");
        }
    }
}

Poromodo::Poromodo(int poromodo_dur_min, int short_break_dur_min, int long_break_dur_min, QObject* parent)
    : Poromodo(parent) {
    poromodo_dur_ = minutes(poromodo_dur_min);
    short_break_dur_ = minutes(short_break_dur_min);
    long_break_dur_ = minutes(long_break_dur_min);
}

void Poromodo::StartPoromodo(QString category, QString job, QString hashtags) {
    category_ = category;
    job_ = job;
    hashtags_ = hashtags;

    StartPoromodo();
}

void Poromodo::StartPoromodo() {
    if (status_ != Status::NONE) {
        Stop();
    }
    set_status(Status::POROMODO);
    qDebug() << "Start Poromodo";

    start_time_ = QDateTime::currentDateTime();
    time_left_sec_ = duration_cast<seconds>(poromodo_dur_).count();
    timer_->start(milliseconds(1000));
}

void Poromodo::StartShortBreak() {
    if (status_ != Status::NONE) {
        Stop();
    }
    qDebug() << "Start Short Break";
    set_status(Status::SHORT_BREAK);
    time_left_sec_ = duration_cast<seconds>(short_break_dur_).count();
    timer_->start(milliseconds(1000));
}

void Poromodo::StartLongBreak() {
    if (status_ != Status::NONE) {
        Stop();
    }
    qDebug() << "Start Long Break";
    set_status(Status::LONG_BREAK);
    time_left_sec_ = duration_cast<seconds>(long_break_dur_).count();
    timer_->start(milliseconds(1000));
}

void Poromodo::Pause() {
    if (status_ != Status::NONE) {
        timer_->stop();
        pre_status_ = status_;
        set_status(Status::PAUSE);
        pause_start_time_ = QDateTime::currentDateTime();
    }
}

void Poromodo::Unpause() {
    if (status_ == Status::PAUSE) {
        set_status(pre_status_);
        timer_->start();
        auto pause_end_time = QDateTime::currentDateTime();
        pause_duration_milliseconds_ += pause_start_time_.msecsTo(pause_end_time);
    }
}

void Poromodo::Stop() {
    timer_->stop();
    set_status(Status::NONE);
    auto end_time = QDateTime::currentDateTime();
    long whole_duration_milliseconds = start_time_.msecsTo(end_time);
    long work_duration = (whole_duration_milliseconds - pause_duration_milliseconds_) / 1000;

    // todo: append record to database
    auto db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO :database (start_time, end_time, duration, category, job, hashtags) "
        "VALUES(:start_time, :end_time, :duration, :category, :job, :hashtags);");
    query.bindValue(":database", kTableName);
    query.bindValue(":start_time", start_time_.toString(Qt::DateFormat::ISODate));
    query.bindValue(":end_time", end_time.toString(Qt::DateFormat::ISODate));
    query.bindValue(":duration", int(work_duration));
    query.bindValue(":category", category_);
    query.bindValue(":job", job_);
    query.bindValue(":hashtags", hashtags_);
    bool query_status = query.exec();
    if (query_status) {
        qDebug() << "Entry inserted successfully!";
    } else {
        qDebug() << "Data insersion failed!";
    }
    db.commit();

    // clean up
    pause_duration_milliseconds_ = 0;
}

void Poromodo::set_status(Poromodo::Status s) {
    if (s != status_) {
        status_ = s;
        emit StatusChanged(s);
    }
}

void Poromodo::onTimeOut() {
    time_left_sec_--;
    emit TimeLeft(seconds(time_left_sec_));
    QString time_left_str = QString::number(time_left_sec_ / 60).rightJustified(2, '0') + ":" +
                            QString::number(time_left_sec_ % 60).rightJustified(2, '0');
    emit TimeLeftStr(time_left_str);
    qDebug() << "Time left in string: " << time_left_str;
    if (time_left_sec_ == 0) {
        if (status_ == Status::POROMODO) {
            poromodo_count_++;
            if (poromodo_count_ == kMaxPoomodoCount) {
                poromodo_count_ = 0;
                StartLongBreak();
            } else {
                StartShortBreak();
            }
        } else {
            StartPoromodo();
        }
    }
}
