#ifndef INFOVIEW_H
#define INFOVIEW_H

// infoview.h
// 4/10/2012:%s

#include "project/common/acwindow.h"
#include <QtCore/QString>

class AcTextView;

class InfoView : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(InfoView)
  typedef InfoView Self;
  typedef AcWindow Base;

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
