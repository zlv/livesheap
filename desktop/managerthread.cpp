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

void ManagerThread::replyChatStatus(QNetworkReply* r)
{
    QString answer = r->readAll();
    QStringList list = answer.split(" ");
    int i=0;
    if (list[i++]=="200")
    {
        chatId = list[i++].toInt();
        bGainId = 1;
    }
}

void ManagerThread::replyWaitChatStatus(QNetworkReply* r)
{
    QString answer = r->readAll();
    if (answer=="200")
        bCanStartChat = 1;
    else
        bChatRejected = 1;
}

void ManagerThread::replyHowAreYou(QNetworkReply* r)
{
    QString answer = r->readAll();
    QStringList list = answer.split(" ");
    int i=0;
    if (list[i]=="200")
    {
        allOk = 1;
        okSplit = list;
    }
    else if (list[i]=="100")
    {
        bSomeoneWantToPlay = 1;
        otherUserId = list[++i].toInt();
    }
    else if (list[i]=="400")
    {
        bDisconnected = 1;
    }
}
}
