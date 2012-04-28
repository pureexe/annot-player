// annotcloud/annothtml_unparse.cc
// 8/19/2011

#include "module/annotcloud/annothtml.h"
#include "module/annotcloud/annottag.h"
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>

//#define DEBUG "annothtmlunparse"
#include "module/debug/debug.h"

QString
AnnotCloud::
AnnotationHtmlParser::reduceHtml(const QString &html) const
{
  DOUT("enter: input html follows");
  DOUT(html);
  if (html.trimmed().isEmpty())
    return "";

  QWebPage page;
  QWebFrame *frame = page.mainFrame();
  frame->setHtml(html);

  QWebElement document = frame->documentElement();
  QString ret = reduceHtmlElement(document);
  while (!ret.isEmpty() && ret.endsWith("\\n "))
    ret.chop(3);
  return ret;
}

QString
AnnotCloud::
AnnotationHtmlParser::reduceHtmlElement(const QWebElement &input) const
{
#define SELF(_e) reduceHtmlElement(_e)

#define IFTAG(_tag, _html)      if (!_tag.compare(_html, Qt::CaseInsensitive))
#define IGNORE

  QString ret;
  QWebElement e = input;
  while (!e.isNull()) {

    QString t = e.tagName();
    QString tail;

    IFTAG(t, "html")
      IGNORE;
    else IFTAG(t, "head") {
      e = e.nextSibling();
      continue;
    }
    else IFTAG(t, "body") {
      // Fix HTML siblings
      // See: http://developer.qt.nokia.com/forums/viewthread/7262/
      // if not <html> and not <head>, fix inner html
      QString innerHtml = e.toInnerXml();
      if (!innerHtml.isEmpty() && !e.firstChild().isNull()) {
        innerHtml.replace("<", "<<")
                 .replace(">", ">>")
                 .replace("<<", "</span><")
                 .replace(">>", "><span>");
        innerHtml = "<span>" + innerHtml + "</span>";
        e.setInnerXml(innerHtml);
      }
      DOUT("new html body follows");
      DOUT(innerHtml);

    } else IFTAG(t, "p")
      tail = "\\n ";
    else IFTAG(t, "br") {
      ret.append("\\n ");
      e = e.nextSibling();
      continue;
    } else IFTAG(t, "span") {

      // color

      QString color = e.styleProperty("color", QWebElement::InlineStyle);
      if (!color.isEmpty()) {
        QColor c = parseRGBColor(color);
        if (c.isValid()) {
          QRgb rgb = c.rgb();
#define IFCOLOR(_c)      if (rgb == QColor(#_c).rgb()) ret.append("\\" #_c "{");
          IFCOLOR(black)
          else IFCOLOR(blue)
          else IFCOLOR(brown)
          else IFCOLOR(cyan)
          else IFCOLOR(green)
          else IFCOLOR(gray)
          else IFCOLOR(magenta)
          else IFCOLOR(orange)
          else IFCOLOR(pink)
          else IFCOLOR(red)
          else IFCOLOR(white)
          else IFCOLOR(yellow)
#undef IFCOLOR
          else ret.append(QString("\\color[#%1]{").arg(QString::number(rgb, 16)));
        } else ret.append(QString("\\color[%1]{").arg(color));

        tail.prepend("}");
      }

      // background-color
      QString bg = e.styleProperty("background-color", QWebElement::InlineStyle);
      if (!bg.isEmpty()) {
        QColor c = parseRGBColor(bg);
        if (c.isValid())
          ret.append(QString("\\background[#%1]{").arg(QString::number(c.rgb(), 16)));
        else
          ret.append(QString("\\background[%1]{").arg(bg));

        tail.prepend("}");
      }

      // text-decoration
      QString textdec = e.styleProperty("text-decoration", QWebElement::InlineStyle);
      if (!textdec.isEmpty()) {
#define IFTEXTDEC(_dec, _tag) if (!textdec.compare(_dec, Qt::CaseInsensitive)) { ret.append(_tag "{"); tail.prepend("}"); }
        IFTEXTDEC("underline", CORE_CMD_LATEX_ULINE)
        else IFTEXTDEC("line-through", CORE_CMD_STYLE_STRIKE)
#undef IFTEXTDEC
      }

      // font-stype
      QString fontstyle = e.styleProperty("text-style", QWebElement::InlineStyle);
      if (!fontstyle.isEmpty()) {
#define IFFONTSTYLE(_style, _tag) if (!fontstyle.compare(_style, Qt::CaseInsensitive)) { ret.append(_tag "{"); tail.prepend("}"); }
        IFFONTSTYLE("italic", "\\em")
#undef IFFONTSTYLE
      }

      QString fontweight = e.styleProperty("font-weight", QWebElement::InlineStyle);
      if (!fontweight.isEmpty()) {
#define IFFONTWEIGHT(_w, _tag) if (fontweight == #_w) { ret.append(_tag "{"); tail.prepend("}"); }
        // 400 is normal
        IFFONTWEIGHT(600, "\\bf") // 600 is bold
#undef IFFONTWEIGHT
      }

      QString fontsize = e.styleProperty("font-size", QWebElement::InlineStyle);
      if (!fontsize.isEmpty()) {
        // TODO: use big, small, large, etc here rather than embedding the font!!!
        ret.append("\\size" "[" + fontsize + "]" "{");
        tail.prepend("}");
      }
    }

    QWebElement c = e.firstChild();
    if (c.isNull())
      ret.append(e.toPlainText());
    else {
      QString c_str = SELF(c);
      if (!c_str.isEmpty())
        ret.append(c_str);
    }

    if (!tail.isEmpty())
      ret.append(tail);
    e = e.nextSibling();
  }
  return ret;
#undef IGNORE
#undef IFTAG
#undef SELF
}

// - Helpers -

QColor
AnnotCloud::
AnnotationHtmlParser::parseRGBColor(const QString &rgb)
{
  QColor ret;
  QRegExp regex("\\s*rgb\\s*\\(\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*\\)\\s*");
  if (regex.exactMatch(rgb)) {
    bool ok;
    int r = regex.cap(1).toInt(&ok); if (!ok) return ret;
    int g = regex.cap(2).toInt(&ok); if (!ok) return ret;
    int b = regex.cap(3).toInt(&ok); if (!ok) return ret;

    ret = QColor::fromRgb(r, g, b);
  }
  return ret;
}

// EOF
