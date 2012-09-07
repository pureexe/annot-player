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
#include "project/common/acsettings.h"
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
#include "module/translator/kotobanktranslator.h"
#ifdef WITH_MODULE_MECABHIGHLIGHTER
# include "module/mecabhighlighter/mecabhighlighter.h"
#endif // WITH_MODULE_MECABHIGHLIGHTER
#ifdef WITH_WIN_ATLAS
# include "win/atlas/atlas.h"
#endif // WITH_WIN_ATLAS
#include <QtGui>
#include <boost/foreach.hpp>

#define DEBUG "mainwindow"
#include "module/debug/debug.h"

enum { DEFAULT_TRANSLATORS = TranslatorManager::RomajiBit | TranslatorManager::OcnBit | TranslatorManager::FresheyeBit };

#define JDIC_CSS \
  HTML_CSS_OPEN() \
    SS_BEGIN(div) \
      SS_MARGIN(1em 0.5em 1em 0.5em) \
      SS_BACKGROUND_COLOR(rgba(173,216,230,64)) \
    SS_END \
    SS_BEGIN(label) \
      SS_BACKGROUND_COLOR(rgba(144,238,144,64)) \
    SS_END \
  HTML_CSS_CLOSE()

#define KOTOBANK_CSS \
  HTML_CSS_OPEN() \
    SS_BEGIN(dt) \
      SS_MARGIN(1em 0.5em 0 0.5em) \
      SS_BACKGROUND_COLOR(rgba(144,238,144,64)) \
      SS_FONT_WEIGHT(bold) \
    SS_END \
    SS_BEGIN(dd) \
      SS_MARGIN(0 0.5em 0 0.5em) \
      SS_BACKGROUND_COLOR(rgba(173,216,230,64)) \
    SS_END \
  HTML_CSS_CLOSE()

//#ifdef Q_OS_MAC
//# define K_CTRL     "cmd"
//# define K_ENTER    "enter"
//#else
//# define K_ENTER    "Enter"
//# define K_CTRL     "Ctrl"
//#endif // Q_OS_MAC

// - Construction -

#define WINDOW_FLAGS \
   Settings::globalSettings()->windowOnTop() ? \
     Qt::WindowStaysOnTopHint : \
     Qt::WindowType(0)

MainWindow::MainWindow(QWidget *parent)
  : Base(parent, WINDOW_FLAGS), language_(0), disposed_(false), translatedTextChanged_(true)
{
  //if (Settings::globalSettings()->windowOnTop())
  //  setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
  setWindowTitle(tr("Annot Translator"));
  setWindowIcon(QIcon(RC_IMAGE_APP));

  //createSearchEngines();
  createLayout();
  createActions();
#ifdef WITH_WIN_ATLAS
  createAtlas();
#endif // WITH_WIN_ATLAS

  // - Translators -

  textTranslator_ = new TranslatorManager(this);
  connect(textTranslator_, SIGNAL(translated(QString)), SLOT(invalidateTranslatedText()));
  connect(textTranslator_, SIGNAL(translatedByRomaji(QString)), SLOT(showRomajiTranslation(QString)));
  connect(textTranslator_, SIGNAL(translatedByMicrosoft(QString)), SLOT(showMicrosoftTranslation(QString)));
  connect(textTranslator_, SIGNAL(translatedByGoogle(QString)), SLOT(showGoogleTranslation(QString)));
  connect(textTranslator_, SIGNAL(translatedByOcn(QString)), SLOT(showOcnTranslation(QString)));
  connect(textTranslator_, SIGNAL(translatedByExcite(QString)), SLOT(showExciteTranslation(QString)));
  connect(textTranslator_, SIGNAL(translatedBySdl(QString)), SLOT(showSdlTranslation(QString)));
  connect(textTranslator_, SIGNAL(translatedBySystran(QString)), SLOT(showSystranTranslation(QString)));
  connect(textTranslator_, SIGNAL(translatedByNifty(QString)), SLOT(showNiftyTranslation(QString)));
  connect(textTranslator_, SIGNAL(translatedByInfoseek(QString)), SLOT(showInfoseekTranslation(QString)));
  connect(textTranslator_, SIGNAL(translatedByYahoo(QString)), SLOT(showYahooTranslation(QString)));
  connect(textTranslator_, SIGNAL(translatedByFresheye(QString)), SLOT(showFresheyeTranslation(QString)));

  kotobankTranslator_ = new KotobankTranslator(this);
  connect(kotobankTranslator_, SIGNAL(translated(QString)), SLOT(showKotobankTranslation(QString)));

  jdicTranslator_ = new JdicTranslator(this);
  connect(jdicTranslator_, SIGNAL(translated(QString)), SLOT(showJdicTranslation(QString)));

  connect(kotobankTranslator_, SIGNAL(errorMessage(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(jdicTranslator_, SIGNAL(errorMessage(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(textTranslator_, SIGNAL(errorMessage(QString)), SLOT(warn(QString)), Qt::QueuedConnection);

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

  (Settings::globalSettings()->dictionary() == Kotobank ? kotobankButton_ : jdicButton_)
    ->setChecked(true);

  ulong t = Settings::globalSettings()->translationServices();
  if (!t)
    t = DEFAULT_TRANSLATORS;
  romajiButton_->setChecked(t & TranslatorManager::RomajiBit);
  microsoftButton_->setChecked(t & TranslatorManager::MicrosoftBit);
  googleButton_->setChecked(t & TranslatorManager::GoogleBit);
  ocnButton_->setChecked(t & TranslatorManager::OcnBit);
  exciteButton_->setChecked(t & TranslatorManager::ExciteBit);
  sdlButton_->setChecked(t & TranslatorManager::SdlBit);
  systranButton_->setChecked(t & TranslatorManager::SystranBit);
  niftyButton_->setChecked(t & TranslatorManager::NiftyBit);
  infoseekButton_->setChecked(t & TranslatorManager::InfoseekBit);
  yahooButton_->setChecked(t & TranslatorManager::YahooBit);
  fresheyeButton_->setChecked(t & TranslatorManager::FresheyeBit);
#ifdef WITH_WIN_ATLAS
  atlasButton_->setChecked(Settings::globalSettings()->isAtlasEnabled());
#endif // WITH_WIN_ATLAS

  autoButton_->setChecked(true);
  topButton_->setChecked(isWindowOnTop());
  //clipboardButton_->setChecked(true);

  updateLanguage();
  updateTranslators();
  translateEdit_->setFocus();
}

//void
//MainWindow::createSearchEngines()
//{
//  enum { LastEngine = SearchEngineFactory::WikiZh };
//  for (int i = 0; i <= LastEngine; i++) {
//    SearchEngine *e = SearchEngineFactory::globalInstance()->create(i);
//    e->setParent(this);
//    searchEngines_.append(e);
//  }
//}

// searchbutton [ language ] |auto| |clipboard|
// textedit
// browser x 3
void
MainWindow::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  translateButton_ = ui->makeToolButton(AcUi::PushHint | AcUi::HighlightHint, tr("Translate"), this, SLOT(translate()));
  //new QShortcut(QKeySequence("CTRL+S"), this, SLOT(translate()));

  autoButton_ = ui->makeToolButton(AcUi::CheckHint, tr("Auto"), tr("Automatically translate selected text"));
  clipboardButton_ = ui->makeToolButton(AcUi::CheckHint, tr("Clipboard"), tr("Monitor and translate clipboard text"), this, SLOT(translateClipboard()));
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
  int lang = Settings::globalSettings()->languageIndex();
  lang = qBound(0, lang, languageCombo_->count());
  languageCombo_->setCurrentIndex(lang);
  connect(languageCombo_, SIGNAL(activated(int)), SLOT(updateLanguage()));
  connect(languageCombo_, SIGNAL(activated(int)), SLOT(translate()));

  kotobankButton_ = ui->makeRadioButton(0, tr("Kotobank"), tr("Kotobank Translator") + " (ja)", this, SLOT(updateTranslators()));
  jdicButton_ = ui->makeRadioButton(0, tr("WWWJDIC"), tr("WWWJDIC Translator") + " (en)", this, SLOT(updateTranslators()));
  romajiButton_ = ui->makeCheckBox(0, tr("Romaji"), tr("Romaji Translator"), this, SLOT(updateTranslators()));
  microsoftButton_ = ui->makeCheckBox(0, tr("Microsoft"), tr("Microsoft Translator"), this, SLOT(updateTranslators()));
  googleButton_ = ui->makeCheckBox(0, tr("Google"), tr("Google Translator"), this, SLOT(updateTranslators()));
  infoseekButton_ = ui->makeCheckBox(0, tr("Infoseek"), tr("Infoseek Honyaku"), this, SLOT(updateTranslators()));
  yahooButton_ = ui->makeCheckBox(0, tr("Yahoo!"), tr("Yahoo! Honyaku"), this, SLOT(updateTranslators()));
  fresheyeButton_ = ui->makeCheckBox(0, tr("freshEYE"), tr("freshEYE Honyaku") + " (en,zh)", this, SLOT(updateTranslators()));
  ocnButton_ = ui->makeCheckBox(0, tr("OCN"), tr("OCN Honyaku") + " (en,zh,ko)", this, SLOT(updateTranslators()));
  exciteButton_ = ui->makeCheckBox(0, tr("Excite"), tr("Excite Honyaku") + " (en)", this, SLOT(updateTranslators()));
  sdlButton_ = ui->makeCheckBox(0, tr("SDL"), tr("SDL Translator") + " (en)", this, SLOT(updateTranslators()));
  systranButton_ = ui->makeCheckBox(0, tr("SYSTRAN"), tr("SYSTRAN Translator") + " (en)", this, SLOT(updateTranslators()));
  niftyButton_ = ui->makeCheckBox(0, tr("@nifty"), tr("@nifty honyaku") + " (en)", this, SLOT(updateTranslators()));
#ifdef WITH_WIN_ATLAS
  atlasButton_ = ui->makeCheckBox(0, tr("ATLAS"), tr("ATLAS Honyaku") + " (en)" " [" + tr("Offline") + "]", this, SLOT(updateTranslators()));
#endif //WITH_WIN_ATLAS

  translateEdit_ = new TranslateEdit;
  connect(translateEdit_, SIGNAL(textChanged()), SLOT(autoTranslate()));
  connect(translateEdit_, SIGNAL(selectedTextChanged(QString)), SLOT(highlightText(QString)));
  connect(translateEdit_, SIGNAL(selectedTextChanged(QString)), SLOT(autoTranslate(QString)));
#ifdef WITH_MODULE_MECABHIGHLIGHTER
  new MeCabHighlighter(translateEdit_);
#endif // WITH_MODULE_MECABHIGHLIGHTER

  for (size_t i = 0; i < sizeof(browsers_)/sizeof(*browsers_); i++) {
    QTextBrowser *b = browsers_[i] = new TranslateBrowser;
    connect(b, SIGNAL(anchorClicked(QUrl)), SLOT(processUrl(QUrl)));
    switch (i) {
    case DictBrowser0:
    case DictBrowser1:
    case DictBrowser2:
      connect(b, SIGNAL(selectedTextChanged(QString)), SLOT(autoTranslate(QString)));
    case DictBrowser3:
      connect(b, SIGNAL(selectedTextChanged(QString)), SLOT(highlightText(QString)));
    }
  }

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *options = new QHBoxLayout,
                *translators = new QHBoxLayout;
    rows->addLayout(translators);
    rows->addLayout(options);

    options->addWidget(translateButton_);
    options->addWidget(languageCombo_);
    options->addStretch();
    options->addWidget(autoButton_);
    options->addWidget(clipboardButton_);
    options->addWidget(topButton_);

    translators->addWidget(kotobankButton_);
    translators->addWidget(jdicButton_);
    translators->addStretch();
    translators->addWidget(romajiButton_);
    translators->addWidget(ocnButton_);
    translators->addWidget(fresheyeButton_);
    translators->addWidget(infoseekButton_);
    translators->addWidget(yahooButton_);
    translators->addWidget(googleButton_);
    translators->addWidget(microsoftButton_);
    translators->addWidget(niftyButton_);
    translators->addWidget(exciteButton_);
    translators->addWidget(sdlButton_);
    translators->addWidget(systranButton_);
#ifdef WITH_WIN_ATLAS
    translators->addStretch();
    translators->addWidget(atlasButton_);
#endif // //WITH_WIN_ATLAS

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
    options->setContentsMargins(0, 0, 0, 0);
    translators->setContentsMargins(9, 0, 0, 0);
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
    m->addAction(tr("About"), this, SLOT(showAbout()));

    m->addAction(tr("Preferences"), this, SLOT(showPreferences())
#ifdef Q_OS_WIN
      , QKeySequence("ALT+O")
#endif // Q_OS_WIN
    );

    m->addAction(tr("Hide"), this, SLOT(fadeOut())
#ifdef Q_OS_WIN
      , QKeySequence("CTRL+H")
#endif // Q_OS_WIN
    );

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

MainWindow::Dictionary
MainWindow::currentDictionary() const
{
  return jdicTranslator_->isEnabled() ? Wwwjdic :
         kotobankTranslator_->isEnabled() ? Kotobank :
         DictionaryCount;
}

// See: http://msdn.microsoft.com/en-us/library/hh456380.aspx
// Must be consistent with Translator::languageCode
void
MainWindow::updateLanguage()
{
  switch (languageCombo_->currentIndex()) {
  case English: language_ = Translator::English; break;
  case TraditionalChinese: language_  = Translator::TraditionalChinese; break;
  case SimplifiedChinese: language_  = Translator::SimplifiedChinese; break;
  case Korean:  language_ = Translator::Korean; break;
  case French:  language_ = Translator::French; break;
  case German:  language_ = Translator::German; break;
  case Italian: language_ = Translator::Italian; break;
  case Spanish: language_ = Translator::Spanish; break;
  case Portuguese: language_ = Translator::Portuguese; break;
  case Russian: language_ = Translator::Russian; break;
  case Japanese:language_ = Translator::Japanese; break;

  default: Q_ASSERT(0); language_ = Translator::English;
  }
}

void
MainWindow::updateTranslators()
{
  kotobankTranslator_->setEnabled(kotobankButton_->isChecked());
  jdicTranslator_->setEnabled(jdicButton_->isChecked());

  textTranslator_->setService(TranslatorManager::Romaji, romajiButton_->isChecked());
  textTranslator_->setService(TranslatorManager::Google, googleButton_->isChecked());
  textTranslator_->setService(TranslatorManager::Microsoft, microsoftButton_->isChecked());
  textTranslator_->setService(TranslatorManager::Ocn, ocnButton_->isChecked());
  textTranslator_->setService(TranslatorManager::Excite, exciteButton_->isChecked());
  textTranslator_->setService(TranslatorManager::Sdl, sdlButton_->isChecked());
  textTranslator_->setService(TranslatorManager::Systran, systranButton_->isChecked());
  textTranslator_->setService(TranslatorManager::Nifty, niftyButton_->isChecked());
  textTranslator_->setService(TranslatorManager::Infoseek, infoseekButton_->isChecked());
  textTranslator_->setService(TranslatorManager::Yahoo, yahooButton_->isChecked());
  textTranslator_->setService(TranslatorManager::Fresheye, fresheyeButton_->isChecked());
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

  showMessage(tr("translate") + ": " + input);

  if (translatedTextChanged_) {
    browsers_[TextBrowser]->clear();
    //showTextTranslation(HTML_SS_OPEN(color:gray) "--------" HTML_SS_CLOSE());
    showTextTranslation(HTML_SS(+input+, color:gray));
    translatedTextChanged_ = false;
  }

#ifdef WITH_WIN_ATLAS
  if (isAtlasEnabled())
    showAtlasTranslation(atlas_->translate(t));
#endif // WITH_WIN_ATLAS

  //textTranslator_->translate(t, language_); // FIXME: inlining is not working under clang T_T
  for (int service = 0; service < TranslatorManager::ServiceCount; service++)
    if (textTranslator_->hasService(service))
      textTranslator_->doTranslate(service, t, language_);

  jdicTranslator_->translate(t, language_);
  kotobankTranslator_->translate(t, language_);
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

//void
//MainWindow::searchWithEngine(int engine, const QString &key)
//{
//  if (engine >= 0 && engine < searchEngines_.size() && !key.isEmpty()) {
//    const SearchEngine *e = searchEngines_[engine];
//    if (e) {
//      QString url = e->search(key);
//      if (!url.isEmpty())
//        openUrl(url);
//    }
//  }
//}

void
MainWindow::saveSettings()
{
  Settings *settings = Settings::globalSettings();
  //settings->setRecentSize(size());
  settings->setLanguageIndex(languageCombo_->currentIndex());
  settings->setDictionary(currentDictionary());
  settings->setWindowOnTop(isWindowOnTop());
  settings->setTranslationServices(textTranslator_->services());
  settings->sync();

#ifdef WITH_WIN_ATLAS
  settings->setAtlasEnabled(isAtlasEnabled());
  AcSettings::globalSettings()->setAtlasLocation(atlas_->location());
  AcSettings::globalSettings()->sync();
#endif // WITH_WIN_ATLAS
}

// - Show Translations -

void
MainWindow::showAtlasTranslation(const QString &text)
{
  showTextTranslation(
    "ATLAS: "
    HTML_SS_OPEN(color:darkgoldenrod) + text + HTML_SS_CLOSE()
  );
}

void
MainWindow::showYahooTranslation(const QString &text)
{
  showTextTranslation(
    "Yahoo!: "
    HTML_SS_OPEN(color:maroon) + text + HTML_SS_CLOSE()
  );
}

void
MainWindow::showFresheyeTranslation(const QString &text)
{
  showTextTranslation(
    "freshEYE: "
    HTML_SS_OPEN(color:teal) + text + HTML_SS_CLOSE()
  );
}

void
MainWindow::showInfoseekTranslation(const QString &text)
{
  showTextTranslation(
    "Infoseek: "
    HTML_SS_OPEN(color:red) + text + HTML_SS_CLOSE()
  );
}

void
MainWindow::showSdlTranslation(const QString &text)
{
  showTextTranslation(
    "SDL: "
    HTML_SS_OPEN(color:brown) + text + HTML_SS_CLOSE()
  );
}

void
MainWindow::showSystranTranslation(const QString &text)
{
  showTextTranslation(
    "SYSTRAN: "
    HTML_SS_OPEN(color:olive) + text + HTML_SS_CLOSE()
  );
}

void
MainWindow::showNiftyTranslation(const QString &text)
{
  showTextTranslation(
    "@nifty: "
    HTML_SS_OPEN(color:black) + text + HTML_SS_CLOSE()
  );
}

void
MainWindow::showOcnTranslation(const QString &text)
{
  showTextTranslation(
    "OCN: "
    HTML_SS_OPEN(color:blue) + text + HTML_SS_CLOSE()
  );
}

void
MainWindow::showExciteTranslation(const QString &text)
{
  showTextTranslation(
    "Excite: "
    HTML_SS_OPEN(color:magenta) + text + HTML_SS_CLOSE()
  );
}

void
MainWindow::showGoogleTranslation(const QString &text)
{
  showTextTranslation(
    "Google: "
    HTML_SS_OPEN(color:olive) + text + HTML_SS_CLOSE()
  );
}

void
MainWindow::showMicrosoftTranslation(const QString &text)
{
  showTextTranslation(
    "Microsoft: "
    HTML_SS_OPEN(color:indigo) + text + HTML_SS_CLOSE()
  );
}

void
MainWindow::showRomajiTranslation(const QString &text)
{
  showTextTranslation(
    "Romaji: "
    HTML_SS_OPEN(color:purple) + text + HTML_SS_CLOSE()
  );
}

void
MainWindow::showKotobankTranslation(const QString &text)
{ showDictTranslation(KOTOBANK_CSS + text); }

void
MainWindow::showJdicTranslation(const QString &text)
{ showDictTranslation(JDIC_CSS + text); }

void
MainWindow::showDictTranslation(const QString &text)
{
  int j = -1;
  for (int i = DictBrowser0; i <= DictBrowser3; i++)
    if (browsers_[i]->hasFocus())
      j = i+1;
  j = qBound<int>(DictBrowser0, j, DictBrowser3);
  browsers_[j]->setHtml(text);
}

void
MainWindow::showTextTranslation(const QString &text)
{ browsers_[TextBrowser]->append(text); }

void
MainWindow::processUrl(const QUrl &url)
{
  QString t = url.toString();
  if (!t.contains("://") &&
      currentDictionary() == Kotobank) {
    if (t.startsWith("/word/")) {
      enum { PrefixSize = 6 }; // size of "/word/"
      t = t.mid(PrefixSize);
      t.remove(QRegExp("?.*"));
      translate(t);
      return;
    }
    t.prepend(KotobankTranslator::host());
  }
  openUrl(t);
}

void
MainWindow::openUrl(const QString &url)
{
  showMessage(tr("opening") + ": " + url);
  browserDelegate_->openUrl(url);
}

void
MainWindow::highlightText(const QString &input)
{
  if (input.isEmpty())
    return;
  QString t = input.trimmed();
  if (t.isEmpty())
    return;

#ifndef WITH_MODULE_MECABHIGHLIGHTER
  translateEdit_->highlightText(input);
#endif // WITH_MODULE_MECABHIGHLIGHTER
  BOOST_FOREACH (TranslateBrowser *e, browsers_)
    e->highlightText(input);
}

// - Atlas Translator -

#ifdef WITH_WIN_ATLAS

bool
MainWindow::isAtlasEnabled() const
{ return atlasButton_->isChecked(); }

void
MainWindow::createAtlas()
{
  atlas_ = new Atlas(this);
  QString location = AcSettings::globalSettings()->atlasLocation();
  if (location.isEmpty() || !Atlas::isValidLocation(location))
    location = Atlas::findLocation();

  if (Atlas::isValidLocation(location)) {
    atlas_->setLocation(location);
    AcSettings::globalSettings()->setAtlasLocation(location);
  }
}

#endif // WITH_WIN_ATLAS

// EOF
