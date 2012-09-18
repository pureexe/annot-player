#ifndef QXACTIONWITHID_H
#define QXACTIONWITHID_H

// qxactionwithid.h
// 9/2/2011

#include "qtx/qtxglobal.h"
#include <QtGui/QAction>

QTX_BEGIN_NAMESPACE

class QxActionWithId : public QAction
{
  Q_OBJECT
  Q_DISABLE_COPY(QxActionWithId)
  typedef QxActionWithId Self;
  typedef QAction Base;

  int id_; // init to be zero

public:
  explicit QxActionWithId(QObject *parent = nullptr)
    : Base(parent), id_(0) { init(); }
  QxActionWithId(const QString &text, QObject *parent = nullptr)
    : Base(text, parent), id_(0) { init(); }
  explicit QxActionWithId(int id, QObject *parent = nullptr)
    : Base(parent), id_(id) { init(); }
  QxActionWithId(int id, const QString &text, QObject *parent = nullptr)
    : Base(text, parent), id_(id) { init(); }
  QxActionWithId(const QIcon &icon, const QString &text, QObject *parent = nullptr)
    : Base(icon, text, parent), id_(0) { init(); }
  QxActionWithId(int id, const QIcon &icon, const QString &text, QObject *parent = nullptr)
    : Base(icon, text, parent), id_(id) { init(); }

  int id() const { return id_; }
public slots:
  void setId(int id) { id_ = id; }

signals:
  void changedWithId(int id);
  void hoveredWithId(int id);
  void toggledWithId(int id, bool checked);
  void triggeredWithId(int id, bool checked = false);

// - Implementations -
private slots:
  void emit_changedWithId() { emit changedWithId(id_); }
  void emit_hoveredWithId() { emit hoveredWithId(id_); }
  void emit_toggledWithId(bool checked) { emit toggledWithId(id_, checked); }
  void emit_triggeredWithId(bool checked = false) { emit triggeredWithId(id_, checked); }

private:
  void init();
};

QTX_END_NAMESPACE

#endif // QXACTIONWITHID_H
