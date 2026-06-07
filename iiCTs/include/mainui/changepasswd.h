#ifndef CHANGEPASSWD_H
#define CHANGEPASSWD_H

#include <QWidget>
#include <QLabel>

class ChangePasswd : public QWidget
{
    Q_OBJECT
public:
    ChangePasswd(const QString &username, QWidget *parent = nullptr);
    void showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText);

private:
    QWidget *titleWidget;
    QLabel *sysTopLabel;
    QWidget *bottomInfoBox;

protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // CHANGEPASSWD_H
