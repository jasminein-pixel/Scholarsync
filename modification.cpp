#include "modification.h"
void formatTime(QString unformattedTime, QString &date , QString &time)
{
    int index;
    index = unformattedTime.indexOf("T");
    date = unformattedTime.left(index);
    time = unformattedTime.mid(index+1, 8);
}

QString spaceRemover(QString string)
{
    for(int i=0;i<string.length();i++)
    {
        if(string[i] == ' ')
        {
            string[i] = '-';
        }
    }
    return string;

}
