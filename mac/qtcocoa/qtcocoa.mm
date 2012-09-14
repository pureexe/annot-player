// qtcocoa.mm
// 7/30/2011

#include "qtcocoa/qtcocoa.h"
#include <QtCore/QFileInfo>
#import <AppKit/AppKit.h>
#ifdef USE_MAC_CARBON
# import <Carbon/CarbonEvents.h>
#endif // USE_MAC_CARBON
//#import <objc/runtime.h>

#ifndef USE_MAC_CARBON
# define GetCurrentEventTime()   nil
#endif // USE_MAC_CARBON

//#define DEBUG "qtcocoa"
//#include "lib/debug/debug.h"

// - Type cast -

NSOBJECT_TYPE_REGISTER(nsobject_t, NSObject)
NSOBJECT_TYPE_REGISTER(nsresponder_t, NSResponder)
NSOBJECT_TYPE_REGISTER(nsstring_t, NSString)
NSOBJECT_TYPE_REGISTER(nswindow_t, NSWindow)
NSOBJECT_TYPE_REGISTER(nsview_t, NSView)

// - Static initialization -

namespace { // anonymous, static
  class qtcocoa_global_ {
    NSAutoreleasePool *pool_;
  public:
    qtcocoa_global_()  { pool_ = [NSAutoreleasePool new]; }
    ~qtcocoa_global_() { [pool_ drain]; }
  };
  qtcocoa_global_ global_;
} // anonymous namespace

// - NSObject -

nsobject_t*
nsobject_new()
{ return nsobject_cast<nsobject_t *>([NSObject new]); }

void
nsobject_release(nsobject_t *obj)
{ [nsobject_cast<NSObject *>(obj) release]; }

// - NSResponder -

nsresponder_t*
nsresponder_new()
{ return nsobject_cast<nsresponder_t *>([NSResponder new]); }

void
nsresonder_release(nsresponder_t *obj)
{ [nsobject_cast<NSResponder *>(obj) release]; }

// - NSString -

nsstring_t*
nsstring_new()
{ return nsobject_cast<nsstring_t *>([NSString new]); }

void
nsstring_release(nsstring_t *obj)
{ [nsobject_cast<NSString *>(obj) release]; }

//nsstring_t*
//nsrstring_string()
//{ return nsobject_cast<nsstring_t *>([NSString string]); }

nsstring_t*
nsstring_new_with_characters(const unichar *chars, ulong len)
{
  return nsobject_cast<nsstring_t *>(
   [[NSString alloc] initWithCharacters:chars length:len]
  );
}

// - NSWindow -

nswindow_t*
nswindow_new()
{ return nsobject_cast<nswindow_t *>([NSWindow new]); }

void
nswindow_release(nswindow_t *obj)
{ [nsobject_cast<NSWindow *>(obj) release]; }

bool
nswindow_accepts_mouse_moved_events(const nswindow_t *obj)
{ return [nsobject_cast<NSWindow *>(obj) acceptsMouseMovedEvents]; }

void
nswindow_set_accepts_mouse_moved_events(nswindow_t *obj, bool t)
{ [nsobject_cast<NSWindow *>(obj) setAcceptsMouseMovedEvents:NSBOOL(t)]; }

// - NSView -

nsview_t*
nsview_new()
{ return nsobject_cast<nsview_t *>([NSView new]); }

void
nsview_release(nsview_t *obj)
{ [nsobject_cast<NSView *>(obj) release]; }

bool
nsview_is_hidden(const nsview_t *obj)
{ return [nsobject_cast<NSView *>(obj) isHidden]; }

void
nsview_set_hidden(nsview_t *obj, bool hidden)
{ [nsobject_cast<NSView *>(obj) setHidden:NSBOOL(hidden)]; }

nswindow_t*
nsview_window(nsview_t *handle)
{
  NSView *view = nsobject_cast<NSView *>(handle);
  NSWindow *window = [view window];
  return nsobject_cast<nswindow_t *>(window);
}

const nswindow_t*
nsview_window(const nsview_t *handle)
{
  NSView *view = nsobject_cast<NSView *>(handle);
  NSWindow *window = [view window];
  return nsobject_cast<nswindow_t *>(window);
}

// - QtCocoa -

// - Type Conversion -

// See: http://www.qtcentre.org/threads/34752-NSString-to-QString
QString
QtCocoa::NSStringToQString(const nsstring_t *handle)
{
  NSString *ns = nsobject_cast<NSString *>(handle);

  NSUInteger size = [ns length];
  if (!size)
    return QString();

  NSRange range = { 0, size }; // { location, length }
  unichar *chars = new unichar[size];

  [ns getCharacters:chars range:range];
  QString ret = QString::fromUtf16(chars, size);
  delete chars;
  return ret;
}

nsstring_t*
QtCocoa::QStringToNSString(const QString &qs)
{
  return qs.isEmpty() ? nsstring_new() :
                        nsstring_new_with_characters(qs.utf16(), qs.size());
}

// See: http://www.cocoadev.com/index.pl?NSEvent
// See: http://stackoverflow.com/questions/3873688/send-a-mouseevent-to-a-nsview-webview-from-code
void
QtCocoa::mouseClickEvent(nsview_t *handle, const QPoint &pos)
{
  NSView *view = nsobject_cast<NSView *>(handle);

  NSEvent *mouseDownEvent = [NSEvent
    mouseEventWithType:NSLeftMouseDown
    location:NSMakePoint(pos.x(), pos.y())
    modifierFlags:nil
    timestamp:GetCurrentEventTime()
    windowNumber:[[view window] windowNumber]
    context:nil
    eventNumber:nil
    clickCount:1
    pressure:nil
  ];
  [view mouseDown:mouseDownEvent];

  NSEvent *mouseUpEvent = [NSEvent
    mouseEventWithType:NSLeftMouseUp
    location:NSMakePoint(pos.x(), pos.y())
    modifierFlags:nil
    timestamp:GetCurrentEventTime()
    windowNumber:[[view window] windowNumber]
    context:nil
    eventNumber:nil
    clickCount:1
    pressure:nil
  ];
  [view mouseUp:mouseUpEvent];
}

// - Event -

// See: NSEvent
// See: http://stackoverflow.com/questions/5993633/how-to-get-keyboard-state-in-objective-c-without-referring-to-nsevent
bool
QtCocoa::isModifierKeyPressed(uint mask)
{ return mask & [NSEvent modifierFlags]; }

bool
QtCocoa::isCommandKeyPressed()
{ return isModifierKeyPressed(NSCommandKeyMask); }

bool
QtCocoa::isControlKeyPressed()
{ return isModifierKeyPressed(NSControlKeyMask); }

bool
QtCocoa::isAlternateKeyPressed()
{ return isModifierKeyPressed(NSAlternateKeyMask); }

bool
QtCocoa::isShiftKeyPressed()
{ return isModifierKeyPressed(NSShiftKeyMask); }

bool
QtCocoa::isHelpKeyPressed()
{ return isModifierKeyPressed(NSHelpKeyMask); }

bool
QtCocoa::isFunctionKeyPressed()
{ return isModifierKeyPressed(NSFunctionKeyMask); }

bool
QtCocoa::isCapsLockKeyPressed()
{ return isModifierKeyPressed(NSAlphaShiftKeyMask); }

// - Shell -

bool
QtCocoa::trashFile(const QString &fileName)
{ return trashFileWithFullPath(QFileInfo(fileName).absoluteFilePath()); }

// See also: http://cocoadev.com/wiki/MoveToTrash
bool
QtCocoa::trashFileWithFullPath(const QString &fullPath)
{
  NSString *path = nsobject_cast<NSString *>(QStringToNSString(fullPath));

  NSInteger tag;
  return [[NSWorkspace sharedWorkspace]
      performFileOperation:NSWorkspaceRecycleOperation
                    source:[path stringByDeletingLastPathComponent]
               destination:@""
                     files:[NSArray arrayWithObject:[path lastPathComponent]]
                       tag:&tag] && !tag;
}

// EOF
