// mainwindow.cc
// 8/13/2012

#include "mainwindow.h"
#include "translatebrowser.h"
#include "translateedit.h"
#include "settings.h"
#include "trayicon.h"
#include "global.h"
#include "rc.h"
#include "src/common/acabout.h"
#include "src/common/acsettings.h"
#include "src/common/acpreferences.h"
#include "src/common/acui.h"
#include "src/common/acrc.h"
#include "src/common/acss.h"
#include "src/common/acbrowser.h"
#include "src/common/actranslator.h"
#include "lib/qtext/htmltag.h"
#include "lib/qtext/ss.h"
#include "lib/qtext/layoutwidget.h"
#include "lib/animation/fadeanimation.h"
#include "lib/translator/translatormanager.h"
#include "lib/translator/jdictranslator.h"
#include "lib/translator/kotobanktranslator.h"
#ifdef WITH_LIB_MECAB
# include "lib/mecab/mecabhighlighter.h"
#endif // WITH_LIB_MECAB
#ifdef WITH_WIN_ATLAS
# include "win/atlas/atlas.h"
#endif // WITH_WIN_ATLAS
#include <QtGui>
#include <boost/foreach.hpp>

#define DEBUG "mainwindow"
#include "lib/debug/debug.h"

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

  language_ = Settings::globalSettings()->language();
  if (!language_)
    language_ = Translator::English;

  //createSearchEngines();
  createActions();
  createLayout();
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

  (Settings::globalSettings()->dictionary() == Kotobank ? kotobankAct_ : jdicAct_)
    ->setChecked(true);

  ulong t = Settings::globalSettings()->translationServices();
  if (!t)
    t = DEFAULT_TRANSLATORS;
  romajiAct_->setChecked(t & TranslatorManager::RomajiBit);
  microsoftAct_->setChecked(t & TranslatorManager::MicrosoftBit);
  googleAct_->setChecked(t & TranslatorManager::GoogleBit);
  ocnAct_->setChecked(t & TranslatorManager::OcnBit);
  exciteAct_->setChecked(t & TranslatorManager::ExciteBit);
  sdlAct_->setChecked(t & TranslatorManager::SdlBit);
  systranAct_->setChecked(t & TranslatorManager::SystranBit);
  niftyAct_->setChecked(t & TranslatorManager::NiftyBit);
  infoseekAct_->setChecked(t & TranslatorManager::InfoseekBit);
  yahooAct_->setChecked(t & TranslatorManager::YahooBit);
  fresheyeAct_->setChecked(t & TranslatorManager::FresheyeBit);

  autoAct_->setChecked(true);
  topAct_->setChecked(isWindowOnTop());
  clipboardAct_->setChecked(Settings::globalSettings()->monitorClipboard());

#ifdef WITH_WIN_ATLAS
  atlasAct_->setChecked(Settings::globalSettings()->isAtlasEnabled());
#endif // WITH_WIN_ATLAS

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

  menuButton_ = ui->makeToolButton(AcUi::PushHint, tr("Menu"), this, SLOT(showMenu()));
  menuButton_->setMenu(contextMenu_);

  //languageCombo_ = ui->makeComboBox(AcUi::ReadOnlyHint, QString(), tr("Language")); {
  //  languageCombo_->addItem(QIcon(ACRC_IMAGE_ENGLISH), tr("English"));
  //  languageCombo_->addItem(QIcon(ACRC_IMAGE_TRADITIONAL_CHINESE), tr("Chinese")); // Chinese = Traditional Chinese, fack ccp
  //  languageCombo_->addItem(QIcon(ACRC_IMAGE_SIMPLIFIED_CHINESE), tr("Simplified Chinese"));
  //  languageCombo_->addItem(QIcon(ACRC_IMAGE_KOREAN), tr("Korean"));
  //  languageCombo_->addItem(QIcon(ACRC_IMAGE_FRENCH), tr("French"));
  //  languageCombo_->addItem(QIcon(ACRC_IMAGE_GERMAN), tr("German"));
  //  languageCombo_->addItem(QIcon(ACRC_IMAGE_ITALIAN), tr("Italian"));
  //  languageCombo_->addItem(QIcon(ACRC_IMAGE_SPANISH), tr("Spanish"));
  //  languageCombo_->addItem(QIcon(ACRC_IMAGE_PORTUGUESE), tr("Portuguese"));
  //  languageCombo_->addItem(QIcon(ACRC_IMAGE_RUSSIAN), tr("Russian"));
  //  languageCombo_->addItem(QIcon(ACRC_IMAGE_JAPANESE), tr("Japanese"));
  //}
  //int lang = Settings::globalSettings()->languageIndex();
  //lang = qBound(0, lang, languageCombo_->count());
  //languageCombo_->setCurrentIndex(lang);
  //connect(languageCombo_, SIGNAL(activated(int)), SLOT(updateLanguage()));
  //connect(languageCombo_, SIGNAL(activated(int)), SLOT(translate()));

  translateEdit_ = new TranslateEdit;
  connect(translateEdit_, SIGNAL(textChanged()), SLOT(autoTranslate()));
  connect(translateEdit_, SIGNAL(selectedTextChanged(QString)), SLOT(highlightText(QString)));
  connect(translateEdit_, SIGNAL(selectedTextChanged(QString)), SLOT(autoTranslate(QString)));
#ifdef WITH_LIB_MECAB
  new MeCabHighlighter(translateEdit_);
#endif // WITH_LIB_MECAB

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
    QHBoxLayout *header = new QHBoxLayout;
    rows->addLayout(header);

    header->addWidget(translateButton_);
    //header->addWidget(languageCombo_);
    header->addStretch();
    header->addWidget(menuButton_);

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

  QMenu *settingsMenu = m = new QMenu(tr("Settings") + " ...", this); {
    clipboardAct_ = m->addAction(tr("Monitor Clipboard"));
    clipboardAct_->setCheckable(true);

    autoAct_ = m->addAction(tr("Auto Translate Selected Text"));
    autoAct_->setCheckable(true);

    topAct_ = m->addAction(tr("Window Stays On Top"), this, SLOT(toggleWindowOnTop()));
    topAct_->setCheckable(true);
  }

  QMenu *wordMenu = m = new QMenu(tr("Word Translators") + " ...", this); {
    kotobankAct_ = m->addAction(tr("Kotobank") + " (ja)", this, SLOT(setWordTranslatorToKotobank()));
    kotobankAct_->setCheckable(true);

    jdicAct_ = m->addAction(tr("WWWJDIC") + " (en)", this, SLOT(setWordTranslatorToJdic()));
    jdicAct_->setCheckable(true);
  }

  QMenu *textMenu = m = new QMenu(tr("Text Translators") + " ...", this); {
    romajiAct_ = m->addAction(tr("Romaji"), this, SLOT(updateTranslators()));
    romajiAct_->setCheckable(true);
    m->addSeparator();

    ocnAct_ = m->addAction(tr("OCN") + " (en,zh,ko)", this, SLOT(updateTranslators()));
    ocnAct_->setCheckable(true);
    fresheyeAct_ = m->addAction(tr("freshEYE") + " (en,zh)", this, SLOT(updateTranslators()));
    fresheyeAct_->setCheckable(true);

    m->addSeparator();
    infoseekAct_ = m->addAction(tr("Infoseek"), this, SLOT(updateTranslators()));
    infoseekAct_->setCheckable(true);
    yahooAct_ = m->addAction(tr("Yahoo!"), this, SLOT(updateTranslators()));
    yahooAct_->setCheckable(true);
    microsoftAct_ = m->addAction(tr("Microsoft"), this, SLOT(updateTranslators()));
    microsoftAct_->setCheckable(true);
    googleAct_ = m->addAction(tr("Google"), this, SLOT(updateTranslators()));
    googleAct_->setCheckable(true);

    m->addSeparator();
    niftyAct_ = m->addAction(tr("@nifty") + " (en)", this, SLOT(updateTranslators()));
    niftyAct_->setCheckable(true);
    exciteAct_ = m->addAction(tr("Excite") + " (en)", this, SLOT(updateTranslators()));
    exciteAct_->setCheckable(true);
    sdlAct_ = m->addAction(tr("SDL") + " (en)", this, SLOT(updateTranslators()));
    sdlAct_->setCheckable(true);
    systranAct_ = m->addAction(tr("SYSTRAN") + " (en)", this, SLOT(updateTranslators()));
    systranAct_->setCheckable(true);
  }

  QMenu *languageMenu = m = new QMenu(tr("Language"), this); {
    AcUi::globalInstance()->setContextMenuStyle(m, true); // persistent = true

    englishAct_ = m->addAction(tr("English"), this, SLOT(setLanguageToEnglish()));
    englishAct_->setCheckable(true);
    m->addSeparator();
    traditionalChineseAct_ = m->addAction(tr("Chinese"), this, SLOT(setLanguageToTraditionalChinese()));
    traditionalChineseAct_->setCheckable(true);
    simplifiedChineseAct_ = m->addAction(tr("Simplified Chinese"), this, SLOT(setLanguageToSimplifiedChinese()));
    simplifiedChineseAct_->setCheckable(true);
    m->addSeparator();
    koreanAct_ = m->addAction(tr("Korean"), this, SLOT(setLanguageToKorean()));
    koreanAct_->setCheckable(true);
    m->addSeparator();
    frenchAct_ = m->addAction(tr("French"), this, SLOT(setLanguageToFrench()));
    frenchAct_->setCheckable(true);
    germanAct_ = m->addAction(tr("German"), this, SLOT(setLanguageToGerman()));
    germanAct_->setCheckable(true);
    italianAct_ = m->addAction(tr("Italian"), this, SLOT(setLanguageToItalian()));
    italianAct_->setCheckable(true);
    spanishAct_ = m->addAction(tr("Spanish"), this, SLOT(setLanguageToSpanish()));
    spanishAct_->setCheckable(true);
    portugueseAct_ = m->addAction(tr("Portuguese"), this, SLOT(setLanguageToPortuguese()));
    portugueseAct_->setCheckable(true);
    russianAct_ = m->addAction(tr("Russian"), this, SLOT(setLanguageToRussian()));
    russianAct_->setCheckable(true);
    m->addSeparator();
    japaneseAct_ = m->addAction(tr("Japanese"), this, SLOT(setLanguageToJapanese()));
    japaneseAct_->setCheckable(true);

  }

  // Create menus
  m = contextMenu_ = new QMenu(this); {
    AcUi::globalInstance()->setContextMenuStyle(m, true); // persistent = true

    m->addMenu(wordMenu);
    m->addMenu(textMenu);
#ifdef WITH_WIN_ATLAS
    atlasAct_ = m->addAction(tr("ATLAS Offline Translator"));
    atlasAct_->setCheckable(true);
#endif //WITH_WIN_ATLAS
    m->addSeparator();
#ifndef Q_OS_MAC
    m->addAction(tr("Preferences"), this, SLOT(showPreferences()), QKeySequence("ALT+O"));
#endif // Q_OS_WIN
    m->addMenu(settingsMenu);
    m->addMenu(languageMenu);
    m->addSeparator();
#ifndef Q_OS_MAC
    m->addAction(tr("Hide"), this, SLOT(fadeOut()), QKeySequence("CTRL+H"));
#endif // Q_OS_WIN
    m->addAction(tr("About"), this, SLOT(showAbout()));
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

void MainWindow::setLanguageToEnglish() { language_ = Translator::English; updateLanguage(); }
void MainWindow::setLanguageToJapanese() { language_ = Translator::Japanese; updateLanguage(); }
void MainWindow::setLanguageToTraditionalChinese() { language_ = Translator::TraditionalChinese; updateLanguage(); }
void MainWindow::setLanguageToSimplifiedChinese() { language_ = Translator::SimplifiedChinese; updateLanguage(); }
void MainWindow::setLanguageToKorean() { language_ = Translator::Korean; updateLanguage(); }
void MainWindow::setLanguageToFrench() { language_ = Translator::French; updateLanguage(); }
void MainWindow::setLanguageToGerman() { language_ = Translator::German; updateLanguage(); }
void MainWindow::setLanguageToItalian() { language_ = Translator::Italian; updateLanguage(); }
void MainWindow::setLanguageToSpanish() { language_ = Translator::Spanish; updateLanguage(); }
void MainWindow::setLanguageToPortuguese() { language_ = Translator::Portuguese; updateLanguage(); }
void MainWindow::setLanguageToRussian() { language_ = Translator::Russian; updateLanguage(); }

void
MainWindow::updateLanguage()
{
  englishAct_->setChecked(language_ == Translator::English);
  japaneseAct_->setChecked(language_ == Translator::Japanese);
  traditionalChineseAct_->setChecked(language_ == Translator::TraditionalChinese);
  simplifiedChineseAct_->setChecked(language_ == Translator::SimplifiedChinese);
  koreanAct_->setChecked(language_ == Translator::Korean);
  frenchAct_->setChecked(language_ == Translator::French);
  germanAct_->setChecked(language_ == Translator::German);
  italianAct_->setChecked(language_ == Translator::Italian);
  spanishAct_->setChecked(language_ == Translator::Spanish);
  portugueseAct_->setChecked(language_ == Translator::Portuguese);
  russianAct_->setChecked(language_ == Translator::Russian);
}

void
MainWindow::updateTranslators()
{
  kotobankTranslator_->setEnabled(kotobankAct_->isChecked());
  jdicTranslator_->setEnabled(jdicAct_->isChecked());

  textTranslator_->setService(TranslatorManager::Romaji, romajiAct_->isChecked());
  textTranslator_->setService(TranslatorManager::Google, googleAct_->isChecked());
  textTranslator_->setService(TranslatorManager::Microsoft, microsoftAct_->isChecked());
  textTranslator_->setService(TranslatorManager::Ocn, ocnAct_->isChecked());
  textTranslator_->setService(TranslatorManager::Excite, exciteAct_->isChecked());
  textTranslator_->setService(TranslatorManager::Sdl, sdlAct_->isChecked());
  textTranslator_->setService(TranslatorManager::Systran, systranAct_->isChecked());
  textTranslator_->setService(TranslatorManager::Nifty, niftyAct_->isChecked());
  textTranslator_->setService(TranslatorManager::Infoseek, infoseekAct_->isChecked());
  textTranslator_->setService(TranslatorManager::Yahoo, yahooAct_->isChecked());
  textTranslator_->setService(TranslatorManager::Fresheye, fresheyeAct_->isChecked());
}

void
MainWindow::setWordTranslatorToKotobank()
{
  kotobankAct_->setChecked(true);
  jdicAct_->setChecked(false);
  updateTranslators();
}

void
MainWindow::setWordTranslatorToJdic()
{
  kotobankAct_->setChecked(false);
  jdicAct_->setChecked(true);
  updateTranslators();
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
MainWindow::updateContextMenu()
{ topAct_->setChecked(isWindowOnTop());  }

void
MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
  Q_ASSERT(event);
  //updateActions();
  //if (copyMenu_->isEnabled())
  //  updateCopyMenu();
  updateContextMenu();
  contextMenu_->exec(event->globalPos());
  event->accept();
}

void
MainWindow::showMenu()
{
  updateContextMenu();
  menuButton_->showMenu();
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
  if (!clipboardAct_->isChecked())
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
{ if (autoAct_->isChecked()) translate(); }

void
MainWindow::autoTranslate(const QString &text)
{ if (autoAct_->isChecked()) translate(text); }

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
  //DOUT("size =" << size());
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
  settings->setLanguage(language_);
  settings->setDictionary(currentDictionary());
  settings->setWindowOnTop(isWindowOnTop());
  settings->setTranslationServices(textTranslator_->services());
  settings->setMonitorClipboard(clipboardAct_->isChecked());
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

#ifndef WITH_LIB_MECAB
  translateEdit_->highlightText(input);
#endif // WITH_LIB_MECAB
  BOOST_FOREACH (TranslateBrowser *e, browsers_)
    e->highlightText(input);
}

// - Atlas Translator -

#ifdef WITH_WIN_ATLAS

bool
MainWindow::isAtlasEnabled() const
{ return atlasAct_->isChecked(); }

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
