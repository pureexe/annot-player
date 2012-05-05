#ifndef TOKENVIEW_H
#define TOKENVIEW_H

// tokenview.h
// 8/13/2011

#include "module/qtext/dialog.h"
#include "module/annotcloud/token.h"
#include "module/annotcloud/alias.h"
#include <QtCore/QModelIndex>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QAction;
class QLabel;
class QMenu;
class QStandardItemModel;
class QSortFilterProxyModel;
class QToolButton;
QT_END_NAMESPACE

class AcFilteredTableView;

class AddAliasDialog;
class ServerAgent;

typedef QtExt::Dialog TokenViewBase;

class TokenView : public TokenViewBase
{
  Q_OBJECT
  Q_DISABLE_COPY(TokenView)
  typedef TokenView Self;
  typedef TokenViewBase Base;

  typedef AnnotCloud::Token Token;
  typedef AnnotCloud::TokenList TokenList;
  typedef AnnotCloud::Alias Alias;
  typedef AnnotCloud::AliasList AliasList;

  Token token_;
  AliasList aliases_;

protected:
  enum AliasHeaderData {
    HD_Text = 0,
    HD_Type,
    HD_UpdateTime,
    HD_Language,
    HD_Status,
    HD_Flags,
    HD_Id,
    HD_UserId,
    HD_BlessedCount,
    HD_CursedCount,
    HD_BlockedCount,
    HD_Count
  };

public:
  explicit TokenView(ServerAgent *server, QWidget *parent = 0);

  //qint64 userId() const;
  //void setUserId(qint64 uid);

signals:
  void aliasSubmitted(const Alias &alias);
  void aliasDeletedWithId(qint64 id);
  void tokenBlessedWithId(qint64 tid);
  void tokenCursedWithId(qint64 tid);

  // - Properties -
public:
  const Token &token() const { return token_; }
  Token &token() { return token_; }
  bool hasToken() const;

protected:
  QModelIndex currentIndex() const;

  // - Slots -
public slots:
  void setToken(const Token &token);
  void clearToken(); ///< Clear token

  void setSource(const QString &source);

  void setAliases(const AliasList &l);
  void addAlias(const Alias &alias);
  void addAliases(const AliasList &l);
  void clearAliases();

  void addAlias();
  void bless();
  void curse();

  // - Events -
public:
  virtual void setVisible(bool visible); ///< \override
protected slots:
  void updateTokenLabels();

  void submitAlias(const QString &alias, int type, qint32 language);

protected slots:
  virtual void dragEnterEvent(QDragEnterEvent *event); ///< \override
  virtual void dragMoveEvent(QDragMoveEvent *event); ///< \override
  virtual void dragLeaveEvent(QDragLeaveEvent *event); ///< \override
  virtual void dropEvent(QDropEvent *event); ///< \override

  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

signals:
  void dragEnterEventReceived(QDragEnterEvent *event);
  void dragMoveEventReceived(QDragMoveEvent *event);
  void dragLeaveEventReceived(QDragLeaveEvent *event);
  void dropEventReceived(QDropEvent *event);

  // - Format -
protected:
  static QString aliasTypeToString(int t);
  static QString languageToString(int lang);
  static QStringList aliasFlagsToStringList(int flags);
  static QString aliasStatusToString(int flags);

  // - Context menu -
protected slots:
  void deleteAlias();
  void copyAlias();
  void openSource();

  qint64 currentAliasId() const;
  qint64 currentAliasUserId() const;
  QString currentAliasText() const;

  // - Implementations -
protected:
  static void setAliasHeaderData(QAbstractItemModel *model);

private:
  void createLayout();
  void createActions();

private:
  ServerAgent *server_;
  //qint64 userId_;
  QStandardItemModel *sourceModel_; // for alias
  QSortFilterProxyModel *proxyModel_;
  AcFilteredTableView *tableView_; // for alias

  QLabel *createDateLabel_,
         *annotCountLabel_,
         *blessedCountLabel_,
         *cursedCountLabel_,
         *visitedCountLabel_;

  QToolButton *sourceButton_;

  QMenu *contextMenu_;
  QAction *copyAliasAct_,
          *deleteAliasAct_;
          //*editAliasAct_,
          //*blockAliasAct_,
          //*blessAliasAct_,
          //*curseAliasAct_;

  AddAliasDialog *aliasDialog_;
};

#endif // TOKENVIEW_H
