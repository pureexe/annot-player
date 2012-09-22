#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

// renderthread.h
// 6/18/2011

#include <QThread>
#include <QImage>
#include <QList>

// Renderable
/**
 *  \brief  Interface for render thread.
 */
class Renderable
{
public:
  virtual ~Renderable() { }
  virtual void setImage(const QImage&) = 0;
};

// RenderTask
/**
 *  \breif  Task to render device.
 */
class RenderTask : public QObject
{
  Q_OBJECT
  typedef RenderTask Self;
  typedef QObject Base;

  Renderable *device_;

public:
  explicit RenderTask(Renderable *device, QObject *parent = nullptr)
    : Base(parent), device_(device) { }

  virtual Renderable *device() const { return device_; }
  virtual QImage render() const { return QImage(); }
};

// RenderThread
/**
 *  \brief Render thread
 */
class RenderThread  : public QThread
{
  Q_OBJECT
  typedef RenderThread Self;
  typedef QThread Base;

  QList<RenderTask*> tasks_;

public:
  explicit RenderThread(QObject *parent = nullptr)
    : Base(parent) { }

  void addTask(RenderTask *t)
  { if (t) tasks_ << t; }

signals:
  void renderableDevice(Renderable *device, const QImage &image);

protected:
  virtual void run()
  {
    foreach (RenderTask *t, tasks_)
      // Q_ASSERT(t);
      emit renderableDevice(t->device(), t->render());
  }
};

#endif // RENDERTHREAD_H
