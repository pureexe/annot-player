// associationprefs.cc
// 5/25/2012

#include "associationprefs.h"
#include "global.h"
#include "tr.h"
#include "registry.h"
#include "module/player/player.h"
#include "project/common/acss.h"
#include "project/common/acui.h"
#include "module/qtext/namedcheckbox.h"
#include <QtGui>

// - Construction -

void
AssociationPreferencesTab::init()
{
  if (!registry_)
    registry_ = Registry::globalInstance();
  Q_ASSERT(registry_);

  supportedTypes_ = Player::supportedSuffices();
  supportedTypes_
    //G_FORMAT_PROGRAM(<<)
    G_FORMAT_ANNOTATION(<<)
    G_FORMAT_PREVIEW(<<)
  ;

  supportedShells_ G_FORMAT_DEVICE(<<);

  setWindowTitle(tr("Association"));
  createLayout();
}

void
AssociationPreferencesTab::createLayout()
{
  AcUi *ui = AcUi::globalInstance();
  QGridLayout //*gameLayout = new QGridLayout,
              *videoLayout = new QGridLayout,
              *audioLayout = new QGridLayout,
              *pictureLayout = new QGridLayout,
              *imageLayout = new QGridLayout,
              *subtitleLayout = new QGridLayout,
              *annotationLayout = new QGridLayout,
              *deviceLayout = new QGridLayout,
              *previewLayout = new QGridLayout;

  int
  count = 0;
  foreach (const QString &suffix, Player::supportedPlaylistSuffices()) {
    enum { Row = 7 };
    QtExt::NamedCheckBox *toggle = new QtExt::NamedCheckBox(suffix);
    toggle->setToolTip(suffix);
    toggle->setStyleSheet(ACSS_CHECKBOX);
    connect(toggle, SIGNAL(toggledWithName(bool,QString)), SLOT(toggleType(bool,QString)));
    toggles_[suffix] = toggle;
    int r = count / Row,
        c = count % Row;
    audioLayout->addWidget(toggle, r, c);
    count++;
  }
  foreach (const QString &suffix, Player::supportedAudioSuffices()) {
    enum { Row = 7 };
    QtExt::NamedCheckBox *toggle = new QtExt::NamedCheckBox(suffix);
    toggle->setToolTip(suffix);
    toggle->setStyleSheet(ACSS_CHECKBOX);
    connect(toggle, SIGNAL(toggledWithName(bool,QString)), SLOT(toggleType(bool,QString)));
    toggles_[suffix] = toggle;
    int r = count / Row,
        c = count % Row;
    audioLayout->addWidget(toggle, r, c);
    count++;
  }
  QGroupBox *audioGroup = ui->makeGroupBox(0, TR(T_FORMAT_AUDIO));
  audioGroup->setLayout(audioLayout);

  //count = 0;
  //foreach (const QString &suffix, QStringList() G_FORMAT_PROGRAM(<<)) {
  //  enum { Row = 7 };
  //  QtExt::NamedCheckBox *toggle = new QtExt::NamedCheckBox(suffix);
  //  toggle->setToolTip(suffix);
  //  toggle->setStyleSheet(ACSS_CHECKBOX);
  //  connect(toggle, SIGNAL(toggledWithName(bool,QString)), SLOT(toggleType(bool,QString)));
  //  toggles_[suffix] = toggle;
  //  int r = count / Row,
  //      c = count % Row;
  //  gameLayout->addWidget(toggle, r, c);
  //  count++;
  //}
  //QGroupBox *gameGroup = ui->makeGroupBox(0, TR(T_GALGAME));
  //gameGroup->setLayout(gameLayout);

  count = 0;
  foreach (const QString &suffix, QStringList() G_FORMAT_ANNOTATION(<<)) {
    enum { Row = 7 };
    QtExt::NamedCheckBox *toggle = new QtExt::NamedCheckBox(suffix);
    toggle->setToolTip(suffix);
    toggle->setStyleSheet(ACSS_CHECKBOX);
    connect(toggle, SIGNAL(toggledWithName(bool,QString)), SLOT(toggleType(bool,QString)));
    toggles_[suffix] = toggle;
    int r = count / Row,
        c = count % Row;
    annotationLayout->addWidget(toggle, r, c);
    count++;
  }
  QGroupBox *annotationGroup = ui->makeGroupBox(0, TR(T_FORMAT_ANNOTATION));
  annotationGroup->setLayout(annotationLayout);

  count = 0;
  foreach (const QString &suffix, QStringList() G_FORMAT_DEVICE(<<)) {
    enum { Row = 7 };
    QtExt::NamedCheckBox *toggle = new QtExt::NamedCheckBox(suffix);
    toggle->setToolTip(suffix);
    toggle->setStyleSheet(ACSS_CHECKBOX);
    connect(toggle, SIGNAL(toggledWithName(bool,QString)), SLOT(toggleShell(bool,QString)));
    toggles_[suffix] = toggle;
    int r = count / Row,
        c = count % Row;
    deviceLayout->addWidget(toggle, r, c);
    count++;
  }
  QGroupBox *deviceGroup = ui->makeGroupBox(0, TR(T_FORMAT_DEVICE));
  deviceGroup->setLayout(deviceLayout);

  count = 0;
  foreach (const QString &suffix, QStringList() G_FORMAT_PREVIEW(<<)) {
    enum { Row = 7 };
    QtExt::NamedCheckBox *toggle = new QtExt::NamedCheckBox(suffix);
    toggle->setToolTip(suffix);
    toggle->setStyleSheet(ACSS_CHECKBOX);
    connect(toggle, SIGNAL(toggledWithName(bool,QString)), SLOT(toggleShell(bool,QString)));
    toggles_[suffix] = toggle;
    int r = count / Row,
        c = count % Row;
    previewLayout->addWidget(toggle, r, c);
    count++;
  }
  QGroupBox *previewGroup = ui->makeGroupBox(0, TR(T_FORMAT_PREVIEW));
  previewGroup->setLayout(previewLayout);

#define REGISTER(_group, _Group, _GROUP) \
  count = 0; \
  foreach (const QString &suffix, Player::supported##_Group##Suffices()) { \
    enum { Row = 7 }; \
    QtExt::NamedCheckBox *toggle = new QtExt::NamedCheckBox(suffix); \
    toggle->setToolTip(suffix); \
    toggle->setStyleSheet(ACSS_CHECKBOX); \
    connect(toggle, SIGNAL(toggledWithName(bool,QString)), SLOT(toggleType(bool,QString))); \
    toggles_[suffix] = toggle; \
    int r = count / Row, \
        c = count % Row; \
    _group##Layout->addWidget(toggle, r, c); \
    count++; \
  } \
  QGroupBox *_group##Group = ui->makeGroupBox(0, TR(T_FORMAT_##_GROUP)); \
  _group##Group->setLayout(_group##Layout);

  REGISTER(video, Video, VIDEO)
  //REGISTER(audio, Audio, AUDIO)
  REGISTER(picture, Picture, PICTURE)
  REGISTER(image, Image, IMAGE)
  REGISTER(subtitle, Subtitle, SUBTITLE)
#undef REGISTER

  searchEdit_ = ui->makeLineEdit(0, QString(), tr("Search"), tr("Regular Expression"));
  connect(searchEdit_, SIGNAL(textChanged(QString)), SLOT(filterToggles(QString)));

  QVBoxLayout *rows = new QVBoxLayout; {
    rows->addWidget(searchEdit_);
    rows->addWidget(deviceGroup);
    //rows->addWidget(gameGroup);
    rows->addWidget(annotationGroup);
    rows->addWidget(subtitleGroup);
    rows->addWidget(imageGroup);
    rows->addWidget(videoGroup);
    rows->addWidget(audioGroup);
    rows->addWidget(pictureGroup);
    rows->addWidget(previewGroup);
  } setLayout(rows);
}

// - Save/Load -

bool
AssociationPreferencesTab::save()
{
  //foreach (const QString &suffix, supportedSuffices_) {
  //  QCheckBox *b = toggles_[suffix];
  //  Q_ASSERT(b);
  //  if (b)
  //    toggleType(b->isChecked(), suffix);
  //}
  registry_->sync();
  return true;
}

void
AssociationPreferencesTab::load()
{
  foreach (const QString &type, supportedTypes_) {
    QCheckBox *toggle = toggles_[type];
    Q_ASSERT(toggle);
    if (toggle)
      toggle->setChecked(registry_->containsType(type));
  }

  foreach (const QString &type, supportedShells_) {
    QCheckBox *toggle = toggles_[type];
    Q_ASSERT(toggle);
    if (toggle)
      toggle->setChecked(registry_->containsShell(type));
  }
}

void
AssociationPreferencesTab::clear()
{
  if (!searchEdit_->text().isEmpty()) {
    searchEdit_->clear();
    foreach (QCheckBox *toggle, toggles_)
      toggle->show();
  }
}

void
AssociationPreferencesTab::filterToggles(const QString &regexp)
{
  QString t = regexp.trimmed();
  if (t.isEmpty())
    foreach (QCheckBox *toggle, toggles_)
      toggle->show();
  else {
    QRegExp rx(t, Qt::CaseInsensitive);
    foreach (const QString &type, supportedTypes_) {
      QCheckBox *toggle = toggles_[type];
      Q_ASSERT(toggle);
      if (toggle)
        toggle->setVisible(type.contains(rx));
    }
    foreach (const QString &type, supportedShells_) {
      QCheckBox *toggle = toggles_[type];
      Q_ASSERT(toggle);
      if (toggle)
        toggle->setVisible(type.contains(rx));
    }
  }
}

void
AssociationPreferencesTab::toggleType(bool checked, const QString &type)
{
  Q_ASSERT(registry_);
  registry_->registerType(type, checked);
  registry_->sync();
  emit message(tr("file type saved") + ": " + type);
}

void
AssociationPreferencesTab::toggleShell(bool checked, const QString &type)
{
  Q_ASSERT(registry_);
  registry_->registerShell(type, checked);
  registry_->sync();
  emit message(tr("file type saved") + ": " + type);
}

// EOF
