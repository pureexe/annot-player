#ifndef MEDIALIBRARYVIEW_H
#define MEDIALIBRARYVIEW_H

// medialibraryview.h
// 8/5/2012

#include "project/common/acmainwindow.h"
#include <QtCore/QModelIndex>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
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
    FT_Folder,
    FT_Game,
    FT_Video,
    FT_Url,
    FT_Count
  };

public:
  explicit MediaLibraryView(MediaLibrary *library, QWidget *parent = nullptr);

signals:
  void openRequested(const QString &fileName);
  void toggled();
  void showGameRequested(const QString &digest);

public:
  bool autoHide() const;
  bool autoRun() const;

  // -  Events -
public slots:
  void setVisible(bool visible) override;

protected:
  void contextMenuEvent(QContextMenuEvent *event) override;

  // - Actions -
protected slots:
  void showIcon();
  void showList();

  void refresh();
  void clear(bool confirm = true);
  void open();
  void browse();
  void showSelection();
  void showGame();

  void updateCount();
  void updateButtons();
  void updateFilterEdit();
  void setFilterType(int type);

  void updateContextMenu();

private:
  void createLayout();
  void createActions();

private:
  MediaLibrary *library_;
  QStackedLayout *views_;

  QListView *iconView_;
  QTreeView *listView_;

  QLineEdit *filterEdit_;
  QComboBox *filterType_;

  QToolButton *openButton_, *browseButton_, *countButton_;

  QSortFilterProxyModel *proxyModel_, *filterModel_;
  QItemSelectionModel *selectionModel_;

  QMenu *contextMenu_;
  QAction *autoHideAct_, *toggleAct_, *autoRunAct_,
          *openAct_, *browseAct_, *showGameAct_;
};

#endif // MEDIALIBRARYVIEW_H
