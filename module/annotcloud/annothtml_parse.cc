// annotcloud/annothtml_parse.cc
// 8/19/2011

#include "module/annotcloud/annothtml.h"
#include "module/annotcloud/annottag.h"
#include "module/qtext/htmltag.h"
#include <boost/tuple/tuple.hpp>
#include <stack>

//#define DEBUG "annothtmlparse"
#include "module/debug/debug.h"

#define MAX_REPEAT_COUNT        10

// - Lexical parsing -

// Regexp is not used to improve performance.
std::pair<QString, QString>
AnnotCloud::
AnnotationHtmlParser::parseLeadingTag(const QString &text)
{
  if (text.isEmpty() || text[0] != CORE_CMDCH)
    return std::make_pair(QString(), QString());

  // TODO: use while
  //int index = 0;
  //foreach (QChar c, text)
  //  if (isSeparator(c))
  //    break;
  //  else
  //    index++;
  //if (index >= text.size())

  int index = text.indexOf(' ');
  if (index < 0)
    return std::make_pair(text, QString());
  else
    return std::make_pair(text.mid(0, index), text.mid(index + 1));
}

std::pair<QString, QString>
AnnotCloud::
AnnotationHtmlParser::parseNextToken(const QString &text)
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
AnnotCloud::
AnnotationHtmlParser::toHtml(const QString &text) const
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

  std::stack<std::pair<QString, char> > stack; // pair<QString, StringType>

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
        auto front = stack.top();
        stack.pop();

        switch (front.second) {

        case '}':
          params.prepend(front.first);
          break;
        case ']':
          attrs.prepend(front.first);
          break;

        case CORE_CMDCH: {
            if (!parsed.isEmpty())
              params.append(parsed);

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
          DOUT("warning: unparsed parentheses on the stack");
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

        default: Q_ASSERT(0);
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
          auto front = stack.top();
          stack.pop();

          switch (front.second) {
          case '{':
            break;
          case '[':
            DOUT("parsed: '[{' mismatched");
            break;

          case '}':
            params.prepend(front.first);
            break;
          case ']':
            attrs.prepend(front.first);
            break;

          case CORE_CMDCH: {
            if (!reduced.isEmpty())
              params.append(reduced);
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
          stack.push(std::make_pair(reduced, '}'));

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
          auto front = stack.top();
          stack.pop();

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
          stack.push(std::make_pair(reduced, ']'));

        DOUT("reduced []:" << reduced);
      } break;


    case '{':
    case '[':
      stack.push(std::make_pair(current, current_ch));
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

      stack.push(std::make_pair(current, current_ch));
      break;

    default:
      stack.push(std::make_pair(current, '\0'));
    }

  } while (!stack.empty());

  DOUT("exit: parsed =" << parsed);
  return RETURN(parsed, tags);
}

QString
AnnotCloud::
AnnotationHtmlParser::translate(const QString &tag,
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
    return params.empty() ? html_br()
    : html_br() + params.join("");

#define CASE_COLOR(_hash, _csstag) \
  case _hash: \
    if (params.empty()) \
      return ""; \
    else { \
      if (attrs.empty()) { \
        if (params.size() == 1) \
          return params.first(); \
        else { \
          QString ret; \
          int i = 0; \
          foreach (const QString &param, params) { \
            switch (i) { \
            case 0: ret = html_style_open(_csstag ":" + param); break; \
            case 1: ret += param + html_style_close(); break; \
            default: ret += param; \
            } \
            i++; \
          } \
          return ret; \
        } \
      } else { \
        QString ret; \
        QString color = attrs.first(); \
        int i = 0; \
        foreach (const QString &param, params) { \
          if (i == 0) \
            ret = html_style(param, _csstag ":" + color); \
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

  case H_Size:
    if (params.empty())
      return "";
    else {
      if (attrs.empty()) {
        if (params.size() == 1)
          return params.first();
        else {
          QString ret;
          int i = 0;
          foreach (QString param, params) {
            if (param.isEmpty())
              continue;
            if (param[param.size() -1].isDigit())
              param += "px";
            switch (i) {
            case 0: ret = html_style_open("font-size:" + param); break;
            case 1: ret += param + html_style_close(); break;
            default: ret += param;
            }
            i++;
          }
          return ret;
        }
      } else {
        QString ret;
        QString size = attrs.first();
        if (!size.isEmpty() && size[size.size() -1].isDigit())
          size += "px";
        int i = 0;
        foreach (const QString &param, params) {
          if (i == 0)
            ret = html_style(param, "font-size:" + size);
          else
            ret += param;
          i++;
        }
        return ret;
      }
    }
    Q_ASSERT(0);
    break;

  case H_Repeat:
    if (params.empty())
      return "";
    else {
      if (attrs.empty()) {
        if (params.size() == 1)
          return params.first();
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
              if (count > 0) {
                if (count > MAX_REPEAT_COUNT)
                  count = MAX_REPEAT_COUNT;
                ret = param.repeated(count);
              }
            default:
              ret += param;
            }
            i++;
          }
          return ret;
        }

      } else {
        QString ret;
        QString s_count = attrs.first();
        int count = s_count.toInt();
        if (count > 0) {
          if (count > MAX_REPEAT_COUNT)
            count = MAX_REPEAT_COUNT;
          int i = 0;
          foreach (const QString &param, params) {
            if (i == 0)
              ret = param.repeated(count);
            else
              ret += param;
            i++;
          }
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
          return params.first();
        else {
          QString ret;
          int i = 0;
          foreach (const QString &param, params) {
            switch (i) {
            case 0: {
                QString style = param;
                style.replace("=", ":").replace(",", ";");
                ret = html_style_open(style);
              } break;
            case 1:
              ret += param + html_style_close();
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
        style.replace("=", ":").replace(",", ";");
        int i = 0;
        foreach (const QString &param, params) {
          if (i == 0)
            ret = html_style(param, style);
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
          return params.first();
        else {
          QString ret;
          int i = 0;
          foreach (const QString &param, params) {
            switch (i) {
            case 0: {
                QString attr = param;
                attr.replace(":", "=").replace(";", " ").replace(",", " ");
                ret = html_font_open(attr);
              } break;
            case 1:
              ret += param + html_font_close();
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
        attr.replace(":", "=").replace(";", " ").replace(",", " ");
        int i = 0;
        foreach (const QString &param, params) {
          if (i == 0)
            ret = html_font(param, attr);
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
    case 0: return html_##_tag(); \
    case 1: return html_##_tag(params.first()); \
    default: { \
        QString ret; \
        int i = 0; \
        foreach (const QString &param, params) { \
          if (i == 0) \
            ret = html_##_tag(param); \
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

#define RETURN_HTML_STYLE(_HTML, _style) \
  { \
    switch (params.size()) { \
    case 0: return ""; \
    case 1: return _HTML(+params.first()+, _style); \
    default: { \
        QString ret; \
        int i = 0; \
        foreach (const QString &param, params) { \
          if (i == 0) \
            ret = _HTML(+param+, _style); \
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

#define RETURN_html_style(_html, _style) \
  { \
    switch (params.size()) { \
    case 0: return ""; \
    case 1: return _html(params.first(), _style); \
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
    RETURN_HTML_STYLE(HTML_STYLE, text-decoration:line-through)

  case H_Underline:
  case H_Uline:
  case H_Url: // Temporary rendering solution
    RETURN_HTML_STYLE(HTML_STYLE, text-decoration:underline)

  case H_Overline:
    RETURN_HTML_STYLE(HTML_STYLE, text-decoration:overline)

  case H_Blink:
    RETURN_HTML_STYLE(HTML_STYLE, text-decoration:blink)

#define CASE_TRANSFORM(_trans) \
  case H_##_trans: \
    RETURN_HTML_STYLE(HTML_STYLE, text-transform:_trans)

  CASE_TRANSFORM(Uppercase)
  CASE_TRANSFORM(Lowercase)
#undef CASE_TRANSFORM

#define CASE_COLOR(_color) \
  case H_##_color: \
    RETURN_HTML_STYLE(HTML_STYLE, color:_color)

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
    RETURN_html_style(html_style, "font-size:" + _size)

  CASE_SIZE(Tiny,       tinySize_)
  CASE_SIZE(Small,      smallSize_)
  CASE_SIZE(Normal,     normalSize_)
  CASE_SIZE(Big,        largeSize_)
  CASE_SIZE(Large,      largeSize_)
  CASE_SIZE(Huge,       hugeSize_)
#undef CASE_SIZE

#undef RETURN_HTML_STYLE
#undef RETURN_html_style

    // Unknown tag
  default:
    return QString();
  }
}

// EOF
