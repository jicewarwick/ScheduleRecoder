#ifndef POROMODO_H
#define POROMODO_H

#include <QDateTime>
#include <QObject>
#include <QStringList>
#include <QTimer>
#include <set>

using namespace std::chrono;
using std::set;

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
    QStringList getAllCategories() const { return SetToQStringList(QueryDatabaseColumn("category")); }
    QStringList getAllActivities() const { return SetToQStringList(QueryDatabaseColumn("activity")); }
    QStringList getAllHashtags() const;

signals:
    void TimeLeftStr(QString);
    void TimeLeft(seconds);
    void StatusChangedAuto(Status);
    void StatusChangedManual(Status);

public slots:
    void StartPoromodo(QString category, QString activity, QString hashtags);
    void Pause();
    void Unpause();
    void Stop();

private slots:
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

    void StartPoromodo();
    void StartShortBreak();
    void StartLongBreak();

    void set_status_auto(Status s);
    void set_status_mannual(Status s);
    set<QString> QueryDatabaseColumn(QString column) const;
    static QStringList SetToQStringList(set<QString> input);
    void InsertRecords();
};

#endif	// POROMODO_H
