#ifndef USERVIEW_H
#define USERVIEW_H

// userview.h
// 8/3/2011

#include "project/common/acwindow.h"
#include "module/annotcloud/user.h"

QT_FORWARD_DECLARE_CLASS(QLabel)

class UserView : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(UserView)
  typedef UserView Self;
  typedef AcWindow Base;

  typedef AnnotCloud::User User;

public:
  explicit UserView(QWidget *parent = 0);

  // - Properties -
public:
  const User &user() const { return user_; }

  // - Slots -
public slots:
  void setUser(const User &user) { user_ = user; updateUser(); }
  void clearUser() { setUser(User()); }

protected slots:
  void updateUser();

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
