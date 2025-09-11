#include "client/view/new_download_dialog.hpp"

NewDownloadDialog::NewDownloadDialog(QWidget* parent) :QDialog(parent)
{

}

void NewDownloadDialog::init()
{
    this->setWindowTitle("New Download");
    this->setGeometry(450, 250, 400, 500);
}