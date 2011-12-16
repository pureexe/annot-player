// annotationparser.cc
// 8/19/2011

#include "core/annotationparser.h"
#include "core/htmltag.h"
#include "core/cmd.h"
#include <QtCore>
#include <QtWebKit>
#include <boost/tuple/tuple.hpp>
#include <boost/typeof/typeof.hpp>

//#define DEBUG "core::annotationparser"
#include "module/debug/debug.h"

#define DEFAULT_TINY_SIZE        "10px"
#define DEFAULT_SMALL_SIZE       "15px"
#define DEFAULT_NORMAL_SIZE      "20px"
#define DEFAULT_LARGE_SIZE       "40px"
#define DEFAULT_HUGE_SIZE        "80px"

// - Constructions -

namespace Core {

  // - Constructions -

  //namespace { AnnotationParser defaultAnnotationParser; }
  //AnnotationParser *AnnotationParser::global_ = &defaultAnnotationParser;

  //void
  //AnnotationParser::setGlobalInstance(Self *global)
  //{ global_ = global; }


  AnnotationParser*
  AnnotationParser::globalInstance()
  { static Self g; return &g; }

  AnnotationParser::AnnotationParser()
    : tinySize_(DEFAULT_TINY_SIZE),
      smallSize_(DEFAULT_SMALL_SIZE),
      normalSize_(DEFAULT_NORMAL_SIZE),
      largeSize_(DEFAULT_LARGE_SIZE),
      hugeSize_(DEFAULT_HUGE_SIZE)
  { }

  // - Lexical parsing -

  // Regexp is not used to improve performance.
  std::pair<QString, QString>
  AnnotationParser::parseLeadingTag(const QString &text)
  {
    if (text.isEmpty() || text[0] != CORE_CMDCH)
      return std::make_pair(QString(), QString());

    int index = text.indexOf(' ');
    if (index < 0)
      return std::make_pair(text, QString());
    else
      return std::make_pair(text.mid(0, index), text.mid(index + 1));
  }

  std::pair<QString, QString>
  AnnotationParser::parseNextToken(const QString &text)
  {
  #define SELF(_text) parseNextToken(_text)
    typedef std::pair<QString, QString> RETURN;

    if (text.isEmpty())
      return RETURN();

    if (text.size() == 1)
      return RETURN(text, QString());

    char ch0 = text[0].toLatin1();
    switch (ch0) {
    case ' ':
      return SELF(text.trimmed());
    case '{': case '}':
    case '[': case ']':
      return RETURN(text[0], text.mid(1));
    }

    int mid = 0;
    for (int i = 1; !mid && i < text.size(); i++) {
      switch (text[i].toLatin1()) {
      case ' ':
        if (ch0 == CORE_CMDCH)
          mid = i;
        break;

      case CORE_CMDCH:
      case '{': case '}':
      case '[': case ']':
        if (ch0 == CORE_CMDCH && i == 1)
          mid = i + 1;
        else
          mid = i;
        break;
      }
    }
    if (!mid)
      return RETURN(text, QString());
    else
      return RETURN(text.mid(0, mid), text.mid(mid).trimmed());
  #undef SELF
  }

  // - Syntax parsing -

  std::pair<QString, QStringList>
  AnnotationParser::renderToHtml(const QString &text) const
  {
    DOUT("enter: text=" << text);

    typedef std::pair<QString, QStringList> RETURN;
    if (text.isEmpty()) {
      DOUT("exit");
      return RETURN();
    }

    QString parsed = "";
    QString unparsed = text;

    QStringList tags;

    enum StringType {   // actually not used
      ST_None = '\0',
      ST_Tag = CORE_CMDCH,
      ST_LeftCurlyBracket = '{',
      ST_RightCurlyBracket = '}',
      ST_LeftSquareBracket = '[',
      ST_RightSquareBracket = ']' };

    QList<std::pair<QString, char> > stack; // pair<QString, StringType>

    if (text.startsWith(CORE_CMD_VERBATIM)) {
      parsed = text.mid(QString(CORE_CMD_VERBATIM).size());
      tags.prepend(CORE_CMD_VERBATIM);
      DOUT("exit: parsed (verbatim) =" << parsed);
      return RETURN(parsed.trimmed(), tags);
    }
    if (text.startsWith(CORE_CMD_V " ")) { // \verbatim alias
      parsed = text.mid(QString(CORE_CMD_V " ").size());
      tags.prepend(CORE_CMD_V);
      DOUT("exit: parsed (verbatim) =" << parsed);
      return RETURN(parsed.trimmed(), tags);
    }

    do {
      QString current;
      boost::tie(current, unparsed) = parseNextToken(unparsed);
      if (current.isEmpty()) {
        // i.e. EOL
        // REDUCE until stack empty

        QStringList params, attrs;
        while (!stack.empty()) {
          BOOST_AUTO(front, stack.takeFirst());

          switch (front.second) {

          case '}':
            params.push_front(front.first);
            break;
          case ']':
            attrs.push_front(front.first);
            break;

          case CORE_CMDCH: {
              if (!parsed.isEmpty())
                params.push_back(parsed);

              QString t = translate(front.first, params, attrs);
              if (!t.isNull()) {
                parsed = t;
              } else {
                tags.prepend(front.first);
                parsed = params.join("");
                if (!attrs.empty())
                  DOUT("attributes of unknown tag ignored:" << front.first);
              }

              params.clear();
              if (!attrs.empty())
                attrs.clear();
            } break;

          case '{':
          case '[':
            qWarning() << "AnnotationParser:parse: Unparsed parentheses on the stack";
          case '\0':
            // Allow post-fixed attributes
            //if (!attrs.empty())
            //  attrs.clear();
            if (!params.empty()) {
              parsed = params.join("") + parsed;
              params.clear();
            }
            parsed = front.first + parsed;
            break;

          default:
            Q_ASSERT(0);
          }
        }
        if (!params.empty())
          parsed = params.join("") + parsed;
        break;
      }

      char current_ch = current[0].toLatin1();
      switch (current_ch) {
      case '}':
        if (stack.empty()) {
          DOUT("exit: '{}' mismatched");
          return RETURN();
        } else {
          // REDUCE until '{'
          QString reduced = "";
          QStringList params, attrs;
          if (stack.empty())
            DOUT("parse: '^]' mismatched");
          while (!stack.empty()) {
            BOOST_AUTO(front, stack.takeFirst());

            switch (front.second) {
            case '{':
              break;
            case '[':
              DOUT("parsed: '[{' mismatched");
              break;

            case '}':
              params.push_front(front.first);
              break;
            case ']':
              attrs.push_front(front.first);
              break;

            case CORE_CMDCH: {
              if (!reduced.isEmpty())
                params.push_back(reduced);
              QString t = translate(front.first, params, attrs);
              if (!t.isNull()) {
                reduced = t;
              } else {
                tags.prepend(front.first);
                reduced = params.join("");
                if (!attrs.empty())
                  DOUT("attributes of unknown tag ignored:" << front.first);
              }
              params.clear();
              if (!attrs.empty())
                attrs.clear();
            } break;

            case '\0':
              // Allow post-fixed attributes
              //if (!attrs.empty())
              //  attrs.clear();
              if (!params.empty()) {
                reduced = params.join("") + reduced;
                params.clear();
              }
              reduced = front.first + reduced;
              break;

            default:
              Q_ASSERT(0);
            }

            if (front.second == '{')
              break;
          }

          if (!params.empty())
            reduced = params.join("") + reduced;
          if (!reduced.isEmpty())
            stack.push_front(std::make_pair(reduced, '}'));

          DOUT("reduced {}:" << reduced);
        } break;

      case ']':
        if (stack.empty()) {
          DOUT("exit: '[]' mismatched");
          return RETURN();
        } else {
          // REDUCE until '['
          QString reduced = "";
          if (stack.empty())
            DOUT("parse: '^]' mismatched");
          while (!stack.empty()) {
            BOOST_AUTO(front, stack.takeFirst());

            switch (front.second) {
            case '[':
              break;
            default:
              reduced = front.first + reduced;
            }

            if (front.second == '[')
              break;
          }

          if (!reduced.isEmpty())
            stack.push_front(std::make_pair(reduced, ']'));

          DOUT("reduced []:" << reduced);
        } break;


      case '{':
      case '[':
        stack.push_front(std::make_pair(current, current_ch));
        break;

      case CORE_CMDCH:
        if (current.size() == 2) {
          char ch1 = current[1].toLatin1();
          switch (ch1) {
          case CORE_CMDCH:
          case '{': case '}':
          case '[': case ']':
            current = ch1;
            current_ch = '\0';
            break;
          }
        }

        stack.push_front(std::make_pair(current, current_ch));
        break;

      default:
        stack.push_front(std::make_pair(current, '\0'));
      }

    } while (!stack.empty());

    DOUT("exit: parsed =" << parsed);
    return RETURN(parsed, tags);
  }

  QString
  AnnotationParser::translate(const QString &tag,
                              const QStringList &params,
                              const QStringList &attrs) const
  {
    DOUT("enter: tag =" << tag);
    #ifdef DEBUG
    {
      qDebug() << "- attrs:";
      foreach (const QString &attr, attrs)
        qDebug() << attr;
      qDebug() << "- params:";
      foreach (const QString &param, params)
        qDebug() << param;
    }
    #endif // DEBUG

    if (tag.isEmpty())
      return QString();

    switch (qHash(tag)) {
    case H_Verbatim:
      return params.empty() ? "" : params.join("");

    case H_N:
    case H_Br:
    case H_LineBreak:
      return params.empty() ? core_html_br()
      : core_html_br() + params.join("");

#define CASE_COLOR(_hash, _csstag) \
    case _hash: \
      if (params.empty()) \
        return ""; \
      else { \
        if (attrs.empty()) { \
          if (params.size() == 1) \
            return params.front(); \
          else { \
            QString ret; \
            int i = 0; \
            foreach (const QString &param, params) { \
              switch (i) { \
              case 0: ret = core_html_style_open(_csstag ":" + param); break; \
              case 1: ret += param + core_html_style_close(); break; \
              default: ret += param; \
              } \
              i++; \
            } \
            return ret; \
          } \
        } else { \
          QString ret; \
          QString color = attrs.front(); \
          int i = 0; \
          foreach (const QString &param, params) { \
            if (i == 0) \
              ret = core_html_style(param, _csstag ":" + color); \
            else \
              ret += param; \
            i++; \
          } \
          return ret; \
        } \
      } \
      Q_ASSERT(0); \
      break;

      CASE_COLOR(H_Color, "color")
      CASE_COLOR(H_ForegroundColor, "color")
      CASE_COLOR(H_BackgroundColor, "background-color")
#undef CASE_COLOR

    case H_Repeat:
      if (params.empty())
        return "";
      else {
        if (attrs.empty()) {
          if (params.size() == 1)
            return params.front();
          else {
            QString ret;
            int count = 0;
            bool ok;
            int i = 0;
            foreach (const QString &param, params) {
              switch (i) {
              case 0:
                count = param.toInt(&ok);
                if (!ok)
                  return "";
                break;
              case 1:
                ret = param.repeated(count);
              default:
                ret += param;
              }
              i++;
            }
            return ret;
          }

        } else {
          QString s_count = attrs.front();
          bool ok;
          int count = s_count.toInt(&ok);
          if (!ok) {
            qDebug() << "Invalid count format";
            return "";
          }
          QString ret;
          int i = 0;
          foreach (const QString &param, params) {
            if (i == 0)
              ret = param.repeated(count);
            else
              ret += param;
            i++;
          }
          return ret;
        }
      }
      Q_ASSERT(0);
      break;

    case H_Style:
      if (params.empty())
        return "";
      else {
        if (attrs.empty()) {
          if (params.size() == 1)
            return params.front();
          else {
            QString ret;
            int i = 0;
            foreach (const QString &param, params) {
              switch (i) {
              case 0: {
                  QString style = param;
                  style.replace("=", ":");
                  style.replace(",", ";");
                  ret = core_html_style_open(style);
                } break;
              case 1:
                ret += param + core_html_style_close();
                break;
              default:
                ret += param;
              }
              i++;
            }
            return ret;
          }

        } else {
          QString ret;
          QString style = attrs.join(";");
          style.replace("=", ":");
          style.replace(",", ";");
          int i = 0;
          foreach (const QString &param, params) {
            if (i == 0)
              ret = core_html_style(param, style);
            else
              ret += param;
            i++;
          }
          return ret;
        }
      }
      Q_ASSERT(0);
      break;

    case H_Font:
      if (params.empty())
        return "";
      else {
        if (attrs.empty()) {
          if (params.size() == 1)
            return params.front();
          else {
            QString ret;
            int i = 0;
            foreach (const QString &param, params) {
              switch (i) {
              case 0: {
                  QString attr = param;
                  attr.replace(":", "=");
                  attr.replace(";", " ");
                  attr.replace(",", " ");
                  ret = core_html_font_open(attr);
                } break;
              case 1:
                ret += param + core_html_font_close();
                break;
              default:
                ret += param;
              }
              i++;
            }
            return ret;
          }

        } else {
          QString ret;
          QString attr = attrs.join(" ");
          attr.replace(":", "=");
          attr.replace(";", " ");
          attr.replace(",", " ");
          int i = 0;
          foreach (const QString &param, params) {
            if (i == 0)
              ret = core_html_font(param, attr);
            else
              ret += param;
            i++;
          }
          return ret;
        }
      }
      Q_ASSERT(0);
      break;

  #define RETURN_HTML_TAG(_tag) \
    { \
      switch (params.size()) { \
      case 0: return core_html_##_tag(); \
      case 1: return core_html_##_tag(params.front()); \
      default: { \
          QString ret; \
          int i = 0; \
          foreach (const QString &param, params) { \
            if (i == 0) \
              ret = core_html_##_tag(param); \
            else \
              ret += param; \
            i++; \
          } \
          return ret; \
        } \
      } \
      Q_ASSERT(0); \
      break; \
    }

    case H_Div:
      RETURN_HTML_TAG(div)

    case H_Em:
    case H_Emph:
    case H_Italic:
    case H_TextIt:
      RETURN_HTML_TAG(em)

    case H_B:
    case H_Bf:
    case H_Bold:
    case H_Strong:
    case H_TextBf:
      RETURN_HTML_TAG(strong)

  #undef RETURN_HTML_TAG

  #define RETURN_HTML_STYLE(_html, _style) \
    { \
      switch (params.size()) { \
      case 0: return ""; \
      case 1: return _html(params.front(), _style); \
      default: { \
          QString ret; \
          int i = 0; \
          foreach (const QString &param, params) { \
            if (i == 0) \
              ret = _html(param, _style); \
            else \
              ret += param; \
            i++; \
          } \
          return ret; \
        } \
      } \
      Q_ASSERT(0); \
      break; \
    }

    case H_LineThrough:
    case H_Strike:
    case H_StrikeOut:
    case H_Sout:
      RETURN_HTML_STYLE(CORE_HTML_STYLE, text-decoration:line-through)

    case H_Underline:
    case H_Uline:
    case H_Url: // Temporary rendering solution
      RETURN_HTML_STYLE(CORE_HTML_STYLE, text-decoration:underline)

    case H_Overline:
      RETURN_HTML_STYLE(CORE_HTML_STYLE, text-decoration:overline)

    case H_Blink:
      RETURN_HTML_STYLE(CORE_HTML_STYLE, text-decoration:blink)

  #define CASE_TRANSFORM(_trans) \
    case H_##_trans: \
      RETURN_HTML_STYLE(CORE_HTML_STYLE, text-transform:_trans)

    CASE_TRANSFORM(Uppercase)
    CASE_TRANSFORM(Lowercase)
  #undef CASE_TRANSFORM

  #define CASE_COLOR(_color) \
    case H_##_color: \
      RETURN_HTML_STYLE(CORE_HTML_STYLE, color:_color)

    CASE_COLOR(Black)
    CASE_COLOR(Blue)
    CASE_COLOR(Brown)
    CASE_COLOR(Cyan)
    CASE_COLOR(Green)
    CASE_COLOR(Gray)
    CASE_COLOR(Grey)
    CASE_COLOR(Magenta)
    CASE_COLOR(Orange)
    CASE_COLOR(Pink)
    CASE_COLOR(Purple)
    CASE_COLOR(Red)
    CASE_COLOR(White)
    CASE_COLOR(Yellow)
  #undef CASE_COLOR

  #define CASE_SIZE(_id, _size) \
    case H_##_id: \
      RETURN_HTML_STYLE(core_html_style, "font-size:" + _size)

    CASE_SIZE(Tiny,       tinySize_)
    CASE_SIZE(Small,      smallSize_)
    CASE_SIZE(Normal,     normalSize_)
    CASE_SIZE(Big,        largeSize_)
    CASE_SIZE(Large,      largeSize_)
    CASE_SIZE(Huge,       hugeSize_)
  #undef CASE_SIZE

  #undef RETURN_HTML_STYLE

      // Unknown tag
    default:
      return QString();
    }
  }

  // - Unparse -

  QString
  AnnotationParser::reduceHtml(const QString &html) const
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
  AnnotationParser::reduceHtmlElement(const QWebElement &input) const
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
          innerHtml.replace("<", "<<");
          innerHtml.replace(">", ">>");
          innerHtml.replace("<<", "</span><");
          innerHtml.replace(">>", "><span>");
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
          IFTEXTDEC("underline", "\\uline")
          else IFTEXTDEC("line-through", "\\strike")
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
  AnnotationParser::parseRGBColor(const QString &rgb)
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

} // namespace Core

// EOF
