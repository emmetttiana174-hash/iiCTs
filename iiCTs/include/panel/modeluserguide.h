#ifndef MODELUSERGUIDE_H
#define MODELUSERGUIDE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class ModelUserGuide : public QWidget {
    Q_OBJECT

public:
    explicit ModelUserGuide(QWidget* parent = nullptr);

private:
    QWidget* imageContainer;
    QLabel* imageLabel;
    QPixmap pixmapCT;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

};

#endif // MODELUSERGUIDE_H
