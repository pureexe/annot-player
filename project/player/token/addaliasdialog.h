#ifndef ADDALIASDIALOG_H
#define ADDALIASDIALOG_H

// addaliasdialog.h
// 10/13/2011

#include "module/qtext/dialog.h"

QT_FORWARD_DECLARE_CLASS(QLineEdit)
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
  void aliasAdded(const QString &alias, int type, quint32 language);

  // - Properties -
public:
  quint32 languageFlags() const;

  // - Slots -
protected slots:
  void ok();
  void paste();
  void cancel();

  void tagBD()          { tag("BD"); }
  void tagDVD()         { tag("DVD"); }
  void tagTV()          { tag("TV"); }
  void tagWeb()         { tag("Web"); }

  void invalidateOKButton();

  void setTypeToName(bool t);
  void setTypeToTag(bool t);
  void setTypeToUrl(bool t);

protected:
  void tag(const QString &tag);

  //virtual void mouseDoubleClickEvent(QMouseEvent *event);
  //virtual void contextMenuEvent(QContextMenuEvent *event);

private:
  QLineEdit *aliasEdit_;
  QToolButton *okButton_, *cancelButton_;

  QToolButton *isNameButton_, *isTagButton_, *isUrlButton_;

  QToolButton *isEnglishButton_,
              *isJapaneseButton_,
              *isChineseButton_,
              *isAlienButton_;
};

#endif // ADDALIASDIALOG_H
