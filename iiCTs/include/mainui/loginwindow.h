#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QScreen>


class LoginWindow : public QWidget {
    Q_OBJECT
public:
    bool useDefaultGeometry = true;
    LoginWindow();
    void applyGeometry(const QRect &geometry) {
        this->setGeometry(geometry);
    }
    QLabel *bgLabel = new QLabel(this);
    void showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText);

signals:
    void loginSuccess(const QString &username, const QRect &geometry);

private slots:
    void handleLogin();
    void openRegisterWindow();
    void setupImage(QLabel *label, const QString &imagePath);


private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPoint dragPos;
    QWidget *titleWidget;
    QLabel *topImage;
    QLabel *sysTopLabel;
    QVBoxLayout *mainLayout;
    QWidget *bottomInfoBox = nullptr;

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int screenW = screenGeometry.width();
    int screenH = screenGeometry.height();

    QLabel *cTSystemContraryLabel;
    QLabel *tubeLabel;
    QLabel *detectorLabel;
    QLabel *computerLabel;
    QLabel *powerLabel;
    QLabel *line1Label;
    QLabel *line2Label;
    QLabel *line3Label;
    QLabel *line4Label;
    QLabel *line5Label;
    void updateImageGeometry();
    void drawLineWithCircles(QLabel &lineLabel, int x1, int y1, int x2, int y2, int lineWidth, int radius);

    bool dragging = false;
    bool resizing = false;
    QRect resizeRect;
    QPoint dragStartPos;
    enum ResizeRegion { None, Right, Bottom, BottomRight } resizeRegion;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    //    void paintEvent(QPaintEvent *event) override;
};

#endif // LOGINWINDOW_H
