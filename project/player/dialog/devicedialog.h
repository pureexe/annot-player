#ifndef DEVICEDIALOG_H
#define DEVICEDIALOG_H

// devicedialog.h
// 12/2/2011

#include "module/qtext/dialog.h"
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE
class QToolButton;
class QComboBox;
class QRadioButton;
QT_END_NAMESPACE

typedef QtExt::Dialog DeviceDialogBase;
class DeviceDialog : public DeviceDialogBase
{
  Q_OBJECT
  typedef DeviceDialog Self;
  typedef DeviceDialogBase Base;

public:
  explicit DeviceDialog(QWidget *parent = 0);

signals:
  void deviceSelected(const QString &path, bool isAudioCD);

  // - Helpers -
public:
  static QStringList devices(); // TODO: move to somewhere else

  // - Properties -
public:
  QString currentPath() const;
  //void setMessage(const QString &text);

  // - Events -
public:
  virtual void setVisible(bool visible); ///< \override

  // - Slots -
public slots:
  void refresh();
protected slots:
  void ok();

  void invalidateButtons();
  void invalidateComboBox();

private:
  void createLayout();

private:
  QToolButton *okButton_;
  QComboBox *pathComboBox_; // device path
  QRadioButton *autoRadioButton_, // device type
               *dvdRadioButton_,
               *cdRadioButton_;
};

#endif // DEVICEDIALOG_H
