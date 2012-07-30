#ifndef USERANALYTICSVIEW_H
#define USERANALYTICSVIEW_H

// useranalyticsview.h
// 5/7/2012

#include "project/common/acwebwindow.h"
#include "module/annotcloud/annotation.h"

QT_FORWARD_DECLARE_CLASS(QUrl)

class DataManager;
class SignalHub;

class UserAnalyticsView: public AcWebWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(UserAnalyticsView)
  typedef UserAnalyticsView Self;
  typedef AcWebWindow Base;

  typedef AnnotCloud::AnnotationList AnnotationList;

  // - Constructions -
public:
  UserAnalyticsView(DataManager *data, SignalHub *hub, QWidget *parent = nullptr);

public slots:
  void refresh();
  void setUserId(qint64 uid) { userId_ = uid; }

  // - Events -
public:
  void setVisible(bool visible) override;

protected slots:
  void setContent(const QString &html);
  void setUrl(const QUrl &url);

  // - Implementations
protected:
  void invalidateAnnotations();
  AnnotationList userAnnotations() const;
private:
  void setupActions();

private:
  DataManager *data_;
  SignalHub *hub_;
  qint64 userId_;
};

#endif // USERANALYTICSVIEW_H
