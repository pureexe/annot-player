#ifndef MEDIALIBRARYVIEW_H
#define MEDIALIBRARYVIEW_H

// medialibraryview.h
// 8/5/2012

#include "project/common/acmainwindow.h"

QT_BEGIN_NAMESPACE
class QListView;
class QStackedLayout;
class QToolButton;
class QTreeView;
class QLineEdit;
class QComboBox;
class QSortFilterProxyModel;
class QItemSelectionModel;
QT_END_NAMESPACE

class MediaLibrary;
class MediaLibraryView : public AcMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(MediaLibraryView)
  typedef MediaLibraryView Self;
  typedef AcMainWindow Base;

  enum FilterType {
    FT_All,
    FT_Game,
    FT_Video,
    FT_Url,
    FT_Count
  };

public:
  explicit MediaLibraryView(MediaLibrary *library, QWidget *parent = nullptr);

signals:
  void openRequested(const QString &fileName);

  // -  Events -
public slots:
  void setVisible(bool visible) override;

  // - Actions -
protected slots:
  void showIcon();
  void showList();
  void showTree();

  void refresh();
  void clear(bool confirm = true);
  void open();
  void showSelection();
  void openDirectory();

  void updateCount();
  void updateButtons();
  void updateFilterEdit();
  void setFilterType(int type);

private:
  void createLayout();

private:
  MediaLibrary *library_;
  QStackedLayout *views_;

  QListView *iconView_;
  QTreeView *listView_;
  QTreeView *treeView_;

  QLineEdit *filterEdit_;
  QComboBox *filterType_;

  QToolButton *openButton_, *dirButton_, *countButton_;

  QSortFilterProxyModel *standardProxyModel_, *standardFilterModel_;
  QItemSelectionModel *standardSelectionModel_;
};

#endif // MEDIALIBRARYVIEW_H
