#ifdef USE_MODULE_WEBBROWSER
// cloudview.cc
// 7/15/2011

#include "cloudview.h"
#include "defines.h"
#include "uistyle.h"
#include "tr.h"
#include <QtGui>
#include <QtWebKit>

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

#define TEXT_SIZE_SCALE 0.9

CloudView::CloudView(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(TR(T_TITLE_CLOUDVIEW));
  setWindowFlags(WINDOW_FLAGS);
  setTextSizeMultiplier(TEXT_SIZE_SCALE);

#ifdef Q_WS_MAC
  // void setContentsMargins(int left, int top, int right, int bottom);
  setContentsMargins(4, 2, 4, 2);
#endif // Q_WS_MAC

  setHomePage(G_STARTPAGE_URL);

  UiStyle::globalInstance()->setWindowStyle(this);
  setWindowOpacity(1.0); // window is opaque
}

#endif // USE_MODULE_WEBBROWSER
// EOF

/*
void
CloudView::setVisible(bool visible)
{
  if (visible && tabCount() == 0)
    openUrl(homePage());

  Base::setVisible(visible);
}
*/
