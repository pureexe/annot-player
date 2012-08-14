#ifndef BACKLOGDIALOG_H
#define BACKLOGDIALOG_H

// backlogview.h
// 1/1/2012

#include "project/common/acwindow.h"

#ifdef PLAYER_ENABLE_GAME
# define BACKLOGDIALOG_HAS_TEXT_TAB
#endif // PLAYER_ENABLE_GAME

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
#ifdef BACKLOGDIALOG_HAS_TEXT_TAB
    TextTabIndex,
#endif // BACKLOGDIALOG_HAS_TEXT_TAB
    TabIndexCount
  };

public:
  explicit BacklogDialog(QWidget *parent = nullptr);

public slots:
  void appendAnnotation(const QString &text);
  void appendSubtitle(const QString &text);
#ifdef BACKLOGDIALOG_HAS_TEXT_TAB
  void appendText(const QString &text);
#endif // BACKLOGDIALOG_HAS_TEXT_TAB
  void clear();

signals:
  void translateRequested(const QString &text);
protected slots:
  void setTranslateEnabled(bool t);

private:
  void createLayout();

private:
  TextEditTabView *tabView_;
};

#endif // BACKLOGDIALOG_H
