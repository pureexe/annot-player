#ifndef AIMLPARSER_H
#define AIMLPARSER_H

// aimlparser.h
// 7/31/2011

#include <QMap>
#include <QStringList>
#include <QRegExp>
#include <QDomDocument>
#include <QList>

class AimlParser
{
public:
  AimlParser();

  bool loadAiml(const QString &__todo);
  bool loadSubs(const QString &__todo);
  bool loadVars(const QString &__todo, bool __todo2);
  bool saveVars(const QString &__todo);
  QString getResponse(const QString &__todo, bool srai = false);

private:
  QString resolveNode(QDomNode *__todo0, const QStringList &__todo = QStringList(),
     const QStringList &__todo2 = QStringList(), const QStringList &__todo3 = QStringList());

  void parseCategory(QDomNode *__todo);
  QString executeCommand(const QString &__todo);

protected:
  struct Node;
  struct Leaf
  {
    Node *parent;
    QDomNode tpl;
    QString topic;
    QString that;

    Leaf() : parent(0), topic(""), that("") { }
  };

  struct Node
  {
    Node *parent;
    QString word;
    QList<Node*> children;
    QList<Leaf*> leaves;

    Node() : parent(0), word("") { }

    bool match(QStringList::const_iterator input, const QStringList &inputWords,
               const QString &currentThat, const QString &currentTopic, QStringList &capturedThatTexts,
               QStringList &capturedTopicTexts, Leaf *&leaf);
  };

private:
  Node root_;

  QMap<QString, QString> parameterValue;
  QMap<QString, QString> botVarValue;
  QList<QRegExp> subOld;
  QStringList subNew;
  QStringList inputList;
  QList<QStringList> thatList;
  QList<QDomNode*> visitedNodeList;
};

#endif
