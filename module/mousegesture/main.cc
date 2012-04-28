#include <QApplication>

#include "mainwindow.h"

#include "mousegesturefilter.h"
#include "mousegesture.h"

int main( int argc, char **argv )
{
  QApplication app( argc, argv );
  MouseGestureFilter filter;
  MainWindow mw;

  MouseGesture *g;
  DirectionList dl;

  /* Clear all by three sideways moves */
  dl << AnyHorizontal << AnyHorizontal << AnyHorizontal;
  g = new MouseGesture( dl, &filter );
  filter.addGesture( g );
  mw.connect( g, SIGNAL(gestured()), SLOT(clearAll()) );

  /* Set all by moving up-left */
  dl.clear();
  dl << Up << Left;
  g = new MouseGesture( dl, &filter );
  filter.addGesture( g );
  mw.connect( g, SIGNAL(gestured()), SLOT(setAll()) );

  /* When nothing else matches */
  dl.clear();
  dl << NoMatch;
  g = new MouseGesture( dl, &filter );
  filter.addGesture( g );
  mw.connect( g, SIGNAL(gestured()), SLOT(noMatch()) );

  /* The secret message - four possible starting corners */
  dl.clear();
  dl << Up << Right << Down << Left;
  g = new MouseGesture( dl, &filter );
  filter.addGesture( g );
  mw.connect( g, SIGNAL(gestured()), SLOT(secretMessage()) );

  dl.clear();
  dl << Right << Down << Left << Up;
  g = new MouseGesture( dl, &filter );
  filter.addGesture( g );
  mw.connect( g, SIGNAL(gestured()), SLOT(secretMessage()) );

  dl.clear();
  dl << Down << Left << Up << Right ;
  g = new MouseGesture( dl, &filter );
  filter.addGesture( g );
  mw.connect( g, SIGNAL(gestured()), SLOT(secretMessage()) );

  dl.clear();
  dl << Left << Up << Right << Down;
  g = new MouseGesture( dl, &filter );
  filter.addGesture( g );
  mw.connect( g, SIGNAL(gestured()), SLOT(secretMessage()) );

  mw.installEventFilter( &filter );
  mw.show();

  return app.exec();
}
