#ifndef MODELDETECTORMODEL_H
#define MODELDETECTORMODEL_H

#include <QWidget>
#include <QPushButton>

class ModelDetectorModel : public QWidget {
    Q_OBJECT

public:
    explicit ModelDetectorModel(QWidget* parent = nullptr);

private:
    QWidget* imageContainer;
    QPushButton* btnRefresh;
    QPushButton* btnExport;
    QPushButton* btnSettings;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

};

#endif // MODELDETECTORMODEL_H
