#ifndef NEWUSER_H
#define NEWUSER_H

#include <QDialog>

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

public slots:
    void ok();
    void reply(QNetworkReply*);
};
}

#endif // NEWUSER_H
