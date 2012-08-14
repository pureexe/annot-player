#ifndef TOKENVIEW_H
#define TOKENVIEW_H

// tokenview.h
// 8/13/2011

#include "project/common/acwindow.h"
#include "module/annotcloud/token.h"
#include "module/annotcloud/alias.h"
#include "module/searchengine/searchenginefactory.h"
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

class AnnotationServerAgent;
class AddAliasDialog;
class DataManager;
class SignalHub;

class TokenView : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(TokenView)
  typedef TokenView Self;
  typedef AcWindow Base;

  typedef AnnotCloud::Token Token;
  typedef AnnotCloud::TokenList TokenList;
  typedef AnnotCloud::Alias Alias;
  typedef AnnotCloud::AliasList AliasList;

  //Token token_;
  //AliasList aliases_;

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
    HD_BlessCount,
    HD_CurseCount,
    HD_BlockCount,
    HD_Count
  };

public:
  TokenView(DataManager *data, AnnotationServerAgent *server, SignalHub *hub, QWidget *parent = nullptr);

  //qint64 userId() const;
  //void setUserId(qint64 uid);

signals:
  void message(QString text);
  void warning(QString text);

  void aliasSubmitted(const Alias &alias);
  void aliasDeletedWithId(qint64 id);
  void tokenBlessedWithId(qint64 tid);
  void tokenCursedWithId(qint64 tid);
  void updateAnnotationsRequested();
  void openUrlRequested(QString url);
  void searchRequested(int engine, const QString &key);

  // - Properties -
public:
  //const Token &token() const { return token_; }
  //Token &token() { return token_; }
  //bool hasToken() const;

protected:
  QModelIndex currentIndex() const;

  // - Slots -
public slots:
  void refresh();
  //void setToken(const Token &token);
  //void clearToken(); ///< Clear token
  void invalidateToken();

  //void setSource(const QString &source);

  void setAliases(const AliasList &l);
  void addAlias(const Alias &alias);
  void addAliases(const AliasList &l);
  void clearAliases();

  void addAlias();
  //void bless();
  //void curse();

  // - Events -
public:
  void setVisible(bool visible) override;
protected slots:
  void updateLabels();
  void updateButtons();

  void submitAlias(const QString &alias, int type, qint32 language);

protected slots:
  void contextMenuEvent(QContextMenuEvent *event) override;

//  void dragEnterEvent(QDragEnterEvent *event) override;
//  void dragMoveEvent(QDragMoveEvent *event) override;
//  void dragLeaveEvent(QDragLeaveEvent *event) override;
//  void dropEvent(QDropEvent *event) override;

//signals:
//  void dragEnterEventReceived(QDragEnterEvent *event);
//  void dragMoveEventReceived(QDragMoveEvent *event);
//  void dragLeaveEventReceived(QDragLeaveEvent *event);
//  void dropEventReceived(QDropEvent *event);

  // - Format -
protected:
  static int aliasTypeFromString(const QString &text);
  static QString aliasTypeToString(int t);
  static QString languageToString(int lang);
  static QStringList aliasFlagsToStringList(int flags);
  static QString aliasStatusToString(int flags);

  // - Context menu -
protected slots:
  void deleteAlias();
  void copyAlias();
  void openAliasUrl();
  void setActive(bool active);

  qint64 currentAliasId() const;
  qint64 currentAliasUserId() const;
  QString currentAliasText() const;
  int currentAliasType() const;

protected slots:
  void searchAliasWithGoogle() { emit searchRequested(SearchEngineFactory::Google, currentAliasText()); }
  void searchAliasWithGoogleImages() { emit searchRequested(SearchEngineFactory::GoogleImages, currentAliasText()); }
  void searchAliasWithBing()   { emit searchRequested(SearchEngineFactory::Bing, currentAliasText()); }
  void searchAliasWithNicovideo() { emit searchRequested(SearchEngineFactory::Nicovideo, currentAliasText()); }
  void searchAliasWithBilibili() { emit searchRequested(SearchEngineFactory::Bilibili, currentAliasText()); }
  void searchAliasWithAcfun()  { emit searchRequested(SearchEngineFactory::Acfun, currentAliasText()); }
  void searchAliasWithYoutube() { emit searchRequested(SearchEngineFactory::Youtube, currentAliasText()); }
  void searchAliasWithYouku()  { emit searchRequested(SearchEngineFactory::Youku, currentAliasText()); }
  void searchAliasWithWikiEn() { emit searchRequested(SearchEngineFactory::WikiEn, currentAliasText()); }
  void searchAliasWithWikiJa() { emit searchRequested(SearchEngineFactory::WikiJa, currentAliasText()); }
  void searchAliasWithWikiZh() { emit searchRequested(SearchEngineFactory::WikiZh, currentAliasText()); }

  // - Implementations -
protected:
  static void setAliasHeaderData(QAbstractItemModel *model);

private:
  void createLayout();
  void createContextMenu();
  void createSearchEngines();
  void createSearchMenu();

private:
  bool active_;
  DataManager *data_;
  AnnotationServerAgent *server_;
  SignalHub *hub_;
  //qint64 userId_;
  QStandardItemModel *sourceModel_; // for alias
  QSortFilterProxyModel *proxyModel_;
  AcFilteredTableView *tableView_; // for alias

  //QLabel *createDateLabel_,
  //       *annotCountLabel_,
  //       *blessCountLabel_,
  //       *curseCountLabel_,
  //       *visitCountLabel_;

  QToolButton *addButton_,
              *updateButton_;

  QMenu *contextMenu_,
        *searchMenu_;
  QAction *copyAliasAct_,
          *deleteAliasAct_,
          *openAliasUrlAct_;
          //*editAliasAct_,
          //*blockAliasAct_,
          //*blessAliasAct_,
          //*curseAliasAct_;

  AddAliasDialog *aliasDialog_;
  QList<SearchEngine *> searchEngines_;
};

#endif // TOKENVIEW_H
