#pragma once
#ifndef PICKER_H
#define PICKER_H

// picker.h
// 10/30/2011

#include "picker_config.h"
#include <QWidget>
#include <QRect>

class WindowPickerImpl; ///< \internal

///  This class should be used as a singleton.
class WindowPicker : public QObject
{
  Q_OBJECT
  typedef WindowPicker Self;
  typedef QObject Base;
  typedef WindowPickerImpl Impl;

  // - Singleton -
public:
  PICKERAPI static Self *globalInstance();

signals:
  void windowPicked(WId hwnd);

  // - Service -
public:
  PICKERAPI bool isActive() const;
  PICKERAPI bool isSingleShot() const;

public slots:
  PICKERAPI void start();       ///< Start getting the winId that the user clicked
  PICKERAPI void stop();        ///< Stop hook service
  PICKERAPI void singleShot();  ///< Pick one window and return

  PICKERAPI void setSingleShot(bool t); ///< Property setter

  // - Implementations -
protected:
  explicit WindowPicker(QObject *parent = 0);   ///< \internal
  ~WindowPicker();  ///< \internal

public:
  void *hook() const; ///< \internal Return global hook handle
  void pickWindow(WId hwnd);

private:
  Impl *impl_;
};

#endif // PICKER_H