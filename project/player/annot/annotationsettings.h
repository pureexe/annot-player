#ifndef ANNOTATIONSETTINGS_H
#define ANNOTATIONSETTINGS_H

// annotationsettings.h
// 5/24/2012

#include <QtGui/QFont>
#include <QtCore/QObject>

//#define ANNOTATION_FONT_FAMILY  "Geneva" // Helvetica on Mac
#define ANNOTATION_FONT_FAMILY  "Helvetica"
#ifdef Q_WS_WIN
#  define ANNOTATION_FONT_SIZE  18
#else
#  define ANNOTATION_FONT_SIZE  20
#endif // Q_WS_WIN

class AnnotationSettings : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationSettings)
  typedef AnnotationSettings Self;
  typedef QObject Base;

  qreal scale_;
  qreal rotation_;
  int offset_;
  QFont font_;

  bool avatarVisible_;

public:
  static Self *globalInstance() { static Self g; return &g; }
protected:
  explicit AnnotationSettings(QObject *parent = 0)
    : Base(parent), scale_(1), rotation_(0), offset_(0),
      avatarVisible_(false)
  { resetFont(); }

signals:
  void scaleChanged(qreal value);
  void rotationChanged(qreal value);
  void offsetChanged(int value);
  void avatarVisibleChanged(bool value);
  //void fontChanged(const QFont &value);
public:
  qreal scale() const { return scale_; }
  qreal rotation() const { return rotation_; }
  int offset() const { return offset_; }
  bool isAvatarVisible() const { return avatarVisible_; }

  QFont &font() { return font_; }
  QFont font() const { return font_; }
  QString fontFamily() const { return font_.family(); }

public slots:
  void setScale(qreal value) { if (!qFuzzyCompare(scale_, value)) emit scaleChanged(scale_ = value); }
  void resetScale() { setScale(0); }

  void setRotation(qreal value) { if (!qFuzzyCompare(rotation_, value)) emit rotationChanged(rotation_ = value); }
  void resetRotation() { setRotation(0); }

  void setOffset(int value) { if (offset_ != value) emit offsetChanged(offset_ = value); }
  void resetOffset() { setOffset(0); }

  void setFont(const QFont &value) { font_ = value; }
  void setFontFamily(const QString &family)
  {
    QString f = family.isEmpty() ? ANNOTATION_FONT_FAMILY : family;
    if (f.compare(font_.family(), Qt::CaseInsensitive))
      font_.setFamily(family);
  }

  void resetFontFamily() { setFontFamily(QString()); }

  void resetFont()
  {
    QFont font(ANNOTATION_FONT_FAMILY, ANNOTATION_FONT_SIZE);
    font.setWeight(QFont::DemiBold);
    font.setStyleStrategy((QFont::StyleStrategy)(
      QFont::PreferAntialias | QFont::PreferQuality
    ));
    setFont(font);
  }

  void setAvatarVisible(bool value)
  { if (avatarVisible_ != value) emit avatarVisibleChanged(avatarVisible_ = value); }
};

#endif // ANNOTATIONSETTINGS_H
