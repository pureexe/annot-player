#ifndef WBSEARCHEDIT_H
#define WBSEARCHEDIT_H

// gui/wbsearchedit.h
// 3/31/2012

#include "wbcomboedit.h"

class SearchEngine;

class WbSearchEdit : public WbComboEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(WbSearchEdit)
  typedef WbSearchEdit Self;
  typedef WbComboEdit Base;

public:
  explicit WbSearchEdit(QWidget *parent = 0)
    : Base(parent), engine_(0) { init(); }
  explicit WbSearchEdit(const QStringList &items, QWidget *parent = 0)
    : Base(items, parent), engine_(0) { init(); }

  void setEngines(const QList<SearchEngine *> engines)
  { engines_ = engines; invalidateEngines(); }

  QStringList recent() const;

protected:
  int recentCount() const { return count() - engines_.size(); }

signals:
  void engineChanged(int engine);
  void searchWithEngineRequested(const QString &text, int engine);

  // - Events -
protected:
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \reimp

  // - Actions -
public slots:
  void setText(const QString &text);
  void clearText() { setText(QString()); }
  void setEngine(int engine);
  void addRecent(const QString &text);
  void removeRecent(const QString &text);

protected slots:
  void invalidateEngines();
  void setEngineByIndex(int index);
  virtual void submitText(); ///< \reimp

  void updateText(const QString &text);
  void searchWithEngine(int engine);

private:
  void init()
  { createActions(); createConnections(); }

  void createActions();
  void createConnections();
protected:
  int engine_;

  QList<SearchEngine *> engines_;

  QString currentText_, lastText_;
};

#endif // WBSEARCHEDIT_H
