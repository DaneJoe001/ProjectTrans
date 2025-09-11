#include "client/view/new_upload_dialog.hpp"

NewUploadDialog::NewUploadDialog(QWidget* parent) :QDialog(parent)
{

}

void NewUploadDialog::init()
{
    this->setWindowTitle("New Upload");
    this->setGeometry(450, 250, 400, 500);
}