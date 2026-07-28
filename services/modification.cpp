#include "modification.h"

void formatTime(QString unformattedTime, QString &date, QString &time)
{
    QDateTime utc = QDateTime::fromString(unformattedTime, Qt::ISODate);
    if (!utc.isValid())
    {
        qDebug() << "Failed" << unformattedTime;
        date = "";
        time = "";
        return;
    }
    utc.setTimeZone(QTimeZone::UTC);

    QDateTime nepalTime = utc.toTimeZone(QTimeZone("Asia/Kathmandu"));

    date = nepalTime.toString("yyyy-MM-dd");
    time = nepalTime.toString("HH:mm:ss");
}

QString spaceRemover(QString string)
{
    for (int i = 0; i < string.length(); i++)
    {
        if (string[i] == ' ')
        {
            string[i] = '-';
        }
    }
    return string;
}
