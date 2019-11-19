#ifndef POROMODO_H
#define POROMODO_H

#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QTimer>
#include <set>
#include <stdexcept>
#include <string>

using namespace std::chrono;
using std::runtime_error;
using std::set;
using std::string;

const QString kDBLoc = "./db.sqlite";
const QString kTableName = "Records";

class Poromodo : public QObject {
    Q_OBJECT

public:
    Poromodo(QObject* parent = nullptr);
    Poromodo(int poromodo_dur_min, int short_break_dur_min, int long_break_dur_min, QObject* parent = nullptr);

    enum class Status { NONE, POROMODO, SHORT_BREAK, LONG_BREAK, PAUSE };

    // setter
    void setPoromodoDurationMin(int dur) { poromodo_dur_ = minutes(dur); }
    void setShortBreakDurationMin(int dur) { short_break_dur_ = minutes(dur); }
    void setLongBreakDurationMin(int dur) { long_break_dur_ = minutes(dur); }

    // db queryer
    QStringList getAllCategories();
    QStringList getAllActivities();
    QStringList getAllHashtags();

signals:
    void TimeLeftStr(QString);
    void TimeLeft(seconds);
    void StatusChanged(Status);

public slots:
    void StartPoromodo(QString category, QString activity, QString hashtags);
    void StartShortBreak();
    void StartLongBreak();
    void Pause();
    void Unpause();
    void Stop();

private slots:
    void StartPoromodo();
    void onTimeOut();

private:
    minutes poromodo_dur_;
    minutes short_break_dur_;
    minutes long_break_dur_;

    Status status_ = Status::NONE;
    Status pre_status_ = Status::NONE;
    QTimer* timer_;
    long long time_left_sec_;
    const short kMaxPoomodoCount = 3;
    short poromodo_count_ = 0;

    long pause_duration_milliseconds_ = 0;

    QDateTime start_time_;
    QDateTime pause_start_time_;

    QString category_;
    QString activity_;
    QString hashtags_;

    void set_status(Status s);
    set<QString> QueryDatabaseColumn(QString column);
    QStringList SetToQStringList(set<QString> input);
};

#endif	// POROMODO_H
