// translatormanager.cc
// 9/16/2011

#include "translatormanager.h"
#include "tr.h"
#include "rc.h" // This breaks the modularity of this pri
#ifdef WITH_MODULE_QT
#  include "module/qt/qtrc.h"
#endif // WITH_MODULE_QT
#include <QtCore>

#ifdef Q_OS_MAC
#  define K_CTRL        "cmd"
#  define K_CMD         "cmd"
#  define K_ALT         "opt"
#  define K_SHIFT       "shift"
#  define K_SPACE       "space"
#  define K_BACKSPACE   "backspace"
#  define K_ESC         "esc"
#  define K_CAPSLOCK    "capslock"
#else
#  define K_CTRL        "Ctrl"
#  define K_CMD         "Alt"
#  define K_ALT         "Alt"
#  define K_SHIFT       "Shift"
#  define K_SPACE       "Space"
#  define K_BACKSPACE   "Backspace"
#  define K_ESC         "Esc"
#  define K_CAPSLOCK    "CapsLock"
#endif // Q_OS_MAC

// - Constructions -

void
TranslatorManager::setLanguage(int language, bool updateTranslator)
{
  if (language_ != language) {
    if (updateTranslator)
      removeCurrentTranslator(qApp);
    language_ = language;
    if (updateTranslator)
      installCurrentTranslator(qApp);

    emit languageChanged();
  }
}

#ifdef WITH_MODULE_QT
  #define MAKE_TR(_tr, _TR) \
  const TranslatorManager::QTranslatorList& \
  TranslatorManager::tr_##_tr() const \
  { \
    if (tr_##_tr##_.isEmpty()) { \
      QTranslator *t = new QTranslator(const_cast<Self*>(this)); \
      t->load(RC_TR_##_TR); \
      tr_##_tr##_.append(t); \
      if (!QString(QTRC_TR_##_TR).isEmpty()) { \
        t = new QTranslator(const_cast<Self*>(this)); \
        t->load(QTRC_TR_##_TR); \
        tr_##_tr##_.append(t); \
      } \
    } \
    return tr_##_tr##_; \
  }
#else
  #define MAKE_TR(_tr, _TR) \
  const TranslatorManager::QTranslatorList& \
  TranslatorManager::tr_##_tr() const \
  { \
    if (tr_##_tr##_.isEmpty()) { \
      QTranslator *t = new QTranslator(const_cast<Self*>(this)); \
      t->load(RC_TR_##_TR); \
      tr_##_tr##_.append(t); \
    } \
    return tr_##_tr##_; \
  }
#endif // WITH_MODULE_QT

  MAKE_TR(en, EN)
  MAKE_TR(ja, JA)
  MAKE_TR(tw, TW)
  MAKE_TR(zh, ZH)
#undef MAKE_TR

TranslatorManager::QTranslatorList
TranslatorManager::currentTranslators() const
{
  switch (language_) {
  case NoLanguage:         return QTranslatorList();
  case QLocale::English:   return tr_en();
  case QLocale::Japanese:  return tr_ja();
  case TraditionalChinese: return tr_tw();
  case QLocale::Chinese:   return tr_zh();
  default: return tr_en();
  }
}

void
TranslatorManager::installCurrentTranslator(QCoreApplication *a)
{
  if (a)
    foreach (QTranslator *t, currentTranslators())
      a->installTranslator(t);
}

void
TranslatorManager::removeCurrentTranslator(QCoreApplication *a)
{
  if (a)
    foreach (QTranslator *t, currentTranslators())
      a->removeTranslator(t);
}

// - Translations -

QString
TranslatorManager::translate(int tid) const
{
#define SELF(_t)      translate(_t)
  switch (tid) {
  case T_NULL:          return QString();

  case T_TIME:          return tr("Time");
  case T_DATE:          return tr("Date");
  case T_MORE:          return tr("More");
  case T_LESS:          return tr("Less");
  case T_UPDATE:        return tr("Update");
  case T_DOWNLOAD:      return tr("Download");
  case T_WINDOWS:       return tr("Windows");
  case T_SOURCE:        return tr("Source");
  case T_OPEN:          return tr("Open");
  case T_PLAY:          return tr("Play");
  case T_START:         return tr("Start");
  case T_SIZE:          return tr("Size");
  case T_BLACKLIST:     return tr("Blacklist");
  case T_DEFAULT:       return tr("Default");
  case T_RANDOM:        return tr("Random");
  case T_POSITION:      return tr("Position");
  case T_PROGRESS:      return tr("Progress");
  case T_MEDIA:         return tr("Media");
  case T_DATA:          return tr("Data");
  case T_VIDEO:         return tr("Video");
  case T_AUDIO:         return tr("Audio");
  case T_URL:           return tr("URL");
  case T_STATE:         return tr("State");
  case T_VOLUME:        return tr("Volume");
  case T_USER:          return tr("User");
  case T_NICKNAME:      return tr("Nickname");
  case T_PERCENTAGE:    return tr("Percentage");
  case T_SPEED:         return tr("Speed");
  case T_EMAIL:         return tr("Email");
  case T_LOGIN:         return tr("Login");
  case T_OK:            return tr("OK");
  case T_APPLY:         return tr("Apply");
  case T_GALGAME:       return tr("Galgame");
  case T_SAVE:          return tr("Save");
  case T_CANCEL:        return tr("Cancel");
  case T_ADD:           return tr("Add");
  case T_DELETE:        return tr("Delete");
  case T_BACKLOG:       return tr("Backlog");
  case T_CONSOLE:       return tr("Console");
  case T_REMOVE:        return tr("Remove");
  case T_AUTO:          return tr("Auto");
  case T_ENABLE:        return tr("Enable");
  case T_CLEAR:         return tr("Clear");
  case T_MENU:          return tr("Menu");
  case T_HTML:          return tr("HTML");
  case T_HOMEPAGE:      return tr("Homepage");
  case T_RICH:          return tr("Rich");
  case T_CODE:          return tr("Code");
  case T_TIMESLOT:      return tr("Slot");
  case T_PREVIOUS:      return tr("Previous");
  case T_PREFERENCES:   return tr("Preferences");
  case T_NEXT:          return tr("Next");
  case T_FASTFORWARD:   return tr("Fast forward");
  case T_REWIND:        return tr("Rewind");
  case T_VERBATIM:      return tr("Verbatim");
  case T_SUBTITLE:      return tr("Subtitle");
  case T_AUDIOTRACK:    return tr("Audio track");
  case T_SECTION:       return tr("Section");
  case T_WORDCOUNT:     return tr("Word count");
  case T_UNKNOWN:       return tr("unknown");
  case T_BLESS:         return tr("Bless");
  case T_CURSE:         return tr("Curse");
  case T_BLOCK:         return tr("Block");
  case T_MESSAGE:       return tr("Message");

  case T_COLOR:         return tr("Color");
  case T_FOREGROUNDCOLOR: return tr("Foreground color");
  case T_BACKGROUNDCOLOR: return tr("Background color");
  case T_BOLD:          return tr("Bold");
  case T_STRIKEOUT:     return tr("Strike out");
  case T_ITALIC:        return tr("Italic");
  case T_UNDERLINE:     return tr("Underline");

  case T_DARK:          return tr("Dark");
  case T_BLACK:         return tr("Black");
  case T_BLUE:          return tr("Blue");
  case T_BROWN:         return tr("Brown");
  case T_CYAN:          return tr("Cyan");
  case T_GRAY:          return tr("Gray");
  case T_GREEN:         return tr("Green");
  case T_MAGENTA:       return tr("Magenta");
  case T_ORANGE:        return tr("Orange");
  case T_PINK:          return tr("Pink");
  case T_PURPLE:        return tr("Purple");
  case T_RED:           return tr("Red");
  case T_WHITE:         return tr("White");
  case T_YELLOW:        return tr("Yellow");
  case T_LIGHTBLUE:     return tr("Light blue");

  case T_ALIEN:         return tr("Alien");
  case T_ENGLISH:       return tr("English");
  case T_JAPANESE:      return tr("Japanese");
  case T_CHINESE:       return tr("Chinese");
  case T_TRADITIONALCHINESE: return tr("Traditional Chinese");
  case T_SIMPLIFIEDCHINESE:  return tr("Simplified Chinese");
  case T_KOREAN:        return tr("Korean");

  case T_ATTACH:        return tr("Attach");
  case T_DETACH:        return tr("Detach");
  case T_ATTACHED:      return tr("Attached");
  case T_DETACHED:      return tr("Detached");
  case T_REFRESH:       return tr("Refresh");
  case T_RESET:         return tr("Reset");

  // Defaults:
  case T_DEFAULT_USERNAME:      return "yin";
  case T_DEFAULT_PASSWORD:      return "pandora";

  // Titles:

  case T_TITLE_PROGRAM:         return tr("Annot Player");
  case T_TITLE_USER:            return tr("User");
  case T_TITLE_LOGIN:           return tr("Login");
  case T_TITLE_SEEK:            return tr("Seek");

  case T_TITLE_OPENFILE:        return tr("Open media file");
  case T_TITLE_OPENDEVICE:      return tr("Select media device");
  case T_TITLE_OPENVIDEODEVICE: return tr("Open video device");
  case T_TITLE_OPENAUDIODEVICE: return tr("Open audio device");
  case T_TITLE_OPENSUBTITLE:    return tr("Open subtitle");
  case T_TITLE_ANNOTATIONBROWSER:   tr("Annotations");
  case T_TITLE_ANNOTATIONEDITOR:    tr("Annotation Editor");
  case T_TITLE_TOKENVIEW:       return tr("Token");
  case T_TITLE_COMMENTVIEW:     return tr("Comments");
  case T_TITLE_ANNOTANALYTICS:     return tr("Annotations Analytics");
  case T_TITLE_SIGNALVIEW:      return tr("Select process signal");
  case T_TITLE_LIVE:            return tr("Live Channel");
  case T_TITLE_SYNC:            return tr("Sync Mode");

  // Messages:

  case T_ERROR_UNKNOWN_COMMAND:         return tr("unrecognized command");
  case T_ERROR_HASH_TOKEN:              return tr("failed to hash token");
  case T_ERROR_BAD_USERNAME:            return tr("invalid username");
  case T_ERROR_BAD_PASSWORD:            return tr("invalid password");
  case T_ERROR_BAD_POS:                 return tr("invalid time format");
  case T_ERROR_BAD_TOKEN:               return tr("invalid token");
  case T_ERROR_BAD_DEVICEPATH:          return tr("invalid device path");
  case T_ERROR_BAD_FILEPATH:            return tr("file does not exist");
  case T_ERROR_BAD_SUBTITLE:            return tr("failed to load subtitle");
  case T_ERROR_CLIPBOARD_UNAVAILABLE:   return tr("clipboard not available");
  case T_ERROR_SUBMIT_TOKEN:            return tr("failed to submit token");
  case T_ERROR_SUBMIT_ANNOTATION:       return tr("failed to submit annotation");
  case T_ERROR_SYNC_FAILURE:            return tr("failed to synchronize offline queue,:return try later");
  case T_ERROR_NO_MEDIA:                return tr("no media");
  case T_ERROR_NETWORK:                 return tr("network error");

  case T_SUCCEED_COPIED:                return tr("copied to clipboard");

  case T_WARNING_LONG_STRING_TRUNCATED:  return tr("long string got truncated");
  case T_MESSAGE_ENABLE_WINDOWSTAYSONTOP:       return tr("window always on top");

  case T_MESSAGE_TRY_LOGINFROMCACHE:    return tr("try to get user information from cache");
  case T_SUCCEED_LOGINFROMCACHE:        return tr("succeeded to login user from cache");
  case T_ERROR_LOGINFROMCACHE_FAILURE:  return tr("failed to login user from cache");

  // Widgets:
  case T_USERNAME:              return tr("Username");
  case T_PASSWORD:              return tr("Password");

  case T_TOOLTIP_ANNOTATIONITEM:return tr("Comment"); // TO BE GENERATED DYNAMICALL
  case T_TOOLTIP_INPUTLINE:     return tr("Annotation");
  case T_TOOLTIP_PREFIXLINE:    return tr("Annotation prefix");
  case T_TOOLTIP_TRACE:         return tr("Trace Window");

  case T_SEEK:                  return tr("Seek");

  // Formats:
  case T_FORMAT_ALL:            return tr("All files");
  case T_FORMAT_SUPPORTED:      return tr("All supported");
  case T_FORMAT_VIDEO:          return tr("Video");
  case T_FORMAT_AUDIO:          return tr("Audio");
  case T_FORMAT_PICTURE:        return tr("Picture");
  case T_FORMAT_IMAGE:          return tr("Image");
  case T_FORMAT_SUBTITLE:       return tr("Subtitle");
  case T_FORMAT_PROGRAM:        return tr("Program");

  // Actions:

  case T_MENUTEXT_OPEN:         return tr("Open");
  case T_TIP_OPEN:              return tr("Open media file or game process");

  case T_MENUTEXT_OPENFILE:     return tr("Open file");
  case T_TIP_OPENFILE:          return tr("Open media file");

  case T_MENUTEXT_OPENURL:     return tr("Open media URL");
  case T_TIP_OPENURL:          return tr("Open media from Internet or local URL");

  case T_MENUTEXT_OPENANNOTATIONURL:     return tr("Import annots from URL");
  case T_TIP_OPENANNOTATIONURL:          return tr("Add annotations from URL");

  case T_MENUTEXT_OPENDEVICE:   return tr("Open DVD/VCD/CD device");
  case T_TIP_OPENDEVICE:        return tr("Open media device");

  case T_MENUTEXT_OPENVIDEODEVICE:   return tr("Open DVD/VCD folder");
  case T_TIP_OPENVIDEODEVICE:        return tr("Open video folder");

  case T_MENUTEXT_OPENAUDIODEVICE:   return tr("Open CD folder");
  case T_TIP_OPENAUDIODEVICE:        return tr("Open audio folder");

  case T_MENUTEXT_OPENSUBTITLE: return tr("Open subtitle");
  case T_TIP_OPENSUBTITLE:      return tr("Load subtitle from file");

  case T_MENUTEXT_OPENCONTEXTMENU: return tr("Open");
  case T_TIP_OPENCONTEXTMENU:      return tr("Open");

  case T_OPENINWEBBROWSER:      return tr("Open in web browser");
  case T_DOWNLOADCURRENT:       return tr("Download current media");

  case T_MENUTEXT_OPENDIRECTORY:         return tr("Open directory");

  case T_MENUTEXT_PLAY:         return tr("Play") + " [" K_SPACE "]";
  case T_TIP_PLAY:              return tr("Play media") + " [" K_SPACE "]";

  case T_MENUTEXT_FASTFORWARD:  return tr("Fast forward") + " " "x4";
  case T_MENUTEXT_FASTFASTFORWARD: return tr("Fast forward") + " " "x16";

  case T_MENUTEXT_MENU:         return tr("Menu");
  case T_TIP_MENU:              return tr("Show menu");

  case T_MENUTEXT_PAUSE:        return tr("Pause") + " [" K_SPACE "]";
  case T_TIP_PAUSE:             return tr("Pause playing") + " [" K_SPACE "]";

  case T_MENUTEXT_STOP:         return tr("Stop");
  case T_TIP_STOP:              return tr("Stop playing");

  case T_MENUTEXT_NEXTFRAME:    return tr("Next frame");
  case T_TIP_NEXTFRAME:         return tr("Next frame");

  case T_MENUTEXT_REPLAY:       return tr("Replay");
  case T_TIP_REPLAY:            return tr("Restart playing");

  case T_MENUTEXT_MINI:         return tr("Mini player");// + " [" K_CMD "+2]";
  case T_TIP_MINI:              return tr("Show mini player") + " [" K_CMD "+2]";

  case T_MENUTEXT_EMBED:        return tr("Embed player");// + " [" K_CMD "+1]";
  case T_TIP_EMBED:             return tr("Embed player window") + " [" K_CMD "+1]";

  case T_MENUTEXT_EMBEDONTOP:   return tr("Embed on top");
  case T_TIP_EMBEDONTOP:        return tr("Embed player window on the top");

  case T_MENUTEXT_LIVE:         return tr("Live channel");
  case T_TIP_LIVE:              return tr("Toggle live mode");

  case T_MENUTEXT_SYNC:         return tr("Sync mode");
  case T_TIP_SYNC:              return tr("Toggle Sync mode");

  case T_MENUTEXT_FULLSCREEN:   return tr("Fullscreen");// + " [" K_CMD "+3]";
  case T_TIP_FULLSCREEN:        return tr("Show fullscreen") + " [" K_CMD "+3]";

  case T_MENUTEXT_SNAPSHOT:     return tr("Snapshot");
  case T_TIP_SNAPSHOT:          return tr("Taks a snapshot of current frame");

  case T_MENUTEXT_ANNOT:        return tr("Toggle annot");
  case T_TIP_ANNOT:             return tr("Toggle annotations");
  case T_MENUTEXT_SHOWANNOT:    return tr("Show annots");
  case T_TIP_SHOWANNOT:         return tr("Show annots");
  case T_MENUTEXT_HIDEANNOT:    return tr("Hide annots");
  case T_TIP_HIDEANNOT:         return tr("Hide annotations");

  case T_MENUTEXT_QUIT:         return tr("Exit");
  case T_TIP_QUIT:              return tr("Quit the application");

  case T_MENUTEXT_USER:         return tr("User panel");
  case T_TIP_USER:              return tr("Show user panel");

  case T_MENUTEXT_SEEK:         return tr("Seek");
  case T_TIP_SEEK:              return tr("Seek position");

  case T_MENUTEXT_LOGIN:        return tr("Login");
  case T_TIP_LOGIN:             return tr("Show login dialog");

  case T_MENUTEXT_LOGOUT:       return tr("Logout");
  case T_TIP_LOGOUT:            return tr("Logout");

  case T_MENUTEXT_LOGINDIALOG:  return tr("Login");
  case T_TIP_LOGINDIALOG:       return tr("Show login dialog");

  case T_MENUTEXT_LIVEDIALOG:   return tr("Choose live time");
  case T_TIP_LIVEDIALOG:        return tr("Show live mode dialog");

  case T_MENUTEXT_SYNCDIALOG:   return tr("Choose sync time");
  case T_TIP_SYNCDIALOG:        return tr("Show sync mode dialog");

  case T_MENUTEXT_SEEKDIALOG:   return tr("Seek");
  case T_TIP_SEEKDIALOG:        return tr("Show seek dialog");

  case T_MENUTEXT_PREVIOUS:     return tr("Previous") + " [" K_CTRL "+" K_SHIFT "+←]";
  case T_TIP_PREVIOUS:          return tr("Previous") + " [" K_CTRL "+" K_SHIFT "+←]";
  case T_MENUTEXT_NEXT:         return tr("Next") + " [" K_CTRL "+" K_SHIFT "+→]";
  case T_TIP_NEXT:              return tr("Next") + " [" K_CTRL "+" K_SHIFT "+→]";

  case T_MENUTEXT_BLACKLIST:    return tr("Blacklist");// + " [" K_CTRL + "+F4]";
  case T_TIP_BLACKLIST:         return tr("Blacklist");

  case T_MENUTEXT_PICKDIALOG:   return tr("Pick window");
  case T_TIP_PICKDIALOG:        return tr("Show pick dialog");

  case T_MENUTEXT_PROCESSPICKDIALOG:   return tr("Pick Game Window");
  case T_TIP_PROCESSPICKDIALOG:        return tr("Select process window");

  case T_MENUTEXT_WINDOWPICKDIALOG:   return tr("Select Annotation Window");
  case T_TIP_WINDOWPICKDIALOG:        return tr("Select window to show annots");

  case T_TITLE_ABOUT:           return tr("About");
  case T_MENUTEXT_ABOUT:        return tr("About");
  case T_TIP_ABOUT:             return tr("About us");

  case T_TITLE_HELP:            return tr("Help");
  case T_MENUTEXT_HELP:         return tr("Help");
  case T_TIP_HELP:              return tr("Help");

  case T_COPY:         return tr("Copy");
  case T_PASTE:        return tr("Paste");

  case T_EXAMPLE:      return tr("Example");

  case T_MENUTEXT_SUBTITLE:     return tr("Subtitle");
  case T_TIP_SUBTITLE:          return tr("Subtitle");

  case T_MENUTEXT_ANNOTSUBTITLE: return tr("Annotation subtitle");
  case T_TIP_ANNOTSUBTITLE:      return tr("Annotation as subtitle");

  case T_MENUTEXT_SECTION:      return tr("DVD sections");
  case T_TIP_SECTION:           return tr("Select DVD sections");

  case T_MENUTEXT_SHOWSUBTITLE: return tr("Show subtitle");
  case T_TIP_SHOWSUBTITLE:      return tr("Show subtitle");

  case T_MENUTEXT_HIDESUBTITLE: return tr("Hide subtitle");
  case T_TIP_HIDESUBTITLE:      return tr("Hide subtitle");

  case T_MENUTEXT_SUBANNOT:     return tr("Show subtitle annot");
  case T_TIP_SUBANNOT:          return tr("Show subtitle annot");

  case T_MENUTEXT_NONSUBANNOT:  return tr("Show non-sub annot");
  case T_TIP_NONSUBANNOT:       return tr("Show non-sub annot");

  case T_MENUTEXT_REMOVEANNOTATION:   return tr("Hide");
  case T_TIP_REMOVEANNOTATION:        return tr("Hide");

  case T_MENUTEXT_ENABLEAUTOCLEARCONSOLE: return tr("Auto clear console");
  case T_TIP_ENABLEAUTOCLEARCONSOLE:      return tr("Enable auto clear console");

  case T_MENUTEXT_DISABLEAUTOCLEARCONSOLE: return tr("Stick console");
  case T_TIP_DISABLEAUTOCLEARCONSOLE:      return tr("Disable auto clear console");

  case T_MENUTEXT_WINDOWSTAYSONTOP:   return tr("Always on top"); // + " [" K_CTRL "+T]";
  case T_TIP_WINDOWSTAYSONTOP:        return tr("Show window on top");

  case T_MENUTEXT_ADVANCED:     return tr("Advanced");
  case T_TIP_ADVANCED:          return tr("Advanced menu");

  case T_MENUTEXT_BLESSUSER:    return tr("Bless user");
  case T_TIP_BLESSUSER:         return tr("Bless user");

  case T_MENUTEXT_CURSEUSER:    return tr("Curse user");
  case T_TIP_CURSEUSER:         return tr("Curse user");

  case T_MENUTEXT_BLOCKUSER:    return tr("Block user");
  case T_TIP_BLOCKUSER:         return tr("Block user");

  case T_MENUTEXT_EDITTHISANNOT:return tr("Edit annot");
  case T_TIP_EDITTHISANNOT:     return tr("Edit annotation");

  case T_MENUTEXT_DELETETHISANNOT:return tr("Delete annot");
  case T_TIP_DELETETHISANNOT:     return tr("Delete annotation");

  case T_MENUTEXT_BLOCKTHISANNOT:return tr("Block annot");
  case T_TIP_BLOCKTHISANNOT:    return tr("Block annotation");

  case T_MENUTEXT_BLESSTHISANNOT:return tr("Bless annot");
  case T_TIP_BLESSTHISANNOT:    return tr("Bless annotation");

  case T_MENUTEXT_CURSETHISANNOT:return tr("Curse annot");
  case T_TIP_CURSETHISANNOT:    return tr("Curse annotation");

  case T_MENUTEXT_BLESSTHISTOKEN:return tr("Bless token");
  case T_TIP_BLESSTHISTOKEN:    return tr("Bless token");

  case T_MENUTEXT_CURSETHISTOKEN:return tr("Curse token");
  case T_TIP_CURSETHISTOKEN:    return tr("Curse token");

  case T_MENUTEXT_HIDETHISANNOT:return tr("Hide annot");
  case T_TIP_HIDETHISANNOT:     return tr("Hide annotation");

  case T_MENUTEXT_SHOWTHISANNOT:return tr("Show annot");
  case T_TIP_SHOWTHISANNOT:     return tr("Show annotation");

  case T_MENUTEXT_COPYTHISANNOT:return tr("Copy annot");
  case T_TIP_COPYTHISANNOT:     return tr("Copy annotation");

  case T_MENUTEXT_VIEWTHISUSER: return tr("Display user");
  case T_TIP_VIEWTHISUSER:      return tr("Display user");

  case T_TITLE_SUBTITLEVIEW:    return tr("Subtitle history");
  case T_MENUTEXT_SUBTITLEVIEW: return tr("Subtitle history");
  case T_TIP_SUBTITLEVIEW:      return tr("Show subtitle history");

  case T_MENUTEXT_BLOCKTHISUSER:return tr("Block user");
  case T_TIP_BLOCKTHISUSER:     return tr("Block user");

  case T_MENUTEXT_CLEARCACHE:   return tr("Clear offline cache");
  case T_TIP_CLEARCACHE:        return tr("Remove offline cache");

  case T_MENUTEXT_CONNECT:      return tr("Synchronize with server");
  case T_TIP_CONNECT:           return tr("Connect to server and synchronize local cache");
  case T_MENUTEXT_DISCONNECT:   return tr("Disconnect from server");
  case T_TIP_DISCONNECT:        return tr("Disconnect from server and use local cache only");

  case T_MENUTEXT_ANNOTATIONEDITOR: return tr("Annotation Editor");// + " [" K_CTRL "+F1]";
  case T_TIP_ANNOTATIONEDITOR: return tr("Show annotation editor");

  case T_MENUTEXT_ANNOTATIONBROWSER: return tr("Search Annotations");// + " [" K_CTRL "+F2]";
  case T_TIP_ANNOTATIONBROWSER: return tr("Show annotation browser");

  case T_MENUTEXT_TOKENVIEW:    return tr("Token");// + " [" K_CTRL "+F3]";
  case T_TIP_TOKENVIEW:         return tr("Show token view");

  case T_MENUTEXT_COMMENTVIEW:  return tr("Comments");
  case T_TIP_COMMENTVIEW:       return tr("Show comment browser");

  case T_MENUTEXT_PROXYVIEW:    return tr("Proxy browser");
  case T_TIP_PROXYVIEW:         return tr("Proxy web browser");

  case T_MENUTEXT_EDIT:         return tr("Edit");
  case T_TIP_EDIT:              return tr("Edit");

  case T_MENUTEXT_CLEAR:        return tr("Clear");
  case T_TIP_CLEAR:             return tr("Clear");

  case T_MENUTEXT_ADDALIAS:     return tr("Add alias");
  case T_TIP_ADDALIAS:          return tr("Add alias");

  case T_MENUTEXT_SIGNALVIEW:     return tr("Select Game Process");
  case T_TIP_SIGNALVIEW:          return tr("Select process signal");

  case T_MENUTEXT_ATTACHPROCESS: return tr("Attach process");
  case T_TIP_ATTACHPROCESS:     return tr("Attach process");

  case T_MENUTEXT_DETACHPROCESS: return tr("Detach process");
  case T_TIP_DETACHPROCESS:     return tr("Detach process");

  case T_MENUTEXT_REFRESHPROCESS: return tr("Refresh process");
  case T_TIP_REFRESHPROCESS:    return tr("Refresh process");

  case T_MENUTEXT_RECENTMESSAGES: return tr("Switch channel");
  case T_TIP_RECENTMESSAGES:    return tr("Switch channel");

  case T_MENUTEXT_PLAYLIST:     return tr("Playlist");
  case T_TIP_PLAYLIST:          return tr("Playlist");

  case T_MENUTEXT_APPLANGUAGE:  return tr("Application language");
  case T_TIP_APPLANGUAGE:       return tr("Choose application language");

  case T_ANYLANGUAGE:           return tr("Any language");
  case T_MENUTEXT_ANYLANGUAGE:  return tr("All");
  case T_TIP_ANYLANGUAGE:       return tr("Choose all languages");

  case T_MENUTEXT_ANNOTATIONLANGUAGE:  return tr("Annot languages");
  case T_TIP_ANNOTATIONLANGUAGE:       return tr("Choose annotation language");

  case T_MENUTEXT_USERLANGUAGE:  return tr("User language");
  case T_TIP_USERLANGUAGE:       return tr("Choose user language");

  case T_MENUTEXT_ANONYMOUS:  return tr("Anonymous");
  case T_TIP_ANONYMOUS:       return tr("User anonymous");

  case T_MENUTEXT_TRANSLATE:  return tr("Show translation");
  case T_TIP_TRANSLATE:       return tr("Display translation");

  case T_MENUTEXT_SUBTITLEONTOP:  return tr("Subtitle on top");
  case T_TIP_SUBTITLEONTOP:       return tr("Display subtitle on the top/bottom");

  case T_MENUTEXT_SUBTITLESTYLE:  return tr("Subtitle color");
  case T_TIP_SUBTITLESTYLE:       return tr("Select subtitle color");

  case T_MENUTEXT_THEME:  return tr("Theme");
  case T_TIP_THEME:       return tr("Select GUI theme");

  case T_LABEL_CREATEDATE:      return tr("Create date");
  case T_TIP_CREATEDATE:        return tr("Create date");
  case T_LABEL_UPDATEDATE:      return tr("Update date");
  case T_TIP_UPDATEDATE:        return tr("Update date");
  case T_LABEL_BLESSEDCOUNT:    return tr("Blessed");
  case T_TIP_BLESSEDCOUNT:      return tr("Blessed count");
  case T_LABEL_CURSEDCOUNT:     return tr("Cursed");
  case T_TIP_CURSEDCOUNT:       return tr("Cursed count");
  case T_LABEL_BLOCKEDCOUNT:    return tr("Blocked");
  case T_TIP_BLOCKEDCOUNT:      return tr("Blocked count");
  case T_LABEL_VISITEDCOUNT:    return tr("Visited");
  case T_TIP_VISITEDCOUNT:      return tr("Visited count");
  case T_LABEL_ANNOTCOUNT:      return tr("Annot");
  case T_TIP_ANNOTCOUNT:        return tr("Annotation count");

  case T_LABEL_ALIAS:           return tr("Alias");
  case T_TIP_ALIAS:             return tr("Alias");

  case T_MENUTEXT_BACKWARD:     return tr("Backward");
  case T_MENUTEXT_FORWARD:      return tr("Forward");
  case T_MENUTEXT_FORWARD5S:    return tr("Forward %1 sec").arg("5");
  case T_MENUTEXT_BACKWARD5S:   return tr("Backward %1 sec").arg("5");
  case T_MENUTEXT_FORWARD10S:   return tr("Forward %1 sec").arg("10");
  case T_MENUTEXT_BACKWARD10S:  return tr("Backward %1 sec").arg("10");
  case T_MENUTEXT_FORWARD25S:   return tr("Forward %1 sec").arg("30") + " [→]";
  case T_MENUTEXT_BACKWARD30S:  return tr("Backward %1 sec").arg("30") + " [←]";
  case T_MENUTEXT_FORWARD60S:   return tr("Forward %1 sec").arg("60");
  case T_MENUTEXT_BACKWARD60S:  return tr("Backward %1 sec").arg("60");
  case T_MENUTEXT_FORWARD85S:   return tr("Forward %1 sec").arg("90") + " [" K_SHIFT "+→]";
  case T_MENUTEXT_BACKWARD90S:  return tr("Backward %1 sec").arg("90") + " [" K_SHIFT "+←]";
  case T_MENUTEXT_FORWARD1M:    return tr("Forward %1 min").arg("1");
  case T_MENUTEXT_BACKWARD1M:   return tr("Backward %1 min").arg("1");
  case T_MENUTEXT_FORWARD5M:    return tr("Forward %1 min").arg("5");
  case T_MENUTEXT_BACKWARD5M:   return tr("Backward %1 min").arg("5");
  case T_MENUTEXT_FORWARD10M:   return tr("Forward %1 min").arg("10");
  case T_MENUTEXT_BACKWARD10M:  return tr("Backward %1 min").arg("10");

  case T_MENUTEXT_RECENT:       return tr("Recent");
  case T_TIP_RECENT:            return tr("Recent file");
  case T_MENUTEXT_CLEARRECENT:  return tr("Clear");
  case T_TIP_CLEARRECENT:       return tr("Clear recent files");

  case T_MENUTEXT_MINIMIZE:     return tr("Minimize and pause") + " [" K_CTRL "+" K_ESC "]";
  case T_TIP_MINIMIZE:          return tr("Minimize and pause") + " [" K_CTRL "+" K_ESC "]";

  case T_MENUTEXT_ANNOTATIONLIMIT: return tr("Annot count");
  case T_TIP_ANNOTATIONLIMIT:      return tr("Hint maximum annotation count");

  case T_MENUTEXT_BROWSE:       return tr("Browse");
  case T_TIP_BROWSE:            return tr("Files in the same folder");

  case T_MENUTEXT_TRACK:        return tr("Tracks");
  case T_TIP_TRACK:             return tr("Media tracks");

  case T_MENUTEXT_AUTOPLAYNEXT: return tr("Auto play next media");
  case T_TIP_AUTOPLAYNEXT:      return tr("Automatically play next media");

  case T_AUTOPLAYCURRENT:       return tr("Repeat current media");

  case T_NOAUTOPLAY:            return tr("No repeat");

  case T_MENUTEXT_SHOWMENUBAR:  return tr("Show menubar") + " [" K_CAPSLOCK "]";
  case T_TIP_SHOWMENUBAR:       return tr("Show menubar");

  case T_MENUTEXT_ENABLEAERO:   return tr("Enable Aero");
  case T_MENUTEXT_DISABLEAERO:  return tr("Disable Aero");

  case T_MENUTEXT_CHECKINTERNET:return tr("Check Internet");
  case T_TIP_CHECKINTERNET:     return tr("Check Internet connection");

  case T_MENUTEXT_DELETECACHE:  return tr("Remove caches");
  case T_TIP_DELETECACHE:       return tr("Remove offline caches");

  case T_MENUTEXT_SITEACCOUNT:  return tr("Link accounts");
  case T_TIP_SITEACCOUNT:       return tr("Set accounts for websites");

  case T_MENUTEXT_ENABLEBLACKLIST:  return tr("Enable blacklist");
  case T_TIP_ENABLEBLACKLIST:       return tr("Enable blacklist");

  case T_MENUTEXT_DOWNLOAD:  return tr("Download");// + " [" K_CTRL "+D]";
  case T_TIP_DOWNLOAD:       return tr("Download");// + " [" K_CTRL "+D]";

  case T_MENUTEXT_ANNOTANALYTICS:  return tr("Annotation analytics");// + " [" K_CTRL "+F5]";
  case T_TIP_ANNOTANALYTICS:       return tr("Show annotations as thread");

  case T_MENUTEXT_SAVEMEDIA:    return tr("Save buffered video");
  case T_TIP_SAVEMEDIA:         return tr("Save buffered video on desktop");

  case T_MENUTEXT_AUTOSAVEMEDIA:  return tr("Save buffered video");
  case T_TIP_AUTOSAVEMEDIA:       return tr("Save buffered video on desktop");

  case T_MENUTEXT_MONITORCLIPBOARD:  return tr("Monitor clipboard");

  case T_MENUTEXT_NOTHINGAFTERFINISHED: return tr("Nothing");
  case T_MENUTEXT_SHUTDOWNAFTERFINISHED: return tr("Shutdown");
  case T_MENUTEXT_SLEEPAFTERFINISHED: return tr("Sleep");

  case T_MENUTEXT_ASPECTRATIOSTANDARD: return tr("Standard") + " (4:3)";
  case T_MENUTEXT_ASPECTRATIOWIDESCREEN: return tr("Wide screen") + " (16:9)";

  case T_MENUTEXT_MULTIWINDOW: return tr("Allow multiple windows");

  case T_MENUTEXT_RESUMEANNOT:  return tr("Release annotations");

  case T_FILTER_PATTERN:        return tr("Filter pattern");
  case T_FILTER_SYNTAX:         return tr("Filter syntax");
  case T_FILTER_COLUMN:         return tr("Filter column");
  case T_FILTER_REGEX:          return tr("Regular expression");
  case T_FILTER_WILDCARD:       return tr("Wildcard");
  case T_FILTER_FIXED:          return tr("Fixed string");

  case T_ID:            return tr("ID");
  case T_NAME:          return tr("Name");
  case T_TYPE:          return tr("Type");
  case T_TAG:           return tr("Tag");
  case T_ANNOTATION:    return tr("Annotation");
  case T_ALIAS:         return tr("Alias");
  case T_USER_ID:       return tr("UserID");
  case T_USER_ALIAS:    return tr("UserAlias");
  case T_TOKEN_ID:      return tr("TokenId");
  case T_HASH:          return tr("Hash");
  case T_TEXT:          return tr("Text");
  case T_POS:           return tr("Pos");
  case T_CREATETIME:    return tr("CreateTime");
  case T_LOGINTIME:     return tr("LoginTime");
  case T_UPDATETIME:    return tr("UpdateTime");
  case T_LANGUAGE:      return tr("Language");
  case T_STATUS:        return tr("Status");
  case T_FLAGS:         return tr("Flags");
  case T_BLESSEDCOUNT:  return tr("BlessedCount");
  case T_CURSEDCOUNT:   return tr("CursedCount");
  case T_BLOCKEDCOUNT:  return tr("BlockedCount");
  case T_ANNOTCOUNT:    return tr("AnnotCount");
  case T_VISITEDCOUNT:  return tr("VisitedCount");

  case T_PID:           return tr("PID");
  case T_FOLDER:        return tr("Folder");
  case T_PATH:          return tr("Path");

  case T_ACTIVE:        return tr("Active");
  case T_DELETED:       return tr("Deleted");
  case T_BLOCKED:       return tr("Blocked");
  case T_ANNOT:         return tr("Annot");
  case T_NA:            return tr("n/a");
  case T_ANONYMOUS:     return tr("Anonymous");

  case T_MAXIMIZE:      return tr("Maximize");
  case T_MINIMIZE:      return tr("Minimize");
  case T_RESTORE:       return tr("Restore");

  case T_HISTORY:       return tr("History");
  case T_WEBSITE:       return tr("Website");

  case T_ANNOTATIONEFFECT: return tr("Annotation effect");
  case T_TRANSPARENT:      return tr("Transparent");
  case T_SHADOW:           return tr("Shadow");
  case T_BLUR:             return tr("Blur");

  case T_INCREASE:      return tr("Increase");
  case T_DECREASE:      return tr("Decrease");

  case T_SECOND:        return tr("Second");
  case T_MINUTE:        return tr("Minute");
  case T_HOUR:          return tr("Hour");

  case T_NETWORKPROXY:  return tr("Network proxy");
  case T_MENUTHEME:     return tr("Render context menu");

  case T_URL_NICOVIDEO: return tr("http://nicovideo.jp");
  case T_URL_BILIBILI:  return tr("http://bilibili.tv");

  case T_ASPECTRATIO:   return tr("Aspect ratio");
  case T_NEWWINDOW:     return tr("New window"); // + " [" K_CTRL "+" K_SHIFT "+" "N]";

  case T_MENUTEXT_LABELPLAYER: return tr("Label player with annots");
  case T_MENUTEXT_AUTOSUBMIT:  return tr("Save annots associations");
  case T_MENUTEXT_BACKLOG:     return tr("Backlog");// + " [" K_CTRL "+F6]";
  case T_MENUTEXT_PREFERLOCALDB:     return tr("Prefer offline annotations");

  case T_MENUTEXT_RESETANNOTSCALE:  return tr("Reset scale") + " [" K_CTRL "+" + tr("Mid") + "]";
  case T_TIP_RESETANNOTSCALE:       return tr("Reset annotation scale");

  case T_MENUTEXT_INCREASEANNOTSCALE:  return tr("Scale up") + " [" K_CTRL "+" + tr("Wheel") + "↑]";
  case T_TIP_INCREASEANNOTSCALE:       return tr("Scale up");

  case T_MENUTEXT_DECREASEANNOTSCALE:  return tr("Scale down") + " [" K_CTRL "+" + tr("Wheel") + "↓]";
  case T_TIP_DECREASEANNOTSCALE:       return tr("Scale down");

  case T_MENUTEXT_RESETANNOTOFFSET:  return tr("Reset offset") + " [" K_CTRL "+" K_ALT "+" + tr("Mid") + "]";
  case T_TIP_RESETANNOTOFFSET:       return tr("Reset annotation offset in time");

  case T_MENUTEXT_INCREASEANNOTOFFSET:  return tr("Increase offset") + " [" K_CTRL "+" K_ALT "+" + tr("Wheel") + "↑]";
  case T_TIP_INCREASEANNOTOFFSET:       return tr("Increase offset");

  case T_MENUTEXT_DECREASEANNOTOFFSET:  return tr("Decrease offset") + " [" K_CTRL "+" K_ALT "+" + tr("Wheel") + "↓]";
  case T_TIP_DECREASEANNOTOFFSET:       return tr("Decrease offset");

  case T_MENUTEXT_RESETANNOTROTATION:  return tr("Reset rotation") + " [" K_SHIFT "+" + tr("Mid") + "]";
  case T_TIP_RESETANNOTROTATION:       return tr("Reset annotation rotation");

  case T_MENUTEXT_INCREASEANNOTROTATION:  return tr("Rotate up") + " [" K_SHIFT "+" + tr("Wheel") + "↑]";
  case T_TIP_INCREASEANNOTROTATION:       return tr("Rotate up");

  case T_MENUTEXT_DECREASEANNOTROTATION:  return tr("Rotate down") + " [" K_SHIFT "+" + tr("Wheel") + "↓]";
  case T_TIP_DECREASEANNOTROTATION:       return tr("Rotate down");

  case T_COPYCURRENTURL:       return tr("Copy URL");
  case T_COPYCURRENTTITLE:     return tr("Copy Title");
  case T_ACTUALSIZE:       return tr("Actual size");

  case T_HUEUP: return tr("Hue up");
  case T_HUEDOWN: return tr("Hue down");
  case T_CONTRASTUP: return tr("Contrast up");
  case T_CONTRASTDOWN: return tr("Contrast down");
  case T_GAMMAUP: return tr("Gamma up");
  case T_GAMMADOWN: return tr("Gamma down");
  case T_BRIGHTNESSUP: return tr("Brightness up");
  case T_BRIGHTNESSDOWN: return tr("Brightness down");
  case T_SATURATIONUP: return tr("Saturation up");
  case T_SATURATIONDOWN: return tr("Saturation down");

  case T_HUE:           return tr("Hue");
  case T_SATURATION:    return tr("Saturation");
  case T_GAMMA:         return tr("Gamma");
  case T_CONTRAST:      return tr("Contrast");
  case T_BRIGHTNESS:    return tr("Brightness");

  case T_MEDIAINFO:  return tr("Media information");

  default:
    qWarning() << "TranslatorManager:translate: Unknown tid =" << tid;
    Q_ASSERT(0);
    return QString();
  }
#undef SELF
}

// EOF
