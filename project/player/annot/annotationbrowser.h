#ifndef ANNOTATIONBROWSER_H
#define ANNOTATIONBROWSER_H

// annotationbrowser.h
// 10/23/2011

#include "core/gui/dialog.h"
#include "core/cloud/annotation.h"
#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QStandardItemModel;
class QComboBox;
class QLineEdit;
class QSortFilterProxyModel;
class QTreeView;
class QMenu;
class QAction;
QT_END_NAMESPACE

class AnnotationEditor;

typedef Core::Gui::Dialog AnnotationBrowserBase;
class AnnotationBrowser : public AnnotationBrowserBase
{
  Q_OBJECT
  typedef AnnotationBrowser Self;
  typedef AnnotationBrowserBase Base;

  typedef Core::Cloud::Annotation Annotation;
  typedef Core::Cloud::AnnotationList AnnotationList;

protected:
  enum HeaderCloud {
    HD_Text = 0,
    HD_Pos,
    HD_UserAlias,
    HD_CreateTime,
    HD_UpdateTime,
    HD_Id,
    HD_Language,
    HD_Status,
    HD_UserId,
    HD_Flags,
    HD_BlessedCount,
    HD_CursedCount,
    HD_BlockedCount,
    HD_Count
  };

public:
  explicit AnnotationBrowser(QWidget *parent = 0);

signals:
  void annotationTextUpdated(QString text, qint64 id);

  // - Properties -
public:
  bool isEmpty() const;
  qint64 userId() const;

public slots:
  void setUserId(qint64 uid);
  void setAnnotations(const AnnotationList &l);
  void addAnnotation(const Annotation &annot);
  void addAnnotations(const AnnotationList &l);
  void clear();

protected slots:
  void saveAnnotationText(const QString &text);

protected:
  QModelIndex currentIndex() const;
  qint64 currentId() const;
  qint64 currentUserId() const;
  QString currentText() const;

  // - Events -
protected:
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

protected slots:
  virtual void dragEnterEvent(QDragEnterEvent *event); ///< \override
  virtual void dragMoveEvent(QDragMoveEvent *event); ///< \override
  virtual void dragLeaveEvent(QDragLeaveEvent *event); ///< \override
  virtual void dropEvent(QDropEvent *event); ///< \override

signals:
  void dragEnterEventReceived(QDragEnterEvent *event);
  void dragMoveEventReceived(QDragMoveEvent *event);
  void dragLeaveEventReceived(QDragLeaveEvent *event);
  void dropEventReceived(QDropEvent *event);

  // - Implementations -
protected slots:
  void invalidateFilterRegExp();
  void invalidateFilterColumn();

  void editAnnotation();
  void copyAnnotation();
  void hideAnnotation();
  void showAnnotation();
  void blessAnnotation();
  void curseAnnotation();
  void blockAnnotation();

  void viewUser();
  void blockUser();

protected:
  static void setHeaderData(QAbstractItemModel *model);

public:  // TO BE MOVED TO A COMMON PLACE
  static QString languageToString(int lang);
  static QStringList annotationFlagsToStringList(int flags);
  static QString annotationStatusToString(int flags);

private:
  void createLayout();
  void createActions();

private:
  qint64 userId_;

  AnnotationEditor *editor_;
  QModelIndex editedIndex_;

  // - Modes and UI components -
  QStandardItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;
  QTreeView *proxyView_;

  QLineEdit *filterPatternLineEdit_;
  QComboBox *filterSyntaxComboBox_;
  QComboBox *filterColumnComboBox_;

  // - Menus and actions -
  QMenu *contextMenu_;

  QAction *editAnnotAct_,
          *copyAnnotAct_,
          *blockAnnotAct_,
          *hideAnnotAct_,
          *showAnnotAct_,
          *blessAnnotAct_,
          *curseAnnotAct_;

  QAction *viewUserAct_,
          *blockUserAct_;
};

#endif // ANNOTATIONBROWSER_H
