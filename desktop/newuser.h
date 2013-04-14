/*
Другой из диалогов / just that another one
copyright (c) 2013 Евгений Лежнин
  */
#ifndef NEWUSER_H
#define NEWUSER_H

#include <QDialog>
#include <QUrl>

namespace Ui {
class NewUser;
}

class QNetworkReply;
class QNetworkAccessManager;
namespace livesheap{
class NewUser : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewUser(QWidget *parent = 0);
    ~NewUser();

private:
    Ui::NewUser *ui;
    QNetworkAccessManager *pManager;
    QUrl serverUrl; //сохраняется в самом начале

private:
    QString reaction(const QString&); //реакция на сообщение от сервера / ... on server message
public slots:
    void ok();
    void reply(QNetworkReply*); //сообщение от сервера / ... from server
};
}

#endif // NEWUSER_H
