#ifndef BACKLOGDIALOG_H
#define BACKLOGDIALOG_H

// backlogview.h
// 1/1/2012

#include "project/common/acwindow.h"

class TextEditTabView;

class BacklogDialog : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(BacklogDialog)
  typedef BacklogDialog Self;
  typedef AcWindow Base;

  enum TabIndex {
    AnnotationTabIndex = 0,
    SubtitleTabIndex,
    TextTabIndex,
    TabIndexCount
  };

public:
  explicit BacklogDialog(QWidget *parent = 0);

public slots:
  void appendAnnotation(const QString &text);
  void appendSubtitle(const QString &text);
  void appendText(const QString &text);
  void clear();

private:
  void createLayout();

private:
  TextEditTabView *tabView_;
};

#endif // BACKLOGDIALOG_H
