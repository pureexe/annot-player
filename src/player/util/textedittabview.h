#ifndef TEXTEDITTABVIEW_H
#define TEXTEDITTABVIEW_H

// textedittabview.h
// 1/1/2012

#include "src/common/actabview.h"

class TextView;

class TextEditTabView : public AcTabView
{
  Q_OBJECT
  Q_DISABLE_COPY(TextEditTabView)
  typedef TextEditTabView Self;
  typedef AcTabView Base;

public:
  explicit TextEditTabView(QWidget *parent = nullptr)
  : Base(parent) { }

public slots:
  void appendText(const QString &text, int tab);
  void clear(int tab);

public:
  void addTab(const QString &name);
  TextView *textWidget(int tab) const;
};

#endif // TEXTEDITTABVIEW_H
