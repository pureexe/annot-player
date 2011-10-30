#ifndef STYLESHEET_H
#define STYLESHEET_H
// stylesheet.h
// 7/10/2011

#define SS_BEGIN(id)    #id "{"
#define SS_END          "}"

#define SS_TRANSPARENT  "background:transparent;"
#define SS_BORDERLESS   "border-top:0;"

#define SS_LINEEDIT \
  SS_BEGIN(QLineEdit) \
    "border-image:url(\":/image/lineedit.png\");" \
    "border-width:4;" \
    "color:black;" \
  SS_END

// Is tabwidget broken???
#define SS_TABWIDGET \
  SS_BEGIN(QTabWidget::tab-bar) \
     "top:4px;" \
  SS_END \
\
  SS_BEGIN(QTabWidget::pane) \
    "border-image:url(\":/image/tabframe.png\")" \
    "border-width:4;" \
    /*"border-top:8px;"*/ \
  SS_END

#define SS_TABBAR \
  SS_BEGIN(QTabBar::tab) \
     "border-image:url(\":/image/tab.png\");" \
     "border-width:4px;" \
     "padding-left:3px;" \
     "min-width:54ex;" \
     "color: black;" \
  SS_END \
\
  "QTabBar::tab:!selected" "{" \
     "margin-top:3px;" \
     "margin-bottom: 5px;" \
  "}" \
\
  "QTabBar::tab:selected" "{" \
    "border-image:url(\":/image/tab-selected.png\");" \
    "margin-bottom:3px;" \
  "}" \
\
  "QTabBar::tab:last" "{" \
    "border-image:url(\":/image/tab-last.png\");" \
  "}" \
\
  "QTabBar::tab:selected:only-one" "{" \
    "border-image:url(\":/image/tab-last-selected.png\");" \
    "margin-bottom:3px;" \
  "}" \
\
  "QTabBar::tab:selected:last" "{" \
    "border-image:url(\":/image/tab-last-selected.png\");" \
    "margin-bottom: 3px;" \
  "}" \
\
  "QTabBar::tab:!selected:last:hover" "{" \
    "border-image:url(\":/image/tab-last-hover.png\");" \
  "}" \
\
  "QTabBar::tab:hover:!selected" "{" \
    "border-image: url(\":/image/tab-hover.png\");" \
  "}" \
\
  "QTabBar::close-button" "{" \
    "image:url(\":/image/closebutton.png\");" \
  "}" \
\
  "QTabBar::close-button:hover:!pressed" "{" \
    "image:url(\":/image/closebutton_hover.png\");" \
  "}" \
\
  "QTabBar::close-button:pressed" "{" \
    "image:url(\":/image/closebutton_pressed.png\");" \
  "}"

#define SS_TOOLBUTTON \
  "QToolButton" "{" \
    "border-image:url(\":/image/button.png\");" \
    "border-width:3;" \
    "min-width:22px;" \
  "}" \
\
  "QToolButton::hover" "{" \
    "border-image:url(\":/image/button-hover.png\");" \
  "}" \
\
  "QToolButton::pressed" "{" \
     "border-image:url(\":/image/button-pressed.png\");" \
  "}"


#define SS_LEFT_TOOLBUTTON \
  "QToolButton" "{" \
    "border-image:url(\":/image/button-left.png\");" \
  "}" \
\
  "QToolButton:hover" "{" \
    "border-image:url(\":/image/button-left-hover.png\");" \
  "}"

#define SS_RIGHT_TOOLBUTTON \
  "QToolButton" "{" \
    "border-image:url(\":/image/button-right.png\");" \
  "}" \
\
  "QToolButton:hover" "{" \
    "border-image:url(\":/image/button-right-hover.png\");" \
  "}"

#endif // STYLESHEET_H
