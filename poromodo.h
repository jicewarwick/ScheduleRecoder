#ifndef POROMODO_H
#define POROMODO_H

#include <QDebug>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTimer>
#include <QDateTime>
#include <stdexcept>

using namespace std::chrono;
using std::runtime_error;
using std::tuple;

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

signals:
    void TimeLeftStr(QString);
    void TimeLeft(seconds);
    void StatusChanged(Status);

public slots:
    void StartPoromodo(QString, QString, QString);
    void StartShortBreak();
    void StartLongBreak();
    void Pause();
    void Unpause();
    void Stop();

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

    QDateTime start_time_;
    QDateTime pause_start_time_;

    seconds pause_duration_ = seconds(0);

    QString category_;
    QString job_;
    QString hashtags_;

    void set_status(Status s);

private slots:
    void onTimeOut();
};

#endif	// POROMODO_H
