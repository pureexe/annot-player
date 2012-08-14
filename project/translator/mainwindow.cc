// mainwindow.cc
// 8/13/2012

#include "mainwindow.h"
#include "translatebrowser.h"
#include "translateedit.h"
#include "settings.h"
#include "trayicon.h"
#include "global.h"
#include "rc.h"
#include "project/common/acabout.h"
#include "project/common/acpreferences.h"
#include "project/common/acui.h"
#include "project/common/acrc.h"
#include "project/common/acss.h"
#include "project/common/acbrowser.h"
#include "project/common/actranslator.h"
#include "module/qtext/htmltag.h"
#include "module/qtext/ss.h"
#include "module/qtext/layoutwidget.h"
#include "module/animation/fadeanimation.h"
#include "module/translator/translatormanager.h"
#include "module/translator/jdictranslator.h"
#include <QtGui>

#define DEBUG "mainwindow"
#include "module/debug/debug.h"

#define DICT_CSS \
  HTML_CSS_OPEN() \
    SS_BEGIN(div) \
      SS_MARGIN(1em 0.5em 1em 0.5em) \
      SS_BACKGROUND_COLOR(rgba(173,216,230,64)) \
    SS_END \
    SS_BEGIN(label) \
      SS_BACKGROUND_COLOR(rgba(144,238,144,64)) \
    SS_END \
  HTML_CSS_CLOSE()

// - Construction -

MainWindow::MainWindow(QWidget *parent)
  : Base(parent, Qt::WindowStaysOnTopHint), disposed_(false)
{
  setWindowTitle(tr("Annot Translator"));
  setWindowIcon(QIcon(RC_IMAGE_APP));

  createSearchEngines();
  createLayout();
  createActions();

  // - Translators -

  textTranslator_ = new TranslatorManager(this);
  connect(textTranslator_, SIGNAL(translatedByGoogle(QString)), SLOT(showGoogleTranslation(QString)));
  connect(textTranslator_, SIGNAL(translatedByMicrosoft(QString)), SLOT(showMicrosoftTranslation(QString)));
  connect(textTranslator_, SIGNAL(translatedByRomaji(QString)), SLOT(showRomajiTranslation(QString)));

  dictTranslator_ = new JdicTranslator(this);
  connect(dictTranslator_, SIGNAL(translated(QString)), SLOT(showDictTranslation(QString)));

  // - IPC -
  browserDelegate_ = new AcBrowser(this);

  appServer_ = new AcTranslatorServer(this);
  connect(appServer_, SIGNAL(showRequested()), SLOT(show()), Qt::QueuedConnection);
  connect(appServer_, SIGNAL(arguments(QStringList)), SLOT(showTranslate(QStringList)), Qt::QueuedConnection);
  appServer_->start();

  if (QSystemTrayIcon::isSystemTrayAvailable()) {
    trayIcon_ = new TrayIcon(this, this);
    trayIcon_->show();
  }

  // - Clipboard -
  connect(QApplication::clipboard(), SIGNAL(dataChanged()), SLOT(translateClipboard()));

  // - Post status -

  jdicButton_->setChecked(true);
  romajiButton_->setChecked(true);
  microsoftButton_->setChecked(true);

  autoButton_->setChecked(true);
  topButton_->setChecked(true);
  //clipboardButton_->setChecked(true);

  updateLanguage();
  updateTranslators();
  translateEdit_->setFocus();
}

void
MainWindow::createSearchEngines()
{
  enum { LastEngine = SearchEngineFactory::WikiZh };
  for (int i = 0; i <= LastEngine; i++) {
    SearchEngine *e = SearchEngineFactory::globalInstance()->create(i);
    e->setParent(this);
    searchEngines_.append(e);
  }
}

// searchbutton [ language ] |auto| |clipboard|
// textedit
// browser x 3
void
MainWindow::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  translateButton_ = ui->makeToolButton(AcUi::PushHint | AcUi::HighlightHint, tr("Translate"), this, SLOT(translate()));

  autoButton_ = ui->makeToolButton(AcUi::CheckHint, tr("Auto"), tr("Automatically translate selected text"));
  clipboardButton_ = ui->makeToolButton(AcUi::CheckHint, tr("Clipboard"), tr("Monitor and translate clipboard text"));
  topButton_ = ui->makeToolButton(AcUi::CheckHint, tr("Top"), tr("Window stays on top"), this, SLOT(setWindowOnTop(bool)));

  languageCombo_ = ui->makeComboBox(AcUi::ReadOnlyHint, QString(), tr("Language")); {
    languageCombo_->addItem(QIcon(ACRC_IMAGE_ENGLISH), tr("English"));
    languageCombo_->addItem(QIcon(ACRC_IMAGE_TRADITIONAL_CHINESE), tr("Chinese")); // Chinese = Traditional Chinese, fack ccp
    languageCombo_->addItem(QIcon(ACRC_IMAGE_SIMPLIFIED_CHINESE), tr("Simplified Chinese"));
    languageCombo_->addItem(QIcon(ACRC_IMAGE_KOREAN), tr("Korean"));
    languageCombo_->addItem(QIcon(ACRC_IMAGE_FRENCH), tr("French"));
    languageCombo_->addItem(QIcon(ACRC_IMAGE_GERMAN), tr("German"));
    languageCombo_->addItem(QIcon(ACRC_IMAGE_ITALIAN), tr("Italian"));
    languageCombo_->addItem(QIcon(ACRC_IMAGE_SPANISH), tr("Spanish"));
    languageCombo_->addItem(QIcon(ACRC_IMAGE_PORTUGUESE), tr("Portuguese"));
    languageCombo_->addItem(QIcon(ACRC_IMAGE_RUSSIAN), tr("Russian"));
    languageCombo_->addItem(QIcon(ACRC_IMAGE_JAPANESE), tr("Japanese"));
  }
  connect(languageCombo_, SIGNAL(activated(int)), SLOT(updateLanguage()));

  jdicButton_ = ui->makeCheckBox(0, tr("WWWJDIC"), tr("WWWJDIC Translator"), this, SLOT(updateTranslators()));
  romajiButton_ = ui->makeCheckBox(0, tr("Romaji"), tr("Romaji Translator"), this, SLOT(updateTranslators()));
  microsoftButton_ = ui->makeCheckBox(0, tr("Microsoft"), tr("Microsoft Translator"), this, SLOT(updateTranslators()));
  googleButton_ = ui->makeCheckBox(0, tr("Google"), tr("Google Translator"), this, SLOT(updateTranslators()));

  translateEdit_ = new TranslateEdit;
  connect(translateEdit_, SIGNAL(textChanged()), SLOT(autoTranslate()));
  connect(translateEdit_, SIGNAL(selectedTextChanged(QString)), SLOT(autoTranslate(QString)));

  for (size_t i = 0; i < sizeof(browsers_)/sizeof(*browsers_); i++) {
    QTextBrowser *b = browsers_[i] = new TranslateBrowser;
    connect(b, SIGNAL(anchorClicked(QUrl)), SLOT(openUrl(QUrl)));
    switch (i) {
    case DictBrowser0:
    case DictBrowser1:
    case DictBrowser2:
      connect(b, SIGNAL(selectedTextChanged(QString)), SLOT(autoTranslate(QString)));
    }
  }

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout;
    rows->addLayout(header);

    header->addWidget(translateButton_);
    header->addWidget(languageCombo_);
    header->addWidget(autoButton_);
    header->addWidget(clipboardButton_);
    header->addWidget(topButton_);
    header->addStretch();
    header->addWidget(jdicButton_);
    header->addWidget(romajiButton_);
    header->addWidget(microsoftButton_);
    header->addWidget(googleButton_);

    QSplitter *h = new QSplitter(Qt::Horizontal),
              *left = new QSplitter(Qt::Vertical),
              *right = new QSplitter(Qt::Vertical);
    rows->addWidget(h);
    h->addWidget(left);
    h->addWidget(right);

    left->addWidget(translateEdit_);
    left->addWidget(browsers_[DictBrowser0]);
    left->setStretchFactor(0, 1);
    left->setStretchFactor(1, 100); // push the first item to its minimum height

    right->addWidget(browsers_[TextBrowser]);
    right->addWidget(browsers_[DictBrowser1]);
    right->addWidget(browsers_[DictBrowser2]);
    right->addWidget(browsers_[DictBrowser3]);

    // left, top, right, bottom
    int patch = 0;
    if (!AcUi::isAeroAvailable())
      patch = 9;
    header->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(patch, patch, patch, patch);
    setContentsMargins(0, 0, 0, 0);
  } setCentralWidget(new LayoutWidget(rows, this));

  //QDockWidget *dock = new QDockWidget(tr("Dictionary"), this);
  //addDockWidget(Qt::BottomDockWidgetArea, dock);
  //dock->setWidget(browsers_[TextBrowser]);
}

void
MainWindow::createActions()
{
  menuBar_ =
#ifdef Q_OS_MAC
    new QMenuBar // global, no parent, see: http://qt-project.org/doc/qt-4.8/qmenubar.html#qmenubar-on-mac-os-x
#else
    menuBar()
#endif // Q_OS_MAC
  ;

  // Copy menu
  QMenu *m;
  //m = copyMenu_ = new QMenu(tr("Copy") + " ...", this); {
  //  m->addSeparator();
//
  //  QString t = tr("Search with %1");
//
  //  SearchEngine *e = searchEngines_[SearchEngineFactory::Google];
  //  m->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithGoogle()));
//
  //  e = searchEngines_[SearchEngineFactory::GoogleImages];
  //  m->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithGoogleImages()));
//
  //  e = searchEngines_[SearchEngineFactory::Bing];
  //  m->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithBing()));
//
  //  m->addSeparator();
//
  //  e = searchEngines_[SearchEngineFactory::Nicovideo];
  //  m->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithNicovideo()));
//
  //  e = searchEngines_[SearchEngineFactory::Bilibili];
  //  m->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithBilibili()));
//
  //  e = searchEngines_[SearchEngineFactory::Acfun];
  //  m->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithAcfun()));
//
  //  e = searchEngines_[SearchEngineFactory::Youtube];
  //  m->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithYoutube()));
//
  //  e = searchEngines_[SearchEngineFactory::Youku];
  //  m->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithYouku()));
//
  //  m->addSeparator();
//
  //  e = searchEngines_[SearchEngineFactory::WikiEn];
  //  m->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithWikiEn()));
//
  //  e = searchEngines_[SearchEngineFactory::WikiJa];
  //  m->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithWikiJa()));
//
  //  e = searchEngines_[SearchEngineFactory::WikiZh];
  //  m->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithWikiZh()));
//
  //}

  // Create menus
  m = contextMenu_ = new QMenu(this); {
    AcUi::globalInstance()->setContextMenuStyle(m, true); // persistent = true
  //  m->addMenu(copyMenu_);
  //  m->addSeparator();
  //  m->addAction(startAct_);
  //  m->addAction(openDirectoryAct_);
  //  m->addAction(playAct_);
  //  m->addAction(browseAct_);
  //  m->addSeparator();
  //  m->addAction(stopAct_);
  //  m->addAction(restartAct_);
  //  m->addAction(removeAct_);
  //  m->addSeparator();
  //  m->addAction(startAllAct_);
  //  m->addAction(stopAllAct_);
  //  m->addAction(removeAllAct_);
  //  m->addSeparator();
  //#ifndef Q_OS_MAC
  //  m->addAction(menuBarAct_);
  //  m->addAction(tr("Preferences"), this, SLOT(preferences()), QKeySequence("CTRL+,"));
  //#endif // Q_OS_MAC
    m->addAction(tr("Hide"), this, SLOT(fadeOut()));
    m->addAction(tr("Quit"), this, SLOT(quit()));
  }

  // Create menu bar

#ifdef Q_OS_WIN
  menuBar_->hide();
#endif // Q_OS_WIN
  m = menuBar_->addMenu(tr("&File")); {
  //  m->addAction(newAct_);
  //  m->addSeparator();
  //  m->addAction(stopAllAct_);
  //  m->addAction(removeAllAct_);
  //  m->addSeparator();
  //  m->addAction(playAct_);
  //  m->addAction(browseAct_);
  //  m->addAction(openDirectoryAct_);
  //  m->addSeparator();
    m->addAction(tr("&Quit"), this, SLOT(quit()), QKeySequence::Quit);
  }
  //m = menuBar_->addMenu(tr("&Edit")); {
  //  m->addAction(startAct_);
  //  m->addSeparator();
  //  m->addAction(stopAct_);
  //  m->addAction(restartAct_);
  //  m->addAction(removeAct_);
  //}
  m = menuBar_->addMenu(tr("&Settings")); {
    m->addAction(tr("&Preferences"), this, SLOT(showPreferences()), QKeySequence("CTRL+,"));
  }
  m = menuBar_->addMenu(tr("&Help")); {
    //helpMenu_->addAction(tr("&Help"), this, SLOT(help())); // DO NOT TRANSLATE ME
    m->addAction(tr("&About"), this, SLOT(showAbout())); // DO NOT TRANSLATE ME
  }

#ifdef Q_OS_WIN
  new QShortcut(QKeySequence("ALT+O"), this, SLOT(showPreferences()));
#endif // Q_OS_WIN
}

// - Properties -

// See: http://msdn.microsoft.com/en-us/library/hh456380.aspx
// Must be consistent with Translator::languageCode
void
MainWindow::updateLanguage()
{
  switch (languageCombo_->currentIndex()) {
  case English: languageCode_ = "en"; break;
  case TraditionalChinese: languageCode_  = "zh-CHT"; break;
  case SimplifiedChinese: languageCode_  = "zh-CHS"; break;
  case Korean: languageCode_ = "ko"; break;
  case French: languageCode_ = "fr"; break;
  case German: languageCode_ = "de"; break;
  case Italian: languageCode_ = "it"; break;
  case Spanish: languageCode_ = "es"; break;
  case Portuguese: languageCode_ = "pt"; break;
  case Russian: languageCode_ = "ru"; break;
  case Japanese: languageCode_ = "ja"; break;

  default: Q_ASSERT(0); languageCode_ = "en";
  }
}

void
MainWindow::updateTranslators()
{
  dictTranslator_->setEnabled(jdicButton_->isChecked());

  textTranslator_->setService(TranslatorManager::Google, googleButton_->isChecked());
  textTranslator_->setService(TranslatorManager::Microsoft, microsoftButton_->isChecked());
  textTranslator_->setService(TranslatorManager::Romaji, romajiButton_->isChecked());

  connect(dictTranslator_, SIGNAL(errorMessage(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(textTranslator_, SIGNAL(errorMessage(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
}

// - Events -

void
MainWindow::setVisible(bool visible)
{
  if (visible)
    refresh();
  Base::setVisible(visible);
}

void
MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
  Q_ASSERT(event);
  //updateActions();
  //if (copyMenu_->isEnabled())
  //  updateCopyMenu();
  contextMenu_->exec(event->globalPos());
  event->accept();
}

void
MainWindow::closeEvent(QCloseEvent *event)
{
  enum { CloseTimeout = 3000 };
  DOUT("enter");
  //if (!disposed_ && downloadManager_->isRunning()) {
  //  event->ignore();
  //  fadeOut();
  //  trayIcon_->showMessage(tr("Annot Translator"), tr("Minimize to Tray"));
  //  DOUT("exit: not disposed");
  //  return;
  //}
  disposed_ = true;
  appServer_->stop();

  hide();
  if (trayIcon_)
    trayIcon_->hide();

  saveSettings();

  if (QThreadPool::globalInstance()->activeThreadCount())
    // wait for at most 2 seconds ant kill all threads
    QThreadPool::globalInstance()->waitForDone(CloseTimeout);

#ifdef Q_OS_WIN
  QTimer::singleShot(0, qApp, SLOT(quit())); // ensure quit app and clean up zombie threads
#endif // Q_OS_WIN

  Base::closeEvent(event);
  DOUT("exit");
}

void
MainWindow::keyPressEvent(QKeyEvent *e)
{
  switch (e->key()) {
  case Qt::Key_CapsLock:
#ifdef Q_OS_WIN
    menuBar_->setVisible(!menuBar_->isVisible());
#endif // Q_OS_WIN
    break;
  default: ;
  }
  Base::keyReleaseEvent(e);
}

// - Actions -

void
MainWindow::translateClipboard()
{
  if (!clipboardButton_->isChecked())
    return;
  QString text = QApplication::clipboard()->text().trimmed();
  if (!text.isEmpty())
    showTranslate(text);
}

void
MainWindow::showTranslate(const QString &text)
{
  translateEdit_->setPlainText(text);
  translate(text);
}

void
MainWindow::showTranslate(const QStringList &l)
{
  switch (l.size()) {
  case 0: break;
  case 1: showTranslate(l.first()); break;
  default: showTranslate(l.join(" "));
  }
}

void
MainWindow::autoTranslate()
{ if (autoButton_->isChecked()) translate(); }

void
MainWindow::autoTranslate(const QString &text)
{ if (autoButton_->isChecked()) translate(text); }

void
MainWindow::translate()
{ translate(translateEdit_->toPlainText()); }

void
MainWindow::translate(const QString &input)
{
  if (input.isEmpty())
    return;
  QString t = input.trimmed();
  if (t.isEmpty())
    return;
  //textTranslator_->translate(t, language_); // FIXME: inlining is not working under clang T_T
  for (int service = 0; service < TranslatorManager::ServiceCount; service++)
    if (textTranslator_->hasService(service))
      textTranslator_->translate(service, t, languageCode_);

  showTextTranslation(HTML_SS_OPEN(color:gray) "--------" HTML_SS_CLOSE());
  dictTranslator_->translate(t, languageCode_);
}

void
MainWindow::showAbout()
{
  static QWidget *w = 0;
  if (!w)
    w = new AcAbout(G_APPLICATION, G_VERSION, this);
  w->show();
  w->raise();
}

void
MainWindow::refresh()
{
}

void
MainWindow::showPreferences()
{
  static QWidget *w = 0;
  if (!w)
    w = new AcPreferences(AcPreferences::NetworkProxyTab, this);
  w->show();
  w->raise();
}

void
MainWindow::quit()
{
  disposed_ = true;
  fadeOut();
  QTimer::singleShot(fadeAnimation()->duration(), this, SLOT(close()));
}

void
MainWindow::searchWithEngine(int engine, const QString &key)
{
  if (engine >= 0 && engine < searchEngines_.size() && !key.isEmpty()) {
    const SearchEngine *e = searchEngines_[engine];
    if (e) {
      QString url = e->search(key);
      if (!url.isEmpty())
        openUrl(QUrl(url));
    }
  }
}

void
MainWindow::saveSettings()
{
  Settings *settings = Settings::globalSettings();
  settings->setRecentSize(size());
  settings->sync();
}

// - Show Translations -

void
MainWindow::showGoogleTranslation(const QString &text)
{
  showTextTranslation(
    HTML_SS_OPEN(color:orange) + text + HTML_SS_CLOSE()
  );
}

void
MainWindow::showMicrosoftTranslation(const QString &text)
{
  showTextTranslation(
    HTML_SS_OPEN(color:blue) + text + HTML_SS_CLOSE()
  );
}

void
MainWindow::showRomajiTranslation(const QString &text)
{
  showTextTranslation(
    HTML_SS_OPEN(color:purple) + text + HTML_SS_CLOSE()
  );
}

void
MainWindow::showDictTranslation(const QString &text)
{
  int j = -1;
  for (int i = DictBrowser0; i <= DictBrowser3; i++)
    if (browsers_[i]->hasFocus())
      j = i+1;
  j = qBound<int>(DictBrowser0, j, DictBrowser3);
  browsers_[j]->setHtml(DICT_CSS + text);
}

void
MainWindow::showTextTranslation(const QString &text)
{ browsers_[TextBrowser]->append(text); }

void
MainWindow::openUrl(const QUrl &url)
{
  QString t = url.toString();
  showMessage(tr("opening") + ": " + t);
  browserDelegate_->openUrl(t);
}

// EOF
