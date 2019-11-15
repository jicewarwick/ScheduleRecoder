#ifndef POROMODO_H
#define POROMODO_H

#include <QDebug>
#include <QObject>
#include <QTimer>
#include <chrono>

using namespace std::chrono;

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
    void StartPoromodo();
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

    void set_status(Status s);

private slots:
    void onTimeOut();
};

#endif	// POROMODO_H
