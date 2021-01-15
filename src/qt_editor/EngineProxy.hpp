#pragma once

#include <QDebug>
#include <QTimer>
#include <QWidget>

#include "CoreLoader.hpp"

class EngineProxy : public QWidget {
  Q_OBJECT
 private:
  /* data */
  CoreLoader* _loader;
  QTimer* timer;
  // TODO multiple world
  CVEcsWorld _ecs_world;

  // CVVkRenderServer _render_server;

 public:
  EngineProxy(CoreLoader* loader, QWidget* parent = nullptr);
  ~EngineProxy();

 public slots:
  float getDelta();
  void stopAll();
 signals:
  void delta_emit(const QString& delta);
};
