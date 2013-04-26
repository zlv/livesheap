#ifndef MANAGERTHREAD_H
#define MANAGERTHREAD_H

#include <QObject>
#include <QStringList>

class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;
namespace livesheap{
class ManagerThread : public QObject
{
    Q_OBJECT
public:
    explicit ManagerThread(QObject *parent = 0);

public:
    QNetworkAccessManager *pManager;
    bool bCanQuit;
    bool bCanStartChat, bChatRejected, bGainId;
    int chatId;
    bool allOk, bDisconnected, bSomeoneWantToPlay;
    int otherUserId;
    QStringList okSplit;

public:
    void emitPost(const QNetworkRequest&, const QByteArray&);
signals:
    void finished(QNetworkReply*);
public slots:
    void post(const QNetworkRequest&, const QByteArray&);
    void replyCanQuit(QNetworkReply*);
    void replyChatStatus(QNetworkReply*);
    void replyWaitChatStatus(QNetworkReply*);
    void replyHowAreYou(QNetworkReply*);
signals:
    void postSignal(const QNetworkRequest&, const QByteArray&);
};
}

#endif // MANAGERTHREAD_H
