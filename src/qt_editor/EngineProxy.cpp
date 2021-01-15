#include "EngineProxy.hpp"

#include <QString>

EngineProxy::EngineProxy(CoreLoader* loader,QWidget *parent):QWidget(parent) {
  _loader = loader;

  loader->create_engine_handler();
  _ecs_world = loader->cv_ecs_new_world();
  loader->cv_ecs_setup(_ecs_world);

  timer = new QTimer();
  connect(timer, &QTimer::timeout, [this, loader] {
    loader->timer_tick();
    loader->cv_ecs_progress(_ecs_world);
  });
  timer->start();
}

EngineProxy::~EngineProxy() {}

float EngineProxy::getDelta() {
  auto delta = _loader->timer_delta();
  emit delta_emit(QString("delta is %1").arg(delta));
  return delta;
}

void EngineProxy::stopAll() {
  timer->stop();
  auto render_server = _loader->get_crevice_render_server_instance();
  _loader->render_server_wait_idle(render_server);
}