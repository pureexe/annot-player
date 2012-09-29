#ifndef SKGLOBAL_H
#define SKGLOBAL_H

// skglobal.h
// 9/23/2012 jichi

// Redefine SK_BEGIN_NAMESPACE/SK_END_NAMESPACE to turn on custom namespace
#ifndef SK_BEGIN_NAMESPACE
# define SK_BEGIN_NAMESPACE
#endif // SK_BEGIN_NAMESPACE
#ifndef SK_END_NAMESPACE
# define SK_END_NAMESPACE
#endif // SK_END_NAMESPACE

#define SK_FORWARD_DECLARE_CLASS(name)     SK_BEGIN_NAMESPACE class name;  SK_END_NAMESPACE
#define SK_FORWARD_DECLARE_STRUCT(name)    SK_BEGIN_NAMESPACE struct name; SK_END_NAMESPACE

SK_BEGIN_NAMESPACE
namespace Sk {

} // Sk
SK_END_NAMESPACE

#define SK_OFFSCREEN_POS   QPoint(-1, -1)
#define SK_OFFSCREEN_POSF  QPointF(-1, -1)

// - GUI -

#define SK_STYLE_CLASS(_class) \
    Q_PROPERTY(QString class READ styleClass) \
  public: \
    QString styleClass() const { return #_class; } \
  private:

#define SK_SYNTHESIZE_STYLE_CLASS \
    Q_PROPERTY(QString class READ styleClass WRITE setStyleClass) \
    QString styleClass_; \
  public: \
    QString styleClass() const { return styleClass_; } \
  public slots: \
    void seStyleClass(const QString &value) { styleClass_ = value; } \
  private:

#endif // SKGLOBAL_H
