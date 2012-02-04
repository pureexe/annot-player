#ifndef USERVIEW_H
#define USERVIEW_H

// userview.h
// 8/3/2011

#include "module/qtext/dialog.h"
#include "module/annotcloud/user.h"

QT_FORWARD_DECLARE_CLASS(QLabel)

typedef QtExt::Dialog UserViewBase;

class UserView : public UserViewBase
{
  Q_OBJECT
  typedef UserView Self;
  typedef UserViewBase Base;

  typedef AnnotCloud::User User;

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
