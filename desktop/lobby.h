#ifndef LOBBY_H
#define LOBBY_H

#include <QDialog>
#include <QUrl>

namespace Ui {
class Lobby;
}

class QNetworkReply;
class QNetworkRequest;
class QNetworkAccessManager;
namespace livesheap{
class ManagerThread;
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
private:
    bool bLoggedIn_;
    Ui::Lobby *ui;
    bool bCanQuit_;
    QTimer *timer_;
    int exitTimerCount_;
    static const int exitTimerMaxCount = 20;

private:
    void reaction(const QString&); //реакция на сообщение от сервера / ... on server message
protected:
    void closeEvent(QCloseEvent*);
public:
    void start();
public slots:
    void reply(QNetworkReply*); //сообщение от сервера / ... from server
private slots:
    void exitTimerUpdate();
signals:
    void postSignal(const QNetworkRequest&, const QByteArray&);
};
}

#endif // LOBBY_H
