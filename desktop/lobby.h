#ifndef LOBBY_H
#define LOBBY_H

#include <QDialog>
#include <QUrl>
#include <QModelIndex>
#include "useritem.h"

namespace Ui {
class Lobby;
}

class QNetworkReply;
class QNetworkRequest;
class QNetworkAccessManager;
namespace livesheap{
class ManagerThread;
class ChatStartWindow;
class Lobby : public QDialog
{
    Q_OBJECT

public:
    explicit Lobby(QWidget *parent = 0);
    ~Lobby();

public:
    QString username;
    QUrl serverUrl; //сохраняется в самом начале
    int id_;
    ManagerThread *pManagerThread;
    ManagerThread *pConstantManager;
private:
    bool bLoggedIn_;
    Ui::Lobby *ui;
    bool bCanQuit_;
    QTimer *exiTimer_;
    QTimer *chaTimer_;
    QTimer *howAreYouTimer_;
    int exitTimerCount_;
    int startChatTimerCount_;
    int waitingId_;
    static const int exitTimerMaxCount = 20;
    static const int startChatMaxCount = 1000;
    static const int normalTimeout = 100;
    static const int howAreYouTimeout = 500;
    std::vector<UserItem*> users_;
    ChatStartWindow *chatStartWaitWindow_;
    bool bStartedAskingIsSheAnswered_;

private:
    void reaction(const QString&); //реакция на сообщение от сервера / ... on server message
    void startChat();
    void clear();
protected:
    void closeEvent(QCloseEvent*);
public:
    void start();
public slots:
    void reply(QNetworkReply*); //сообщение от сервера / ... from server
private slots:
    void exitTimerUpdate();
    void userSelected(QModelIndex);
    void waitForAnswerStartChat();
    void howAreYou();
signals:
    void postSignal(const QNetworkRequest&, const QByteArray&);
};
}

#endif // LOBBY_H
