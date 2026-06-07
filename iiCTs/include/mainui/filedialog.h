#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QFileDialog>
#include <QDialog>

class FileDialog : public QFileDialog {
public:
    using QFileDialog::QFileDialog;

    static QString getOpenFileName(QWidget* parent = nullptr,
                                   const QString& caption = QString(),
                                   const QString& dir     = QString(),
                                   const QString& filter  = QString())
    {
        FileDialog dlg(parent, caption, dir, filter);
        dlg.setOption(QFileDialog::DontUseNativeDialog);
        dlg.setFileMode(QFileDialog::ExistingFile);

        dlg.setStyleSheet(R"(
            QFileDialog, QFileDialog QWidget, QFileDialog QAbstractItemView {
                background: white; color: black;
            }
            QFileDialog QPushButton {
                background: #f0f0f0; color: black;
            }
            QFileDialog QLineEdit, QFileDialog QComboBox {
                background: white; color: black;
            }
        )");

        return dlg.exec() == QDialog::Accepted
               ? dlg.selectedFiles().first()
               : QString();
    }

    static QString getExistingDirectory(QWidget* parent = nullptr,
                                        const QString& caption = QString(),
                                        const QString& dir     = QString(),
                                        Options options       = ShowDirsOnly | DontResolveSymlinks)
    {
        FileDialog dlg(parent, caption, dir);

        dlg.setOption(DontUseNativeDialog);
        dlg.setFileMode(QFileDialog::Directory);

        dlg.setOptions(options | DontUseNativeDialog);

        dlg.setStyleSheet(R"(
            QFileDialog, QFileDialog QWidget, QFileDialog QAbstractItemView {
                background: white; color: black;
            }
            QFileDialog QPushButton {
                background: #f0f0f0; color: black;
            }
            QFileDialog QLineEdit, QFileDialog QComboBox {
                background: white; color: black;
            }
        )");

        return dlg.exec() == QDialog::Accepted
               ? dlg.selectedFiles().first()
               : QString();
    }
};

#endif // FILEDIALOG_H
