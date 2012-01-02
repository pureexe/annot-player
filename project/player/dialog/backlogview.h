#ifndef BACKLOGEVIEW_H
#define BACKLOGEVIEW_H

// backlogview.h
// 1/1/2012

#include "core/gui/dialog.h"

QT_FORWARD_DECLARE_CLASS(QToolButton)
QT_FORWARD_DECLARE_CLASS(QStackedLayout)

class TextView;

typedef Core::Gui::Dialog BacklogViewBase;
class BacklogView : public BacklogViewBase
{
  Q_OBJECT
  typedef BacklogView Self;
  typedef BacklogViewBase Base;

  enum TabIndex {
    AnnotationTabIndex = 0,
    SubtitleTabIndex,
    TextTabIndex,
    TabIndexCount
  };

public:
  explicit BacklogView(QWidget *parent = 0);

public slots:
  void appendAnnotation(const QString &text);
  void appendSubtitle(const QString &text);
  void appendText(const QString &text);
  void clear();

protected slots:
  void setTabIndex(TabIndex tab) { tabIndex_ = tab; invalidateTabIndex(); }
  void invalidateTabIndex();

  void setAnnotationTab() { setTabIndex(AnnotationTabIndex); }
  void setSubtitleTab()   { setTabIndex(SubtitleTabIndex); }
  void setTextTab()       { setTabIndex(TextTabIndex); }

private:
  void createLayout();

private:
  TabIndex tabIndex_;
  QStackedLayout *stackLayout_;

  QToolButton *annotationButton_,
              *subtitleButton_,
              *textButton_;

  TextView    *annotationView_,
              *subtitleView_,
              *textView_;
};

#endif // BACKLOGVIEW_H
