#ifndef VERTICALBUTTON_H
#define VERTICALBUTTON_H

#include <QPushButton>
#include <QPainter>
#include <QStyleOptionButton>

class VerticalPushButton : public QPushButton {
    Q_OBJECT

public:
    explicit VerticalPushButton(const QString& text, QWidget* parent = nullptr)
        : QPushButton(text, parent)
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    QSize sizeHint() const override {
        QSize s = QPushButton::sizeHint();
        return QSize(s.height(), s.width());
    }

protected:
    void paintEvent(QPaintEvent* /*event*/) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.translate(width(), 0);
        painter.rotate(90);

        QStyleOptionButton opt;
        initStyleOption(&opt);
        opt.rect = QRect(0, 0, height(), width());

        style()->drawControl(QStyle::CE_PushButtonBevel, &opt, &painter, this);
        style()->drawControl(QStyle::CE_PushButtonLabel, &opt, &painter, this);
    }
};

#endif // VERTICALBUTTON_H
