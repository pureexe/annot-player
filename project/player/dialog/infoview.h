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
  explicit InfoView(QWidget *parent = nullptr);

protected:
  AcTextView *textView() const { return textView_; }

public slots:
  void setVisible(bool t) override;

public slots:
  virtual void refresh() { }

private:
  void createLayout();
};

#endif // INFOVIEW_H
