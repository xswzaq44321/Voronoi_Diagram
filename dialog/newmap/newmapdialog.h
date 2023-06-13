#ifndef NEWMAPDIALOG_H
#define NEWMAPDIALOG_H

#include <QDialog>
#include <QSize>

namespace Ui
{
class NewMapDialog;
}

class NewMapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewMapDialog(QWidget* parent = nullptr);
    ~NewMapDialog();
    QSize size;

private slots:
    void on_spinBox_width_valueChanged(int arg1);
    void on_spinBox_height_valueChanged(int arg1);

private:
    Ui::NewMapDialog* ui;
};

#endif  // NEWMAPDIALOG_H
