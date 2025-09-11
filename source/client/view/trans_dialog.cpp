#include "client/view/trans_dialog.hpp"

TransDialog::TransDialog(QWidget* parent)
    : QDialog(parent)
{

}

void TransDialog::init()
{
    setWindowTitle("Trans Dialog");
    this->setGeometry(450, 250, 400, 500);
}