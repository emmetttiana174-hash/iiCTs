#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QWidget>
#include <QLabel>

class RegisterWindow : public QWidget {
    Q_OBJECT

public:
    RegisterWindow(QWidget *parent = nullptr);
    void showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText);

private:
    QWidget *titleWidget;
    QLabel *sysTopLabel;
    QWidget *bottomInfoBox;

protected:
    void resizeEvent(QResizeEvent *event) override;

};

#endif // REGISTERWINDOW_H
