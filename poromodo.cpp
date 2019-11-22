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
            query.exec("CREATE TABLE " + kTableName +
                       "(`id` INTEGER, `start_time` DATETIME, `end_time` DATETIME, `duration` INTEGER, "
                       "`category` TEXT, `activity` TEXT, `hashtags` TEXT);");
        }
    }
}

Poromodo::Poromodo(int poromodo_dur_min, int short_break_dur_min, int long_break_dur_min, QObject* parent)
    : Poromodo(parent) {
    poromodo_dur_ = minutes(poromodo_dur_min);
    short_break_dur_ = minutes(short_break_dur_min);
    long_break_dur_ = minutes(long_break_dur_min);
}

QStringList Poromodo::getAllCategories() { return SetToQStringList(QueryDatabaseColumn("category")); }

QStringList Poromodo::getAllActivities() { return SetToQStringList(QueryDatabaseColumn("activity")); }

QStringList Poromodo::getAllHashtags() {
    set<QString> results;
    set<QString> hastag_lins = QueryDatabaseColumn("hashtags");
    for (auto it : hastag_lins) {
        for (auto hashtag : it.split(" ")) {
            results.insert(hashtag);
        }
    }
    return SetToQStringList(results);
}

////////////////////////////////////////////////////////////////////////////////
/// public slots
////////////////////////////////////////////////////////////////////////////////

void Poromodo::StartPoromodo(QString category, QString activity, QString hashtags) {
    category_ = category;
    activity_ = activity;
    hashtags_ = hashtags;

    start_time_ = QDateTime::currentDateTime();
    set_status_mannual(Status::POROMODO);
    qDebug() << "Start Poromodo";
    StartPoromodo();
}

void Poromodo::Pause() {
    if (status_ != Status::NONE) {
        timer_->stop();
        pre_status_ = status_;
        set_status_mannual(Status::PAUSE);
        pause_start_time_ = QDateTime::currentDateTime();
    }
}

void Poromodo::Unpause() {
    if (status_ == Status::PAUSE) {
        set_status_mannual(pre_status_);
        timer_->start();
        auto pause_end_time = QDateTime::currentDateTime();
        pause_duration_milliseconds_ += pause_start_time_.msecsTo(pause_end_time);
    }
}

void Poromodo::Stop() {
    timer_->stop();
    if (status_ != Status::NONE) {
        set_status_mannual(Status::NONE);
        InsertRecords();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// private functions
////////////////////////////////////////////////////////////////////////////////
void Poromodo::StartPoromodo() {
    time_left_sec_ = duration_cast<seconds>(poromodo_dur_).count();
    timer_->start(milliseconds(1000));
}

void Poromodo::StartShortBreak() {
    qDebug() << "Start Short Break";
    set_status_auto(Status::SHORT_BREAK);
    time_left_sec_ = duration_cast<seconds>(short_break_dur_).count();
    timer_->start(milliseconds(1000));
}

void Poromodo::StartLongBreak() {
    qDebug() << "Start Long Break";
    set_status_auto(Status::LONG_BREAK);
    time_left_sec_ = duration_cast<seconds>(long_break_dur_).count();
    timer_->start(milliseconds(1000));
}

void Poromodo::set_status_auto(Poromodo::Status s) {
    if (s != status_) {
        status_ = s;
        emit StatusChangedAuto(s);
    }
}

void Poromodo::set_status_mannual(Poromodo::Status s) {
    if (s != status_) {
        status_ = s;
        emit StatusChangedManual(s);
    }
}

set<QString> Poromodo::QueryDatabaseColumn(QString column) {
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.setForwardOnly(true);
    QString query_base = "SELECT " + column + " FROM " + kTableName;
    query.prepare(query_base);
    query.exec();

    set<QString> results;
    while (query.next()) {
        results.insert(query.value(0).toString());
    }
    return results;
}

QStringList Poromodo::SetToQStringList(set<QString> input) {
    QStringList output;
    for (auto it : input) {
        output << it;
    }
    return output;
}

void Poromodo::onTimeOut() {
    time_left_sec_--;
    emit TimeLeft(seconds(time_left_sec_));
    QString time_left_str = QString::number(time_left_sec_ / 60).rightJustified(2, '0') + ":" +
                            QString::number(time_left_sec_ % 60).rightJustified(2, '0');
    emit TimeLeftStr(time_left_str);
    //    qDebug() << "Time left in string: " << time_left_str;
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
            set_status_auto(Status::POROMODO);
            qDebug() << "Start Poromodo";
            StartPoromodo();
        }
    }
}

void Poromodo::InsertRecords() {
    auto end_time = QDateTime::currentDateTime();
    long whole_duration_milliseconds = start_time_.msecsTo(end_time);
    long work_duration_sec = (whole_duration_milliseconds - pause_duration_milliseconds_) / 1000;

    // append record to database
    auto db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.prepare("INSERT INTO " + kTableName +
                  "(start_time, end_time, duration, category, activity, hashtags) "
                  "VALUES(:start_time, :end_time, :duration, :category, :activity, :hashtags);");
    query.bindValue(":start_time", start_time_.toString(Qt::DateFormat::ISODate));
    query.bindValue(":end_time", end_time.toString(Qt::DateFormat::ISODate));
    query.bindValue(":duration", int(work_duration_sec / 60));
    query.bindValue(":category", category_);
    query.bindValue(":activity", activity_);
    query.bindValue(":hashtags", hashtags_);
    bool query_status = query.exec();
    if (query_status) {
        qDebug() << "Entry inserted successfully!";
    } else {
        qDebug() << "Data insersion failed!";
    }

    // clean up
    pause_duration_milliseconds_ = 0;
}
