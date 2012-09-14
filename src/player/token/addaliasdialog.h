#ifndef ADDALIASDIALOG_H
#define ADDALIASDIALOG_H

// addaliasdialog.h
// 10/13/2011

#include "src/common/acwindow.h"

QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QToolButton)

class SignalHub;
class AddAliasDialog : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(AddAliasDialog)
  typedef AddAliasDialog Self;
  typedef AcWindow Base;

public:
  explicit AddAliasDialog(SignalHub *hub, QWidget *parent = nullptr);

private:
  void createLayout();

signals:
  void aliasAdded(const QString &alias, int type, qint32 language);

  // - Properties -
public:
  //quint32 languageFlags() const;

  // - Actions -
public slots:
  void setVisible(bool visible) override;
  void refresh();
protected slots:
  void ok();
  void paste();
  void verifyEditText();

  //void tag01()          { tag("01"); }
  //void tag02()          { tag("02"); }
  //void tag03()          { tag("03"); }
  //void tag04()          { tag("04"); }
  //void tag05()          { tag("05"); }
  //void tagOVA()         { tag("OVA"); }
  //void tagOAD()         { tag("OAD"); }

  //void tagBD()          { tag("BD"); }
  //void tagDVD()         { tag("DVD"); }
  //void tagTV()          { tag("TV"); }
  //void tagWeb()         { tag("Web"); }

  void updateOKButton();
  void updateUrlButton();

  void setTypeToName(bool t = true);
  //void setTypeToTag(bool t);
  void setTypeToUrl(bool t = true);

protected:
  void tag(const QString &tag);
  //void postfix(const QString &tag);

  //void mouseDoubleClickEvent(QMouseEvent *event) override;
  //void contextMenuEvent(QContextMenuEvent *event) override;

private:
  SignalHub *hub_;
  QComboBox *edit_;
  QString editStyleSheet_;
  QToolButton *okButton_, *cancelButton_;

  QToolButton *isNameButton_,
              //*isTagButton_,
              *isUrlButton_;

  //QToolButton *isEnglishButton_,
  //            *isJapaneseButton_,
  //            *isChineseButton_,
  //            *isAlienButton_;
};

#endif // ADDALIASDIALOG_H
