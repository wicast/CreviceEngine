
#include "CoreLoader.hpp"
#include "EngineProxy.hpp"
#include "VKContainerWidget.hpp"

#include <QtWidgets>
#include "MainWindow.hpp"

int main(int argc, char *argv[]) {
  /* code */
  QApplication app(argc, argv);
  MainWindow mainWind;


  CoreLoader loader;
  loader.loadCore();

  QDockWidget *dock = new QDockWidget("Game", &mainWind);
  
  auto container = new VKContainerWidget(dock);

  auto render_server = loader.get_crevice_render_server_instance();

  loader.render_server_set_container(render_server, container);
  loader.render_server_init(render_server);

  EngineProxy proxy(&loader, &mainWind);

  mainWind.proxy = &proxy;

  dock->setWidget(container);
  mainWind.addDockWidget(Qt::RightDockWidgetArea, dock);

  dock = new QDockWidget("Timer", &mainWind);
  QPushButton* pBtnTest = new QPushButton(dock);
  pBtnTest->setText("get delta");
  QObject::connect(pBtnTest, &QAbstractButton::clicked, &proxy,
                   &EngineProxy::getDelta);
  QObject::connect(&proxy, &EngineProxy::delta_emit, pBtnTest,
                   &QPushButton::setText);

  dock->setWidget(pBtnTest);
  mainWind.addDockWidget(Qt::RightDockWidgetArea, dock);

  mainWind.show();
  return app.exec();
}
