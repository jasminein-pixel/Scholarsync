#include <QString>
#include<QDebug>
#include <iostream>

void formatTime(QString unformattedTime, QString &date , QString &time)
{
    int index;
    index = unformattedTime.indexOf("T");
    date = unformattedTime.left(index);
    time = unformattedTime.mid(index+1, 8);
}

