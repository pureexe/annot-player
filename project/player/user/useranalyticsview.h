#ifndef USERANALYTICSVIEW_H
#define USERANALYTICSVIEW_H

// useranalyticsview.h
// 5/7/2012

#include "project/common/acwebwindow.h"
#include "module/annotcloud/annotation.h"

QT_FORWARD_DECLARE_CLASS(QUrl)

class DataManager;

class UserAnalyticsView: public AcWebWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(UserAnalyticsView)
  typedef UserAnalyticsView Self;
  typedef AcWebWindow Base;

  typedef AnnotCloud::AnnotationList AnnotationList;

  // - Constructions -
public:
  explicit UserAnalyticsView(DataManager *data, QWidget *parent = 0);

public slots:
  void refresh();
  void setUserId(qint64 uid) { userId_ = uid; }

  // - Events -
public:
  virtual void setVisible(bool visible); ///< \override

protected slots:
  void setContent(const QString &html);
  void setUrl(const QUrl &url);

  // - Implementations
protected:
  void invalidateAnnotations();
  AnnotationList userAnnotatinons() const;
private:
  void setupActions();

private:
  DataManager *data_;
  qint64 userId_;
};

#endif // USERANALYTICSVIEW_H
