#ifndef ADDALIASDIALOG_H
#define ADDALIASDIALOG_H

// addaliasdialog.h
// 10/13/2011

#include "module/qtext/dialog.h"

QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QToolButton)

typedef QtExt::Dialog AddAliasDialogBase;
class AddAliasDialog : public AddAliasDialogBase
{
  Q_OBJECT
  typedef AddAliasDialog Self;
  typedef AddAliasDialogBase Base;

public:
  explicit AddAliasDialog(QWidget *parent = 0);

signals:
  void aliasAdded(const QString &alias, int type, qint32 language);

  // - Properties -
public:
  quint32 languageFlags() const;

  // - Slots -
protected slots:
  void ok();
  void paste();

  void tag01()          { tag("01"); }
  void tag02()          { tag("02"); }
  void tag03()          { tag("03"); }
  void tag04()          { tag("04"); }
  void tag05()          { tag("05"); }
  void tagOVA()         { tag("OVA"); }
  void tagOAD()         { tag("OAD"); }

  //void tagBD()          { tag("BD"); }
  //void tagDVD()         { tag("DVD"); }
  //void tagTV()          { tag("TV"); }
  //void tagWeb()         { tag("Web"); }

  void invalidateOKButton();

  void setTypeToName(bool t);
  void setTypeToTag(bool t);
  void setTypeToUrl(bool t);

protected:
  void tag(const QString &tag);
  //void postfix(const QString &tag);

  //virtual void mouseDoubleClickEvent(QMouseEvent *event);
  //virtual void contextMenuEvent(QContextMenuEvent *event);

private:
  QComboBox *aliasEdit_;
  QToolButton *okButton_, *cancelButton_;

  QToolButton *isNameButton_, *isTagButton_, *isUrlButton_;

  QToolButton *isEnglishButton_,
              *isJapaneseButton_,
              *isChineseButton_,
              *isAlienButton_;
};

#endif // ADDALIASDIALOG_H
