#ifndef TEXTVIEW_H
#define TEXTVIEW_H

// textedittabview.h
// 8/12/2012

#include "src/common/actextview.h"

class AcTextView;

class TextView : public AcTextView
{
  Q_OBJECT
  Q_DISABLE_COPY(TextView)
  typedef TextView Self;
  typedef AcTextView Base;

  QString selectedText_;
  bool selectionMonitored_;

public:
  explicit TextView(QWidget *parent = nullptr);

signals:
  void selectedTextChanged(const QString &text);

public slots:
  void setSelectionMonitored(bool t) { selectionMonitored_ = t; }

protected slots:
  void updateSelectedText();
};

#endif // TEXTVIEW_H
