#include "newmapdialog.h"
#include "ui_newmapdialog.h"

NewMapDialog::NewMapDialog(QWidget* parent)
    : QDialog(parent),
      ui(new Ui::NewMapDialog)
{
    ui->setupUi(this);

    size.setWidth(ui->spinBox_width->value());
    size.setHeight(ui->spinBox_height->value());
}

NewMapDialog::~NewMapDialog()
{
    delete ui;
}

void NewMapDialog::on_spinBox_width_valueChanged(int arg1)
{
    size.setWidth(arg1);
}

void NewMapDialog::on_spinBox_height_valueChanged(int arg1)
{
    size.setHeight(arg1);
}
