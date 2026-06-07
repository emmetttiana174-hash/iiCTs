#ifndef VERTICALLABEL_H
#define VERTICALLABEL_H

#include <QLabel>
#include <QPainter>

class VerticalLabel : public QLabel {
    Q_OBJECT

public:
    explicit VerticalLabel(const QString& text, QWidget* parent = nullptr)
        : QLabel(text, parent)
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    QSize sizeHint() const override {
        QSize s = QLabel::sizeHint();
        return QSize(s.height(), s.width());
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::TextAntialiasing);

        painter.translate(width()  / 2.0,
                          height() / 2.0);
        painter.rotate(90);
        painter.translate(-height() / 2.0,
                          -width()  / 2.0);

        QRect rect(0, 0, height(), width());
        painter.drawText(rect, alignment(), text());
    }
};

#endif // VERTICALLABEL_H
