#ifndef INFOVIEW_H
#define INFOVIEW_H

// infoview.h
// 4/10/2012:%s

#include "module/qtext/dialog.h"
#include <QtCore/QString>

class AcTextView;

typedef QtExt::Dialog InfoViewBase;
class InfoView : public InfoViewBase
{
  Q_OBJECT
  typedef InfoView Self;
  typedef InfoViewBase Base;

  AcTextView *textView_;

public:
  explicit InfoView(QWidget *parent = 0);

protected:
  AcTextView *textView() const { return textView_; }

signals:
  void refreshRequested();

public slots:
  virtual void setVisible(bool t); ///< \override

protected slots:
  void refresh() { emit refreshRequested(); }

private:
  void createLayout();
};

#endif // INFOVIEW_H
