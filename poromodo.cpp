#include "poromodo.h"

Poromodo::Poromodo(QObject* parent) : QObject(parent) {
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &Poromodo::onTimeOut);
}

Poromodo::Poromodo(int poromodo_dur_min, int short_break_dur_min, int long_break_dur_min, QObject* parent)
    : Poromodo(parent) {
    poromodo_dur_ = minutes(poromodo_dur_min);
    short_break_dur_ = minutes(short_break_dur_min);
    long_break_dur_ = minutes(long_break_dur_min);
}

void Poromodo::StartPoromodo() {
    if (status_ != Status::NONE) {
        Stop();
    }
    set_status(Status::POROMODO);
    qDebug() << "Start Poromodo";
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
    }
}

void Poromodo::Unpause() {
    if (status_ == Status::PAUSE) {
        set_status(pre_status_);
        timer_->start();
    }
}

void Poromodo::Stop() {
    timer_->stop();
    set_status(Status::NONE);
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
