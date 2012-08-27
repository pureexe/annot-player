// localizer.cc
// 9/16/2011

#include "localizer.h"
#include "tr.h"
#include "rc.h" // This breaks the modularity of this pri
#include "module/qt/qtrc.h"
#include <QtCore>

#ifdef Q_OS_MAC
# define K_CTRL        "cmd"
# define K_CMD         "cmd"
# define K_ALT         "opt"
# define K_SHIFT       "shift"
# define K_SPACE       "space"
# define K_BACKSPACE   "backspace"
# define K_ESC         "esc"
# define K_CAPSLOCK    "capslock"
#else
# define K_CTRL        "Ctrl"
# define K_CMD         "Alt"
# define K_ALT         "Alt"
# define K_SHIFT       "Shift"
# define K_SPACE       "Space"
# define K_BACKSPACE   "Backspace"
# define K_ESC         "Esc"
# define K_CAPSLOCK    "CapsLock"
#endif // Q_OS_MAC

// - Constructions -

void
Localizer::setLocale(int language, int script, bool updateTranslator)
{
  if (language_ != language || script_ != script) {
    if (updateTranslator)
      removeCurrentTranslator(qApp);
    language_ = language;
    script_ = script;
    if (updateTranslator)
      installCurrentTranslator(qApp);

    emit localeChanged();
  }
}

// - Properties -

const Localizer::QTranslatorList&
Localizer::tr_en() const
{
  if (tr_en_.isEmpty()) {
    QTranslator*
    t = new QTranslator(const_cast<Self *>(this));
    t->load(RC_TR_EN);
    tr_en_.append(t);

    if (!QString(RC_TR_EN).isEmpty()) {
      t = new QTranslator(const_cast<Self *>(this));
      t->load(RC_TR_EN);
      tr_en_.append(t);
    }
  }
  return tr_en_;
}

const Localizer::QTranslatorList&
Localizer::tr_ja() const
{
  if (tr_ja_.isEmpty()) {
    QTranslator*
    t = new QTranslator(const_cast<Self *>(this));
    t->load(RC_TR_JA);
    tr_ja_.append(t);

    if (!QString(RC_TR_JA).isEmpty()) {
      t = new QTranslator(const_cast<Self *>(this));
      t->load(QTRC_TR_JA);
      tr_ja_.append(t);
    }
  }
  return tr_ja_;
}

const Localizer::QTranslatorList&
Localizer::tr_zh_CN() const
{
  if (tr_zh_CN_.isEmpty()) {
    QTranslator*
    t = new QTranslator(const_cast<Self *>(this));
    t->load(RC_TR_ZH_CN);
    tr_zh_CN_.append(t);

    if (!QString(RC_TR_ZH_CN).isEmpty()) {
      t = new QTranslator(const_cast<Self *>(this));
      t->load(QTRC_TR_ZH_CN);
      tr_zh_CN_.append(t);
    }
  }
  return tr_zh_CN_;
}

const Localizer::QTranslatorList&
Localizer::tr_zh_TW() const
{
  if (tr_zh_TW_.isEmpty()) {
    QTranslator*
    t = new QTranslator(const_cast<Self *>(this));
    t->load(RC_TR_ZH_TW);
    tr_zh_TW_.append(t);

    if (!QString(RC_TR_ZH_TW).isEmpty()) {
      t = new QTranslator(const_cast<Self *>(this));
      t->load(QTRC_TR_ZH_TW);
      tr_zh_TW_.append(t);
    }
  }
  return tr_zh_TW_;
}

Localizer::QTranslatorList
Localizer::currentTranslators() const
{
  switch (language_) {
  case QLocale::English:   return tr_en();
  case QLocale::Japanese:  return tr_ja();
  case QLocale::Chinese:   return script_ == QLocale::SimplifiedChineseScript ? tr_zh_CN() : tr_zh_TW();
  default: return tr_en();
  }
}

void
Localizer::installCurrentTranslator(QCoreApplication *a)
{
  if (a)
    foreach (QTranslator *t, currentTranslators())
      a->installTranslator(t);
}

void
Localizer::removeCurrentTranslator(QCoreApplication *a)
{
  if (a)
    foreach (QTranslator *t, currentTranslators())
      a->removeTranslator(t);
}

// - Translations -

QString
Localizer::translate(int tid)
{
#define SELF(_t)      translate(_t)
  switch (tid) {
  case T_NULL:          return QString();

  case T_TIME:          return tr("Time");
  case T_DATE:          return tr("Date");
  case T_DEVICE:        return tr("Device");
  case T_PREVIEW:       return tr("Preview");
  case T_PIXEL:         return tr("Pixel");
  case T_MORE:          return tr("More");
  case T_LESS:          return tr("Less");
  case T_UPDATE:        return tr("Update");
  case T_DOWNLOAD:      return tr("Download");
  case T_INTERNET:      return tr("Internet");
  case T_WINDOWS:       return tr("Windows");
  case T_SOURCE:        return tr("Source");
  case T_LIBRARY:       return tr("Library");
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
  case T_WORDCOUNT:     return tr("Word Count");
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
  case T_FRENCH:        return tr("French");
  case T_GERMAN:        return tr("German");
  case T_ITALIAN:       return tr("Italian");
  case T_SPANISH:       return tr("Spanish");
  case T_PORTUGUESE:    return tr("Portuguese");
  case T_RUSSIAN:       return tr("Russian");

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

  case T_TITLE_OPENFILE:        return tr("Open Media File");
  case T_TITLE_OPENDEVICE:      return tr("Select Media Device");
  case T_TITLE_OPENVIDEODEVICE: return tr("Open Video Device");
  case T_TITLE_OPENAUDIODEVICE: return tr("Open Audio Device");
  case T_TITLE_OPENSUBTITLE:    return tr("Open Subtitle");
  case T_TITLE_COMMENTVIEW:     return tr("Comments");
  case T_TITLE_ANNOTANALYTICS:     return tr("Annotations Analytics");
  case T_TITLE_SIGNALVIEW:      return tr("Select Process Signal");
  case T_TITLE_LIVE:            return tr("Live Channel");
  case T_TITLE_SYNC:            return tr("Sync Mode");

  // Messages:

  case T_ERROR_UNKNOWN_COMMAND:         return tr("unrecognized command");
  case T_ERROR_HASH_TOKEN:              return tr("failed to digest media ID");
  case T_ERROR_BAD_USERNAME:            return tr("invalid username");
  case T_ERROR_BAD_PASSWORD:            return tr("invalid password");
  case T_ERROR_BAD_POS:                 return tr("invalid time format");
  case T_ERROR_BAD_TOKEN:               return tr("invalid media");
  case T_ERROR_BAD_DEVICEPATH:          return tr("invalid device path");
  case T_ERROR_BAD_FILEPATH:            return tr("file does not exist");
  case T_ERROR_BAD_SUBTITLE:            return tr("failed to load subtitle");
  case T_ERROR_CLIPBOARD_UNAVAILABLE:   return tr("clipboard not available");
  case T_ERROR_SUBMIT_TOKEN:            return tr("failed to query media");
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
  case T_FORMAT_PROGRAM:        return tr("Game");

  // Actions:

  case T_MENUTEXT_OPEN:         return tr("Open");
  case T_TIP_OPEN:              return tr("Open media file or game process");

  case T_MENUTEXT_OPENFILE:     return tr("Open File");
  case T_TIP_OPENFILE:          return tr("Open media file");

  case T_MENUTEXT_OPENURL:     return tr("Open Media URL");
  case T_TIP_OPENURL:          return tr("Open media from Internet or local URL");

  case T_MENUTEXT_OPENANNOTATIONURL:     return tr("Import annots from URL");
  case T_TIP_OPENANNOTATIONURL:          return tr("Add annotations from URL");

  case T_MENUTEXT_OPENDEVICE:   return tr("Open DVD/VCD/CD Device");
  case T_TIP_OPENDEVICE:        return tr("Open media device");

  case T_MENUTEXT_OPENVIDEODEVICE:   return tr("Open DVD/VCD Folder");
  case T_TIP_OPENVIDEODEVICE:        return tr("Open video folder");

  case T_MENUTEXT_OPENAUDIODEVICE:   return tr("Open CD Folder");
  case T_TIP_OPENAUDIODEVICE:        return tr("Open audio folder");

  case T_MENUTEXT_OPENSUBTITLE: return tr("Open Subtitle");
  case T_TIP_OPENSUBTITLE:      return tr("Load subtitle from file");

  case T_MENUTEXT_OPENCONTEXTMENU: return tr("Open");
  case T_TIP_OPENCONTEXTMENU:      return tr("Open");

  case T_OPENINWEBBROWSER:      return tr("Open in Web Browser");
  case T_DOWNLOADCURRENT:       return tr("Download Current Media");

  case T_MENUTEXT_OPENDIRECTORY:         return tr("Open Directory");

  case T_MENUTEXT_PLAY:         return tr("Play") + " [" K_SPACE "]";
  case T_TIP_PLAY:              return tr("Play media") + " [" K_SPACE "]";

  case T_MENUTEXT_FASTFORWARD:  return tr("Fast Forward") + " " "x4";
  case T_MENUTEXT_FASTFASTFORWARD: return tr("Fast forward") + " " "x16";

  case T_MENUTEXT_MENU:         return tr("Menu");
  case T_TIP_MENU:              return tr("Show menu");

  case T_MENUTEXT_PAUSE:        return tr("Pause") + " [" K_SPACE "]";
  case T_TIP_PAUSE:             return tr("Pause playing") + " [" K_SPACE "]";

  case T_MENUTEXT_STOP:         return tr("Stop");
  case T_TIP_STOP:              return tr("Stop playing");

  case T_MENUTEXT_NEXTFRAME:    return tr("Next Frame");
  case T_TIP_NEXTFRAME:         return tr("Next frame");

  case T_MENUTEXT_REPLAY:       return tr("Replay");
  case T_TIP_REPLAY:            return tr("Restart playing");

  case T_MENUTEXT_MINI:         return tr("Mini Player");// + " [" K_CMD "+2]";
  case T_TIP_MINI:              return tr("Show mini player") + " [" K_CMD "+2]";

  case T_MENUTEXT_EMBED:        return tr("Embed Player");// + " [" K_CMD "+1]";
  case T_TIP_EMBED:             return tr("Embed player window") + " [" K_CMD "+1]";

  case T_MENUTEXT_EMBEDONTOP:   return tr("Embed On Top");
  case T_TIP_EMBEDONTOP:        return tr("Embed player window on the top");

  case T_MENUTEXT_LIVE:         return tr("Live Channel") + " (" + tr("experimental") + ")";
  case T_TIP_LIVE:              return tr("Toggle live mode");

  case T_MENUTEXT_SYNC:         return tr("Sync Mode");
  case T_TIP_SYNC:              return tr("Toggle Sync mode");

  case T_MENUTEXT_FULLSCREEN:   return tr("Fullscreen");// + " [" K_CMD "+3]";
  case T_TIP_FULLSCREEN:        return tr("Show fullscreen") + " [" K_CMD "+3]";

  case T_MENUTEXT_SNAPSHOT:     return tr("Snapshot");
  case T_TIP_SNAPSHOT:          return tr("Taks a snapshot of current frame");

  case T_MENUTEXT_ANNOT:        return tr("Toggle Annotations");
  case T_TIP_ANNOT:             return tr("Toggle annotations");
  case T_MENUTEXT_SHOWANNOT:    return tr("Show Annotations");
  case T_TIP_SHOWANNOT:         return tr("Show Annotations");
  case T_MENUTEXT_HIDEANNOT:    return tr("Hide Annotations");
  case T_TIP_HIDEANNOT:         return tr("Hide annotations");

  case T_MENUTEXT_QUIT:         return tr("Exit");
  case T_TIP_QUIT:              return tr("Quit the application");

  case T_MENUTEXT_USER:         return tr("About Me");
  case T_TIP_USER:              return tr("Show user informatin");

  case T_MENUTEXT_SEEK:         return tr("Seek");
  case T_TIP_SEEK:              return tr("Seek position");

  case T_MENUTEXT_LOGIN:        return tr("Login");
  case T_TIP_LOGIN:             return tr("Show login dialog");

  case T_MENUTEXT_LOGOUT:       return tr("Logout");
  case T_TIP_LOGOUT:            return tr("Logout");

  case T_MENUTEXT_LOGINDIALOG:  return tr("Login");
  case T_TIP_LOGINDIALOG:       return tr("Show login dialog");

  case T_MENUTEXT_LIVEDIALOG:   return tr("Choose Live Time");
  case T_TIP_LIVEDIALOG:        return tr("Show live mode dialog");

  case T_MENUTEXT_SYNCDIALOG:   return tr("Choose Sync Time");
  case T_TIP_SYNCDIALOG:        return tr("Show sync mode dialog");

  case T_MENUTEXT_SEEKDIALOG:   return tr("Seek");
  case T_TIP_SEEKDIALOG:        return tr("Show seek dialog");

  case T_MENUTEXT_PREVIOUS:     return tr("Previous") + " [" K_CTRL "+" K_SHIFT "+←]";
  case T_TIP_PREVIOUS:          return tr("Previous") + " [" K_CTRL "+" K_SHIFT "+←]";
  case T_MENUTEXT_NEXT:         return tr("Next") + " [" K_CTRL "+" K_SHIFT "+→]";
  case T_TIP_NEXT:              return tr("Next") + " [" K_CTRL "+" K_SHIFT "+→]";

  case T_MENUTEXT_BLACKLIST:    return tr("Blacklist");// + " [" K_CTRL + "+F4]";
  case T_TIP_BLACKLIST:         return tr("Blacklist");

  case T_MENUTEXT_PICKDIALOG:   return tr("Pick Window");
  case T_TIP_PICKDIALOG:        return tr("Show pick dialog");

  case T_MENUTEXT_PROCESSPICKDIALOG:   return tr("Pick Game Window");
  case T_TIP_PROCESSPICKDIALOG:        return tr("Select process window");

  case T_MENUTEXT_WINDOWPICKDIALOG:   return tr("Pick Annotation Window");
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

  case T_MENUTEXT_ANNOTSUBTITLE: return tr("Annotation Subtitle");
  case T_TIP_ANNOTSUBTITLE:      return tr("Annotation as subtitle");

  case T_MENUTEXT_SECTION:      return tr("DVD Sections");
  case T_TIP_SECTION:           return tr("Select DVD sections");

  case T_MENUTEXT_SHOWSUBTITLE: return tr("Show Subtitle");
  case T_TIP_SHOWSUBTITLE:      return tr("Show subtitle");

  case T_MENUTEXT_HIDESUBTITLE: return tr("Hide Subtitle");
  case T_TIP_HIDESUBTITLE:      return tr("Hide subtitle");

  case T_MENUTEXT_SUBANNOT:     return tr("Show Sub Annotations");
  case T_TIP_SUBANNOT:          return tr("Show subtitle annot");

  case T_MENUTEXT_NONSUBANNOT:  return tr("Show Non-sub Annotations");
  case T_TIP_NONSUBANNOT:       return tr("Show non-sub annot");

  case T_MENUTEXT_REMOVEANNOTATION:   return tr("Hide");
  case T_TIP_REMOVEANNOTATION:        return tr("Hide");

  case T_MENUTEXT_ENABLEAUTOCLEARCONSOLE: return tr("Auto Clear Console");
  case T_TIP_ENABLEAUTOCLEARCONSOLE:      return tr("Enable auto clear console");

  case T_MENUTEXT_DISABLEAUTOCLEARCONSOLE: return tr("Stick Console");
  case T_TIP_DISABLEAUTOCLEARCONSOLE:      return tr("Disable auto clear console");

  case T_MENUTEXT_WINDOWSTAYSONTOP:   return tr("Always On Top"); // + " [" K_CTRL "+T]";
  case T_TIP_WINDOWSTAYSONTOP:        return tr("Show window on top");

  case T_MENUTEXT_ADVANCED:     return tr("Advanced");
  case T_TIP_ADVANCED:          return tr("Advanced menu");

  case T_MENUTEXT_BLESSUSER:    return tr("Bless User");
  case T_TIP_BLESSUSER:         return tr("Bless user");

  case T_MENUTEXT_CURSEUSER:    return tr("Curse User");
  case T_TIP_CURSEUSER:         return tr("Curse user");

  case T_MENUTEXT_BLOCKUSER:    return tr("Block User");
  case T_TIP_BLOCKUSER:         return tr("Block user");

  case T_MENUTEXT_EDITTHISANNOT:return tr("Edit Annotation");
  case T_TIP_EDITTHISANNOT:     return tr("Edit annotation");

  case T_MENUTEXT_DELETETHISANNOT:return tr("Delete Annotation");
  case T_TIP_DELETETHISANNOT:     return tr("Delete annotation");

  case T_MENUTEXT_BLOCKTHISANNOT:return tr("Block Annotation");
  case T_TIP_BLOCKTHISANNOT:    return tr("Block annotation");

  case T_MENUTEXT_BLESSTHISANNOT:return tr("Bless Annotation");
  case T_TIP_BLESSTHISANNOT:    return tr("Bless annotation");

  case T_MENUTEXT_CURSETHISANNOT:return tr("Curse Annotation");
  case T_TIP_CURSETHISANNOT:    return tr("Curse annotation");

  case T_MENUTEXT_BLESSTHISTOKEN:return tr("Bless Media");
  case T_TIP_BLESSTHISTOKEN:    return tr("Bless media");

  case T_MENUTEXT_CURSETHISTOKEN:return tr("Curse Media");
  case T_TIP_CURSETHISTOKEN:    return tr("Curse media");

  case T_MENUTEXT_HIDETHISANNOT:return tr("Hide Annotation");
  case T_TIP_HIDETHISANNOT:     return tr("Hide annotation");

  case T_MENUTEXT_SHOWTHISANNOT:return tr("Show Annotation");
  case T_TIP_SHOWTHISANNOT:     return tr("Show annotation");

  case T_MENUTEXT_COPYTHISANNOT:return tr("Copy Annotation");
  case T_TIP_COPYTHISANNOT:     return tr("Copy annotation");

  case T_MENUTEXT_VIEWTHISUSER: return tr("Display User");
  case T_TIP_VIEWTHISUSER:      return tr("Display user");

  case T_TITLE_SUBTITLEVIEW:    return tr("Subtitle History");
  case T_MENUTEXT_SUBTITLEVIEW: return tr("Subtitle History");
  case T_TIP_SUBTITLEVIEW:      return tr("Show subtitle history");

  case T_MENUTEXT_BLOCKTHISUSER:return tr("Block User");
  case T_TIP_BLOCKTHISUSER:     return tr("Block user");

  case T_MENUTEXT_CLEARCACHE:   return tr("Clear Offline Cache");
  case T_TIP_CLEARCACHE:        return tr("Remove offline cache");

  case T_MENUTEXT_CONNECT:      return tr("Synchronize with Server");
  case T_TIP_CONNECT:           return tr("Connect to server and synchronize local cache");
  case T_MENUTEXT_DISCONNECT:   return tr("Disconnect from Server");
  case T_TIP_DISCONNECT:        return tr("Disconnect from server and use local cache only");

  case T_MENUTEXT_ANNOTATIONEDITOR: return tr("Annotation Editor");// + " [" K_CTRL "+F1]";
  case T_TIP_ANNOTATIONEDITOR: return tr("Show annotation editor");

  case T_MENUTEXT_ANNOTATIONBROWSER: return tr("Search Annotations") + " [" K_CTRL "+F2]";
  case T_TIP_ANNOTATIONBROWSER: return tr("Show annotation browser");

  case T_MENUTEXT_TOKENVIEW:    return tr("Annotation Source") + " [" K_CTRL "+F3]";
  case T_TIP_TOKENVIEW:         return tr("Show media information");

  case T_MENUTEXT_COMMENTVIEW:  return tr("Comments");
  case T_TIP_COMMENTVIEW:       return tr("Show comment browser");

  case T_MENUTEXT_PROXYVIEW:    return tr("Browser");
  case T_TIP_PROXYVIEW:         return tr("Proxy web browser");

  case T_MENUTEXT_EDIT:         return tr("Edit");
  case T_TIP_EDIT:              return tr("Edit");
  case T_EDIT:                  return tr("Edit");

  case T_MENUTEXT_CLEAR:        return tr("Clear");
  case T_TIP_CLEAR:             return tr("Clear");

  case T_MENUTEXT_ADDALIAS:     return tr("Add Alias");
  case T_TIP_ADDALIAS:          return tr("Add alias");

  case T_MENUTEXT_SIGNALVIEW:     return tr("Select Game Process");
  case T_TIP_SIGNALVIEW:          return tr("Select process signal");

  case T_MENUTEXT_ATTACHPROCESS: return tr("Attach Process");
  case T_TIP_ATTACHPROCESS:     return tr("Attach process");

  case T_MENUTEXT_DETACHPROCESS: return tr("Detach Process");
  case T_TIP_DETACHPROCESS:     return tr("Detach process");

  case T_MENUTEXT_REFRESHPROCESS: return tr("Refresh Process");
  case T_TIP_REFRESHPROCESS:    return tr("Refresh process");

  case T_MENUTEXT_RECENTMESSAGES: return tr("Game Preferences");
  case T_TIP_RECENTMESSAGES:    return tr("Game Preferences");

  case T_MENUTEXT_PLAYLIST:     return tr("Playlist");
  case T_TIP_PLAYLIST:          return tr("Playlist");

  case T_MENUTEXT_APPLANGUAGE:  return tr("Application Language");
  case T_TIP_APPLANGUAGE:       return tr("Choose application language");

  case T_ANYLANGUAGE:           return tr("Any Language");
  case T_MENUTEXT_ANYLANGUAGE:  return tr("All");
  case T_TIP_ANYLANGUAGE:       return tr("Choose all languages");

  case T_MENUTEXT_ANNOTATIONLANGUAGE:  return tr("Annotation Languages");
  case T_TIP_ANNOTATIONLANGUAGE:       return tr("Choose annotation language");

  case T_MENUTEXT_USERLANGUAGE:  return tr("User Language");
  case T_TIP_USERLANGUAGE:       return tr("Choose user language");

  case T_MENUTEXT_ANONYMOUS:  return tr("Anonymous");
  case T_TIP_ANONYMOUS:       return tr("User anonymous");

  case T_MENUTEXT_TRANSLATE:  return tr("Show Translation");
  case T_TIP_TRANSLATE:       return tr("Display translation");

  case T_MENUTEXT_SUBTITLEONTOP:  return tr("Subtitle On Top");
  case T_TIP_SUBTITLEONTOP:       return tr("Display subtitle on the top/bottom");

  case T_MENUTEXT_SUBTITLESTYLE:  return tr("Subtitle Color");
  case T_TIP_SUBTITLESTYLE:       return tr("Select subtitle color");

  case T_MENUTEXT_THEME:  return tr("Theme");
  case T_TIP_THEME:       return tr("Select GUI theme");

  case T_LABEL_CREATEDATE:      return tr("Create Date");
  case T_TIP_CREATEDATE:        return tr("Create Date");
  case T_LABEL_UPDATEDATE:      return tr("Update Date");
  case T_TIP_UPDATEDATE:        return tr("Update Date");
  case T_LABEL_BLESSCOUNT:      return tr("Blessed");
  case T_TIP_BLESSCOUNT:        return tr("Bless Count");
  case T_LABEL_CURSECOUNT:      return tr("Cursed");
  case T_TIP_CURSECOUNT:        return tr("Curse Count");
  case T_LABEL_BLOCKCOUNT:      return tr("Blocked");
  case T_TIP_BLOCKCOUNT:        return tr("Block Count");
  case T_LABEL_VISITCOUNT:      return tr("Visited");
  case T_TIP_VISITCOUNT:        return tr("Visit Count");
  case T_LABEL_ANNOTCOUNT:      return tr("Annot");
  case T_TIP_ANNOTCOUNT:        return tr("Annotation Count");

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
  case T_TIP_RECENT:            return tr("Recent File");
  case T_MENUTEXT_CLEARRECENT:  return tr("Clear");
  case T_TIP_CLEARRECENT:       return tr("Clear recent files");

  case T_MENUTEXT_MINIMIZE:     return tr("Minimize and Pause") + " [" K_CTRL "+" K_ESC "]";
  case T_TIP_MINIMIZE:          return tr("Minimize and pause") + " [" K_CTRL "+" K_ESC "]";

  case T_MENUTEXT_ANNOTATIONLIMIT: return tr("Annotation Count");
  case T_TIP_ANNOTATIONLIMIT:      return tr("Hint maximum annotation count");

  case T_MENUTEXT_BROWSE:       return tr("Browse");
  case T_TIP_BROWSE:            return tr("Files in the same folder");

  case T_MENUTEXT_TRACK:        return tr("Tracks");
  case T_TIP_TRACK:             return tr("Media tracks");

  case T_MENUTEXT_AUTOPLAYNEXT: return tr("Auto Play Next Media");
  case T_TIP_AUTOPLAYNEXT:      return tr("Automatically play next media");

  case T_AUTOPLAYCURRENT:       return tr("Repeat Current Media");

  case T_MENUTEXT_DOCK_TOP:     return tr("Dock to the Top");
  case T_TOOLTIP_DOCK_TOP:      return tr("Dock to the top");

  case T_MENUTEXT_DOCK_BOTTOM:  return tr("Dock to the Bottom");
  case T_TOOLTIP_DOCK_BOTTOM:   return tr("Dock to the bottom");

  case T_NOAUTOPLAY:            return tr("No Repeat");

  case T_MENUTEXT_SHOWMENUBAR:  return tr("Show Menubar") + " [" K_CAPSLOCK "]";
  case T_TIP_SHOWMENUBAR:       return tr("Show menubar");

  case T_MENUTEXT_ENABLEAERO:   return tr("Enable Aero");
  case T_MENUTEXT_DISABLEAERO:  return tr("Disable Aero");

  case T_MENUTEXT_CHECKINTERNET:return tr("Check Internet Connection");
  case T_TIP_CHECKINTERNET:     return tr("Check Internet connection");

  case T_MENUTEXT_DELETECACHE:  return tr("Remove Caches");
  case T_TIP_DELETECACHE:       return tr("Remove offline caches");

  case T_MENUTEXT_SITEACCOUNT:  return tr("Link Accounts");
  case T_TIP_SITEACCOUNT:       return tr("Set accounts for websites");

  case T_MENUTEXT_ENABLEBLACKLIST:  return tr("Enable Blacklist");
  case T_TIP_ENABLEBLACKLIST:       return tr("Enable blacklist");

  case T_MENUTEXT_DOWNLOAD:  return tr("Download") + " [" K_CTRL "+D]";
  case T_TIP_DOWNLOAD:       return tr("Download");// + " [" K_CTRL "+D]";

  case T_MENUTEXT_ANNOTANALYTICS:  return tr("Annotation Analytics");// + " [" K_CTRL "+F5]";
  case T_TIP_ANNOTANALYTICS:       return tr("Show annotations as thread");

  case T_MENUTEXT_SAVEMEDIA:    return tr("Save Buffered Video");
  case T_TIP_SAVEMEDIA:         return tr("Save Buffered Video");

  case T_MENUTEXT_AUTOSAVEMEDIA:  return tr("Save Buffered Video");
  case T_TIP_AUTOSAVEMEDIA:       return tr("Save Buffered Video");

  case T_MENUTEXT_MONITORCLIPBOARD:  return tr("Monitor Clipboard");

  case T_MENUTEXT_NOTHINGAFTERFINISHED: return tr("Nothing");
  case T_MENUTEXT_SHUTDOWNAFTERFINISHED: return tr("Shutdown");
  case T_MENUTEXT_SLEEPAFTERFINISHED: return tr("Sleep");

  case T_MENUTEXT_ASPECTRATIOSTANDARD: return tr("Standard") + " (4:3)";
  case T_MENUTEXT_ASPECTRATIOWIDESCREEN: return tr("Wide screen") + " (16:9)";

  case T_MENUTEXT_MULTIWINDOW: return tr("Allow Multiple Windows");

  case T_MENUTEXT_RESUMEANNOT:  return tr("Release Annotations");

  case T_FILTER_PATTERN:        return tr("Filter Pattern");
  case T_FILTER_SYNTAX:         return tr("Filter Syntax");
  case T_FILTER_COLUMN:         return tr("Filter Column");
  case T_FILTER_REGEX:          return tr("Regular Expression");
  case T_FILTER_WILDCARD:       return tr("Wildcard");
  case T_FILTER_FIXED:          return tr("Fixed String");

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
  case T_CREATETIME:    return tr("Create Time");
  case T_LOGINTIME:     return tr("Login Time");
  case T_UPDATETIME:    return tr("Update Time");
  case T_LOGINIP:       return tr("Login IP");
  case T_LOGINCOUNT:    return tr("Login Count");
  case T_LANGUAGE:      return tr("Language");
  case T_STATUS:        return tr("Status");
  case T_FLAGS:         return tr("Flags");
  case T_BLESSCOUNT:    return tr("Bless Count");
  case T_CURSECOUNT:    return tr("Curse Count");
  case T_BLOCKCOUNT:    return tr("Block Count");
  case T_ANNOTCOUNT:    return tr("Annot Count");
  case T_VISITCOUNT:    return tr("Visit Count");

  case T_PID:           return tr("PID");
  case T_FOLDER:        return tr("Folder");
  case T_FILE:          return tr("File");
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

  case T_NETWORKPROXY:  return tr("Network Proxy");
  case T_MENUTHEME:     return tr("Render Context Menu");

  case T_URL_NICOVIDEO: return tr("http://nicovideo.jp");
  case T_URL_BILIBILI:  return tr("http://bilibili.tv");

  case T_ASPECTRATIO:   return tr("Aspect ratio");
  case T_NEWWINDOW:     return tr("New window"); // + " [" K_CTRL "+" K_SHIFT "+" "N]";

  case T_MENUTEXT_LABELPLAYER: return tr("Label Player with Annotations");
  case T_MENUTEXT_AUTOSUBMIT:  return tr("Save Annotation Associations");
  case T_MENUTEXT_BACKLOG:     return tr("Backlog");// + " [" K_CTRL "+F6]";
  case T_MENUTEXT_PREFERLOCALDB:     return tr("Prefer Offline Annotations");

  case T_MENUTEXT_RESETANNOTSCALE:  return tr("Reset Scale") + " [" K_CTRL "+" + tr("Mid") + "]";
  case T_TIP_RESETANNOTSCALE:       return tr("Reset annotation scale");

  case T_MENUTEXT_INCREASEANNOTSCALE:  return tr("Scale Up") + " [" K_CTRL "+" + tr("Wheel") + "↑]" " [" + tr("LButton") + "+" + tr("Wheel") + "↑]";
  case T_TIP_INCREASEANNOTSCALE:       return tr("Scale up");

  case T_MENUTEXT_DECREASEANNOTSCALE:  return tr("Scale Down") + " [" K_CTRL "+" + tr("Wheel") + "↓]" " [" + tr("LButton") + "+" + tr("Wheel") + "↓]";
  case T_TIP_DECREASEANNOTSCALE:       return tr("Scale down");

  case T_MENUTEXT_RESETANNOTOFFSET:  return tr("Reset Offset") + " [" K_CTRL "+" K_ALT "+" + tr("Mid") + "]";
  case T_TIP_RESETANNOTOFFSET:       return tr("Reset annotation offset in time");

  case T_MENUTEXT_INCREASEANNOTOFFSET:  return tr("Increase Offset") + " [" K_CTRL "+" K_ALT "+" + tr("Wheel") + "↑]";
  case T_TIP_INCREASEANNOTOFFSET:       return tr("Increase offset");

  case T_MENUTEXT_DECREASEANNOTOFFSET:  return tr("Decrease Offset") + " [" K_CTRL "+" K_ALT "+" + tr("Wheel") + "↓]";
  case T_TIP_DECREASEANNOTOFFSET:       return tr("Decrease offset");

  case T_MENUTEXT_RESETANNOTROTATION:  return tr("Reset Rotation") + " [" K_SHIFT "+" + tr("Mid") + "]";
  case T_TIP_RESETANNOTROTATION:       return tr("Reset annotation rotation");

  case T_MENUTEXT_INCREASEANNOTROTATION:  return tr("Rotate Up") + " [" K_SHIFT "+" + tr("Wheel") + "↑]";
  case T_TIP_INCREASEANNOTROTATION:       return tr("Rotate up");

  case T_MENUTEXT_DECREASEANNOTROTATION:  return tr("Rotate Down") + " [" K_SHIFT "+" + tr("Wheel") + "↓]";
  case T_TIP_DECREASEANNOTROTATION:       return tr("Rotate down");

  case T_COPYCURRENTURL:       return tr("Copy URL");
  case T_COPYCURRENTTITLE:     return tr("Copy Title");
  case T_ACTUALSIZE:       return tr("Actual Size");

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

  case T_EXPERIMENTAL:  return tr("experimental");

  default:
    qWarning() << "Localizer:translate: Unknown tid =" << tid;
    Q_ASSERT(0);
    return QString();
  }
#undef SELF
}

// EOF
