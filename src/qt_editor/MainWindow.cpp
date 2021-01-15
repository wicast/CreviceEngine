#include "MainWindow.hpp"

void MainWindow::closeEvent(QCloseEvent *event) {
    proxy->stopAll();
}

MainWindow::MainWindow(/* args */)
{
}

MainWindow::~MainWindow()
{
}
