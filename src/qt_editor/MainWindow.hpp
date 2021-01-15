#pragma once

#include <QMainWindow>
#include "EngineProxy.hpp"


class MainWindow: public QMainWindow
{
private:
    /* data */
public:
    MainWindow(/* args */);
    ~MainWindow();

    //TODO manage all widget
    EngineProxy* proxy;

protected:
    void closeEvent(QCloseEvent *event) override;
};
