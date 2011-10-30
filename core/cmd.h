#ifndef _CORE_CMD_H
#define _CORE_CMD_H

// core/cmd.h
// 8/9/2011
//
// See: http://en.wikibooks.org/wiki/LaTeX/Command_Glossary

#include <QString>
#include <QHash>
#include <utility>

// - Excaped -

#define CORE_CMDCH      '\\'
#define CORE_CMDSTR     "\\"

#define CORE_CMD_ESCAPE_CMDSTR                  CORE_CMDSTR CORE_CMDSTR
#define CORE_CMD_ESCAPE_LEFTCURLYBRACKET        CORE_CMDSTR "{"
#define CORE_CMD_ESCAPE_RIGHTCURLYBRACKET       CORE_CMDSTR "}"
#define CORE_CMD_ESCAPE_LEFTSQUAREBRACKET       CORE_CMDSTR "["
#define CORE_CMD_ESCAPE_RIGHTSQUAREBRACKET      CORE_CMDSTR "]"

#define CORE_CMD_ESCAPE_N                       CORE_CMDSTR "n"

// - Special -        -

#define CORE_CMD_EVAL                   CORE_CMDSTR "eval"      // TODO
#define CORE_CMD_REPEAT                 CORE_CMDSTR "repeat"    // \repeat[10]{anything}

#define CORE_CMD_CTRL_CLOSE             CORE_CMDSTR "close"
#define CORE_CMD_CTRL_CHAT              CORE_CMDSTR "chat"
#define CORE_CMD_CTRL_ERROR             CORE_CMDSTR "error"
#define CORE_CMD_CTRL_EXIT              CORE_CMDSTR "exit"
#define CORE_CMD_CTRL_PLAY              CORE_CMDSTR "play"
#define CORE_CMD_CTRL_QUIT              CORE_CMDSTR "quit"
#define CORE_CMD_CTRL_SAY               CORE_CMDSTR "say"
#define CORE_CMD_CTRL_WARN              CORE_CMDSTR "warn"

#define CORE_CMD_VIEW_FLY               CORE_CMDSTR "fly"
#define CORE_CMD_VIEW_TOP               CORE_CMDSTR "top"
#define CORE_CMD_VIEW_BOTTOM            CORE_CMDSTR "bottom"
#define CORE_CMD_SUB                    CORE_CMDSTR "sub"
#define CORE_CMD_SUBTITLE               CORE_CMDSTR "subtitle"

// - HTML style -        -

#define CORE_CMD_HTML_FONT              CORE_CMDSTR "font"      // \font[color=red]{anything}
#define CORE_CMD_HTML_BR                CORE_CMDSTR "br"
#define CORE_CMD_HTML_DIV               CORE_CMDSTR "div"
#define CORE_CMD_HTML_SPAN              CORE_CMDSTR "span"
#define CORE_CMD_HTML_EM                CORE_CMDSTR "em"
#define CORE_CMD_HTML_STRONG            CORE_CMDSTR "strong"

#define CORE_CMD_STYLE                  CORE_CMDSTR "style"     // \style[color:red]{anything}

#define CORE_CMD_STYLE_BLINK            CORE_CMDSTR "blink"
#define CORE_CMD_STYLE_LINETHROUGH      CORE_CMDSTR "linethrough"
#define CORE_CMD_STYLE_LOWERCASE        CORE_CMDSTR "lowercase"
#define CORE_CMD_STYLE_STRIKE           CORE_CMDSTR "strike"
#define CORE_CMD_STYLE_STRIKEOUT        CORE_CMDSTR "strikeout"
#define CORE_CMD_STYLE_UPPERCASE        CORE_CMDSTR "uppercase"

#define CORE_CMD_STYLE_URL              CORE_CMDSTR "url"       // \url{wefef}

// - LaTeX style -

#define CORE_CMD_VERBATIM               CORE_CMDSTR "verbatim"  // \verbatim
#define CORE_CMD_V                      CORE_CMDSTR "v"         // \v, alias of verbatim

#define CORE_CMD_LATEX_BEGIN            CORE_CMDSTR "begin"     // TODO
#define CORE_CMD_LATEX_END              CORE_CMDSTR "end"       // TODO

#define CORE_CMD_LATEX_BF               CORE_CMDSTR "bf"
#define CORE_CMD_LATEX_EMPH             CORE_CMDSTR "emph"
#define CORE_CMD_LATEX_LINEBREAK        CORE_CMDSTR "linebreak"
#define CORE_CMD_LATEX_SOUT             CORE_CMDSTR "sout"
#define CORE_CMD_LATEX_TEXTIT           CORE_CMDSTR "textit"
#define CORE_CMD_LATEX_TEXTBF           CORE_CMDSTR "textbf"
#define CORE_CMD_LATEX_ULINE            CORE_CMDSTR "uline"
#define CORE_CMD_LATEX_UNDERLINE        CORE_CMDSTR "underline"
#define CORE_CMD_LATEX_OVERLINE         CORE_CMDSTR "overline"

#define CORE_CMD_LATEX_TINY             CORE_CMDSTR "tiny"
#define CORE_CMD_LATEX_SMALL            CORE_CMDSTR "small"
#define CORE_CMD_LATEX_NORMAL           CORE_CMDSTR "normalsize"
#define CORE_CMD_LATEX_LARGE            CORE_CMDSTR "large"
#define CORE_CMD_LATEX_HUGE             CORE_CMDSTR "huge"

#define CORE_CMD_COLOR                  CORE_CMDSTR "color"     // \color{red}{anything}
#define CORE_CMD_COLOR_BACKGROUND       CORE_CMDSTR "background"
#define CORE_CMD_COLOR_FOREGROUND       CORE_CMDSTR "foreground"

#define CORE_CMD_COLOR_BLACK            CORE_CMDSTR "black"
#define CORE_CMD_COLOR_BLUE             CORE_CMDSTR "blue"
#define CORE_CMD_COLOR_BROWN            CORE_CMDSTR "brown"
#define CORE_CMD_COLOR_CYAN             CORE_CMDSTR "cyan"
#define CORE_CMD_COLOR_GREEN            CORE_CMDSTR "green"
#define CORE_CMD_COLOR_GRAY             CORE_CMDSTR "gray"
#define CORE_CMD_COLOR_GREY             CORE_CMDSTR "grey"
#define CORE_CMD_COLOR_MAGENTA          CORE_CMDSTR "magenta"
#define CORE_CMD_COLOR_ORANGE           CORE_CMDSTR "orange"
#define CORE_CMD_COLOR_PINK             CORE_CMDSTR "pink"
#define CORE_CMD_COLOR_PURPLE           CORE_CMDSTR "purple"
#define CORE_CMD_COLOR_RED              CORE_CMDSTR "red"
#define CORE_CMD_COLOR_WHITE            CORE_CMDSTR "white"
#define CORE_CMD_COLOR_YELLOW           CORE_CMDSTR "yellow"

namespace Core {

  enum TagHash { // qHash(QString(CORE_CMD_...)

    // - Escaped -

    H_N =             1582,             // CORE_CMD_ESCAPE_N

    // - Special -

    H_Eval =            6474876,        // CORE_CMD_EVAL
    H_Repeat =          59534148,       // CORE_CMD_REPEAT


    H_Close =           103429781,      // CORE_CMD_CTRL_CLOSE))
    H_Chat =            6463108,        // CORE_CMD_CTRL_CHAT))
    H_Error =           103586146,      // CORE_CMD_CTRL_ERROR))
    H_Exit =            6523908,        // CORE_CMD_CTRL_QUIT
    H_Play =            6517385,        // CORE_CMD_CTRL_PLAY
    H_Quit =            6475524,        // CORE_CMD_CTRL_EXIT
    H_Say =             407945,         // CORE_CMD_CTRL_SAY
    H_Tr =              25522,          // CORE_CMD_CTRL_TR
    H_Warn =            6543502,        // CORE_CMD_CTRL_WARN

    H_Fly =             404793,         // CORE_CMD_ANNOT_FLY
    H_Top =             408416,         // CORE_CMD_ANNOT_TOP
    H_Bottom =          43432861,       // CORE_CMD_ANNOT_BOTTOM
    H_Sub =             408242,         // CORE_CMD_ANNOT_SUB
    H_Subtitle =        194759781,      // CORE_CMD_ANNOT_SUBTITLE


    // - HTML style -

    H_Br =              25234,          // CORE_CMD_HTML_BR
    H_Em =              25277,          // CORE_CMD_HTML_EM
    H_Div =             404230,         // CORE_CMD_HTML_DIV
    H_Font =            6477396,        // CORE_CMD_HTML_FONT
    H_Span =            6530686,        // CORE_CMD_HTML_SPAN
    H_Strong =          61576839,       // CORE_CMD_HTML_STRONG

    H_Style =           104513573,      // CORE_CMD_STYLE

    H_Blink =           103362635,      // CORE_CMD_HTML_BLINK
    H_Lowercase =       231013973,      // CORE_CMD_HTML_LOWERCASE
    H_LineThrough =     111108696,      // CORE_CMD_HTML_LINETHROUGH
    H_Strike =          61575381,       // CORE_CMD_HTML_STRIKE
    H_StrikeOut =       151909284,      // CORE_CMD_HTML_STRIKEOUT
    H_Uppercase =       113565813,      // CORE_CMD_HTML_UPPERCASE

    H_Url =             408716,         // CORE_CMD_STYLE_URL


    // - LaTeX style -

    H_Verbatim =        210259325,      // CORE_CMD_VERBATIM
    H_V =               1590,           // CORE_CMD_V

    H_Begin =           103333374,      // CORE_CMD_BEGIN
    H_End =             404548,         // CORE_CMD_END

    H_Color =           103441250,      // CORE_CMD_COLOR
    H_BackgroundColor = 19687172,       // CORE_CMD_COLOR_BACKGROUND
    H_ForegroundColor = 187484484,      // CORE_CMD_COLOR_FOREGROUND

    H_Black =           103360411,      // CORE_CMD_COLOR_BLACK
    H_Blue =            6460341,        // CORE_CMD_COLOR_BLUE
    H_Brown =           103388894,      // CORE_CMD_COLOR_BROWN
    H_Cyan =            6467454,        // CORE_CMD_COLOR_CYAN
    H_Green =           103713726,      // CORE_CMD_COLOR_GREEN
    H_Gray =            6482057,        // CORE_CMD_COLOR_GRAY
    H_Grey =            6482121,        // CORE_CMD_COLOR_GREY
    H_Magenta =         58575297,       // CORE_CMD_COLOR_MAGENTA
    H_Orange =          57181205,       // CORE_CMD_COLOR_ORANGE
    H_Pink =            6516811,        // CORE_CMD_COLOR_PINK
    H_Purple =          58496997,       // CORE_CMD_COLOR_PURPLE
    H_Red =             407732,         // CORE_CMD_COLOR_RED
    H_White =           104722597,      // CORE_CMD_COLOR_WHITE
    H_Yellow =          66859943,       // CORE_CMD_COLOR_YELLOW

    H_Tiny =            6533209,        // CORE_CMD_LATEX_TINY
    H_Small =           104478764,      // CORE_CMD_LATEX_SMALL
    H_Normal =          63261733,       // CORE_CMD_LATEX_NORMAL
    H_Large =           103975125,      // CORE_CMD_LATEX_LARGE
    H_Huge =            6486997,        // CORE_CMD_LATEX_HUGE

    H_Bf =              25222,          // CORE_CMD_LATEX_BF
    H_Emph =            6472808,        // CORE_CMD_LATEX_EMPH
    H_LineBreak =       79031675,       // CORE_CMD_LATEX_LINEBREAK
    H_Overline =        214559365,      // CORE_CMD_LATEX_OVERLINE
    H_TextBf =          61667910,       // CORE_CMD_LATEX_TEXTBF
    H_TextIt =          61668292,       // CORE_CMD_LATEX_TEXTIT
    H_Sout =            6530756,        // CORE_CMD_LATEX_SOUT
    H_Uline =           104607813,      // CORE_CMD_LATEX_ULINE
    H_Underline =       180705477       // CORE_CMD_LATEX_UNDERLINE
  };

#ifdef DEBUG_CORE_CMD
  namespace {
    struct check_cmd_hash()
    {
      check_cmd_hash()
      {
        // - Escaped -

        Q_ASSERT(H_N ==         qHash(QString(CORE_CMD_ESCAPE_N)));

        // - Special -

        Q_ASSERT(H_Eval ==      qHash(QString(CORE_CMD_EVAL)));
        Q_ASSERT(H_Repeat ==      qHash(QString(CORE_CMD_REPEAT)));

        Q_ASSERT(H_Close ==     qHash(QString(CORE_CMD_CTRL_CLOSE)));
        Q_ASSERT(H_Chat ==      qHash(QString(CORE_CMD_CTRL_CHAT)));
        Q_ASSERT(H_Error ==     qHash(QString(CORE_CMD_CTRL_ERROR)));
        Q_ASSERT(H_Exit ==      qHash(QString(CORE_CMD_CTRL_QUIT)));
        Q_ASSERT(H_Play ==      qHash(QString(CORE_CMD_CTRL_PLAY)));
        Q_ASSERT(H_Quit ==      qHash(QString(CORE_CMD_CTRL_EXIT)));
        Q_ASSERT(H_Say ==       qHash(QString(CORE_CMD_CTRL_SAY)));
        Q_ASSERT(H_Warn ==      qHash(QString(CORE_CMD_CTRL_WARN)));

        Q_ASSERT(H_Fly ==       qHash(QString(CORE_CMD_ANNOT_FLY)));
        Q_ASSERT(H_Top ==       qHash(QString(CORE_CMD_ANNOT_TOP)));
        Q_ASSERT(H_Bottom ==    qHash(QString(CORE_CMD_ANNOT_BOTTOM)));
        Q_ASSERT(H_Sub ==       qHash(QString(CORE_CMD_ANNOT_SUB)));
        Q_ASSERT(H_Subtitle ==  qHash(QString(CORE_CMD_ANNOT_SUBTITLE)));

        // - HTML style -

        Q_ASSERT(H_Br ==        qHash(QString(CORE_CMD_HTML_BR)));
        Q_ASSERT(H_Div ==       qHash(QString(CORE_CMD_HTML_DIV)));
        Q_ASSERT(H_Em ==        qHash(QString(CORE_CMD_HTML_EM)));
        Q_ASSERT(H_Font ==      qHash(QString(CORE_CMD_HTML_FONT)));
        Q_ASSERT(H_Span ==      qHash(QString(CORE_CMD_HTML_SPAN)));
        Q_ASSERT(H_Strong ==    qHash(QString(CORE_CMD_HTML_STRONG)));

        Q_ASSERT(H_Style ==     qHash(QString(CORE_CMD_STYLE)));

        Q_ASSERT(H_Blink ==     qHash(QString(CORE_CMD_STYLE_BLINK));
        Q_ASSERT(H_Lowercase == qHash(QString(CORE_CMD_STYLE_LOWERCASE)));
        Q_ASSERT(H_Strike ==    qHash(QString(CORE_CMD_STYLE_STRIKE)));
        Q_ASSERT(H_StrikeOut == qHash(QString(CORE_CMD_STYLE_STRIKEOUT)));
        Q_ASSERT(H_Uppercase == qHash(QString(CORE_CMD_STYLE_UPPERCASE)));
        Q_ASSERT(H_Url ==       qHash(QString(CORE_CMD_STYLE_URL)));

        // - LaTeX style -

        Q_ASSERT(H_Verbatim ==  qHash(QString(CORE_CMD_VERBATIM)));
        Q_ASSERT(H_V ==  qHash(QString(CORE_CMD_V)));
        Q_ASSERT(H_Begin ==     qHash(QString(CORE_CMD_BEGIN)));
        Q_ASSERT(H_End ==       qHash(QString(CORE_CMD_END)));

        Q_ASSERT(H_Color ==     qHash(QString(CORE_CMD_COLOR)));
        Q_ASSERT(H_BackgroundColor == qHash(QString(CORE_CMD_COLOR_BACKGROUND)));
        Q_ASSERT(H_ForegroundColor == qHash(QString(CORE_CMD_COLOR_FOREGROUND)));

        Q_ASSERT(H_Black ==     qHash(QString(CORE_CMD_COLOR_BLACK)));
        Q_ASSERT(H_Blue ==      qHash(QString(CORE_CMD_COLOR_BLUE)));
        Q_ASSERT(H_Brown ==     qHash(QString(CORE_CMD_COLOR_BROWN)));
        Q_ASSERT(H_Cyan ==      qHash(QString(CORE_CMD_COLOR_CYAN)));
        Q_ASSERT(H_Green ==     qHash(QString(CORE_CMD_COLOR_GREEN)));
        Q_ASSERT(H_Gray ==      qHash(QString(CORE_CMD_COLOR_GRAY)));
        Q_ASSERT(H_Grey ==      qHash(QString(CORE_CMD_COLOR_GREY)));
        Q_ASSERT(H_Magenta ==   qHash(QString(CORE_CMD_COLOR_MAGENTA)));
        Q_ASSERT(H_Orange ==    qHash(QString(CORE_CMD_COLOR_ORANGE)));
        Q_ASSERT(H_Pink ==      qHash(QString(CORE_CMD_COLOR_PINK)));
        Q_ASSERT(H_Purple ==    qHash(QString(CORE_CMD_COLOR_PURPLE)));
        Q_ASSERT(H_Red ==       qHash(QString(CORE_CMD_COLOR_RED)));
        Q_ASSERT(H_White ==     qHash(QString(CORE_CMD_COLOR_WHITE)));
        Q_ASSERT(H_Yellow ==    qHash(QString(CORE_CMD_COLOR_YELLOW)));

        Q_ASSERT(H_Tiny ==      qHash(QString(CORE_CMD_LATEX_TINY)));
        Q_ASSERT(H_Small ==     qHash(QString(CORE_CMD_LATEX_SMALL)));
        Q_ASSERT(H_Normal ==    qHash(QString(CORE_CMD_LATEX_NORMAL)));
        Q_ASSERT(H_Large ==     qHash(QString(CORE_CMD_LATEX_LARGE)));
        Q_ASSERT(H_Huge ==      qHash(QString(CORE_CMD_LATEX_HUGE)));

        Q_ASSERT(H_Bf ==        qHash(QString(CORE_CMD_LATEX_BF)));
        Q_ASSERT(H_Emph ==      qHash(QString(CORE_CMD_LATEX_EMPH)));
        Q_ASSERT(H_TextIt ==    qHash(QString(CORE_CMD_LATEX_TEXTIT)));
        Q_ASSERT(H_TextBf ==    qHash(QString(CORE_CMD_LATEX_TEXTBF)));
        Q_ASSERT(H_LineBreak == qHash(QString(CORE_CMD_LATEX_LINEBREAK)));
        Q_ASSERT(H_LineThrough == qHash(QString(CORE_CMD_LATEX_LINETHROUGH)));
        Q_ASSERT(H_Overline ==  qHash(QString(CORE_CMD_LATEX_OVERLINE)));
        Q_ASSERT(H_Sout ==      qHash(QString(CORE_CMD_LATEX_SOUT)));
        Q_ASSERT(H_Uline ==     qHash(QString(CORE_CMD_LATEX_ULINE)));
        Q_ASSERT(H_Underline == qHash(QString(CORE_CMD_LATEX_UNDERLINE)));
      }
    } check_cmd_hash;
  }
#endif // DEBUG_CORE_CMD

} // namespace Core

#endif // CMD_H
