#ifndef CHATSTARTWINDOW_H
#define CHATSTARTWINDOW_H

#include <QDialog>

namespace Ui {
class ChatStartWindow;
}

namespace livesheap{
class ChatStartWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ChatStartWindow(QWidget *parent = 0);
    ~ChatStartWindow();

private:
    static const int pixmaps = 4;
    int currentPixmap_;
private:
    Ui::ChatStartWindow *ui;

public:
    void changePixmap();
};
}

#endif // CHATSTARTWINDOW_H
