#ifndef USERVIEW_H
#define USERVIEW_H

// userview.h
// 8/3/2011

#include "core/gui/dialog.h"
#include "core/cloud/user.h"

QT_FORWARD_DECLARE_CLASS(QLabel)

typedef Core::Gui::Dialog UserViewBase;

class UserView : public UserViewBase
{
  Q_OBJECT
  typedef UserView Self;
  typedef UserViewBase Base;

  typedef Core::Cloud::User User;

public:
  explicit UserView(QWidget *parent = 0);

  // - Properties -
public:
  const User &user() const { return user_; }

  // - Slots -
public slots:
  void setUser(const User &user) { user_ = user; invalidateUser(); }
  void clearUser() { setUser(User()); }

protected slots:
  void invalidateUser();

private:
  void createLayout();

private:
  User user_;

  // TODO: add user Language, user status, etc as well
  QLabel *nameLabel_,
         *nicknameLabel_,
         *emailLabel_,
         *createTimeLabel_,
         *loginTimeLabel_,
         *annotCountLabel_,
         *blessedCountLabel_,
         *cursedCountLabel_,
         *blockedCountLabel_;
};

#endif // USERVIEW_H
