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

    void setPoromodoDurationMin(int);
    void setShortBreakDurationMin(int);
    void setLongBreakDurationMin(int);

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
    minutes _poromodo_dur;
    minutes _short_break_dur;
    minutes _long_break_dur;

    Status _status = Status::NONE;
    Status _pre_status = Status::NONE;
    QTimer* _timer;
    long long _time_left_sec;

    void setStatus(Status s);

private slots:
    void onTimeOut();
};

#endif	// POROMODO_H
