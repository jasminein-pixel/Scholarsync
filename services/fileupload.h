#pragma once
// ─────────────────────────────────────────
//  services/fileupload.h
//  Azure Blob Storage CV uploader.
// ─────────────────────────────────────────
#include <QString>
#include <QApplication>
#include <QFileDialog>
#include <QSqlDatabase>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <QDesktopServices>
#include "modification.h"

class File
{
private:
    QString ACCOUNT   = "studentcv";
    QString CONTAINER = "student-cvs";
    QString SAS_TOKEN = "sp=rcw&st=2026-06-27T05:17:52Z&se=2027-06-27T13:32:52Z&spr=https&sv=2026-02-06&sr=c&sig=HZJVdr5q2Mk8heUUbweFxDgpZ9Nfih4lsREHIyF23vU%3D";

public:
    bool uploadCV(QString sid, QString name, QSqlDatabase &db);
};
