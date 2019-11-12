#include "poromodo.h"

Poromodo::Poromodo(QObject* parent) : QObject(parent) {
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &Poromodo::onTimeOut);
}

Poromodo::Poromodo(int poromodo_dur_min, int short_break_dur_min, int long_break_dur_min, QObject* parent)
    : Poromodo(parent) {
    _poromodo_dur = minutes(poromodo_dur_min);
    _short_break_dur = minutes(short_break_dur_min);
    _long_break_dur = minutes(long_break_dur_min);
}

void Poromodo::setPoromodoDurationMin(int poromodo_dur_min) { _poromodo_dur = minutes(poromodo_dur_min); }

void Poromodo::setShortBreakDurationMin(int short_break_dur_min) { _short_break_dur = minutes(short_break_dur_min); }

void Poromodo::setLongBreakDurationMin(int long_break_dur_min) { _long_break_dur = minutes(long_break_dur_min); }

void Poromodo::StartPoromodo() {
    if (_status != Status::NONE) {
        Stop();
    }
    setStatus(Status::POROMODO);
    _time_left_sec = duration_cast<seconds>(_poromodo_dur).count();
    _timer->start(milliseconds(1000));
}

void Poromodo::StartShortBreak() {
    if (_status != Status::NONE) {
        Stop();
    }
    setStatus(Status::POROMODO);
    _time_left_sec = duration_cast<seconds>(_short_break_dur).count();
    _timer->start(milliseconds(1000));
}

void Poromodo::StartLongBreak() {
    if (_status != Status::NONE) {
        Stop();
    }
    setStatus(Status::POROMODO);
    _time_left_sec = duration_cast<seconds>(_long_break_dur).count();
    _timer->start(milliseconds(1000));
}

void Poromodo::Pause() {
    _timer->stop();
    _pre_status = _status;
    setStatus(Status::PAUSE);
}

void Poromodo::Unpause() {
    if (_status == Status::PAUSE) {
        setStatus(_pre_status);
        _timer->start();
    }
}

void Poromodo::Stop() {
    _timer->stop();
    setStatus(Status::NONE);
}

void Poromodo::setStatus(Poromodo::Status s) {
    if (s != _status) {
        _status = s;
        emit StatusChanged(s);
    }
}

void Poromodo::onTimeOut() {
    _time_left_sec--;
    emit TimeLeft(seconds(_time_left_sec));
    QString time_left_str = QString::number(_time_left_sec / 60).rightJustified(2, '0') + ":" +
                            QString::number(_time_left_sec % 60).rightJustified(2, '0');
    emit TimeLeftStr(time_left_str);
    qDebug() << time_left_str;
    if (_time_left_sec == 0) {
        Stop();
    }
}
