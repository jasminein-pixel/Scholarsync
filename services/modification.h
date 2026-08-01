#pragma once
// ─────────────────────────────────────────
//  services/modification.h
//  Free utility functions: time formatting, string helpers.
// ─────────────────────────────────────────
#include <QString>
#include <QDebug>
#include <QDateTime>
#include <QTimeZone>
#include <iostream>

void formatTime(QString unformattedTime, QString &date, QString &time);
QString spaceRemover(QString string);
