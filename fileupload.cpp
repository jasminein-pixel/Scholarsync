#include "fileupload.h"

bool File::uploadCV(QString sid, QString Name, QSqlDatabase &db)
{
    QString filePath = QFileDialog::getOpenFileName(
        nullptr, "Select a PDF", "", "PDF Files (*.pdf);; Word Files(.docx)");
    if (filePath.isEmpty())
        return 0;

    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open file";
        return 1;
    }

    QString url = QString("https://%1.blob.core.windows.net/%2/%3-%4.pdf?%5")
                      .arg(ACCOUNT)
                      .arg(CONTAINER)
                      .arg(sid)
                      .arg(spaceRemover(Name))
                      .arg(SAS_TOKEN);

    qDebug() << "Uploading to:" << url;

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QUrl qurl(url);
    QNetworkRequest request(qurl);
    request.setRawHeader("x-ms-blob-type", "BlockBlob");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/pdf");

    QNetworkReply *reply = manager->put(request, file);
    file->setParent(reply);

    QObject::connect(reply, &QNetworkReply::uploadProgress,
                     [=](qint64 sent, qint64 total)
                     {
                         if (total > 0)
                             qDebug() << "Progress:" << (sent * 100 / total) << "%";
                     });

    QObject::connect(reply, &QNetworkReply::finished, [=]()
                     {
        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << "Status:" << status;
        qDebug() << "Response:" << reply->readAll();

        if (status == 201)
            qDebug() << "SUCCESS — check your container in Azure Portal!";
        else
            qDebug() << "FAILED";

        reply->deleteLater();
         });

    url = QString("https://%1.blob.core.windows.net/%2/%3-%4.pdf?%5")
              .arg(ACCOUNT)
              .arg(CONTAINER)
              .arg(sid)
              .arg(spaceRemover(Name))
              .arg(SAS_TOKEN);


    QSqlQuery query(db);
    query.prepare("UPDATE StudentDetails SET cv_url = :url WHERE SID = :sid");
    query.bindValue(":sid", sid);
    query.bindValue(":url", url);

    query.exec();

    return true;
}

