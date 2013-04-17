#include "managerthread.h"
#include <QtNetwork>

namespace livesheap{
ManagerThread::ManagerThread(QObject *parent) :
    QObject(parent)
{
    pManager = new QNetworkAccessManager();
    connect(this,SIGNAL(postSignal(const QNetworkRequest&, const QByteArray&)),this,SLOT(post(const QNetworkRequest&, const QByteArray&)));
    connect(pManager,SIGNAL(finished(QNetworkReply*)),this,SIGNAL(finished(QNetworkReply*)));
}

void ManagerThread::emitPost(const QNetworkRequest& r, const QByteArray& s)
{
    emit postSignal(r,s);
}

void ManagerThread::post(const QNetworkRequest& r, const QByteArray& s)
{
    pManager->post(r,s);
}

void ManagerThread::replyCanQuit(QNetworkReply*)
{
    bCanQuit = 1;
}
}
