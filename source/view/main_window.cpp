#include "view/main_window.hpp"

MainWindow::MainWindow(QWidget* parent)
{
    setWindowTitle(m_window_title);
    setGeometry(400, 400, 800, 600);
}

MainWindow::~MainWindow()
{

}