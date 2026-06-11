// APPmainCocoa.mm is an Objective C++ file.  I chose this in order to be compatible with C++ name mangling.
// I try to implement most of the logic in APPmain.cpp in C++ using our familiar libraries.
#import <Cocoa/Cocoa.h>
#import "../APP/APPmain.h"
#import "../CACHE/CACHEfile.h"

@interface MenuBarExtraController : NSObject <NSApplicationDelegate>
@property (nonatomic, strong) NSStatusItem *statusItem;
@end

void MACalert(NSString* message) {
   NSAlert *alert = [[NSAlert alloc] init];
   //[alert setMessageText:[NSString stringWithFormat:@"%@", message]];
   [alert setAlertStyle:NSAlertStyleCritical];
   // show app icon in dock so it's easy to find the alert
   [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
   // try and make the dialog come to front.
   [NSApp activateIgnoringOtherApps:YES];

   // Create a text view for the informative text with left-justified alignment
   NSTextView *textView = [[NSTextView alloc] initWithFrame:NSMakeRect(0, 0, 450, 200)]; // Adjust the frame as necessary
   [textView setString:[NSString stringWithFormat:@"%@", message]];
   [textView setEditable:NO];
   [textView setAlignment:NSTextAlignmentLeft];
   [textView setDrawsBackground:NO];
//   [textView setBorderType:NSNoBorder];

   // Set the text view as the accessory view of the alert
   [alert setAccessoryView:textView];

   // Get the alert window and bring it to the front
   NSWindow *alertWindow = [alert window];
   [alertWindow setLevel:NSModalPanelWindowLevel];

   [alert runModal];
   // don't show app icon in dock
   [NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];
}

// run command and return exit code
int MACexecute(NSString* command) {
   NSTask *task = [[NSTask alloc] init];
   [task setLaunchPath:@"/bin/sh"];
   [task setArguments:@[@"-c", command]];
   [task launch];
   [task waitUntilExit];
   return [task terminationStatus];
}

bool MACgitInstalled() {
   // in most cases, xcode must be installed in order for git to be installed, and installing
   // xcode automatically installs git. This also prevents the 'install xcode tools' dialog
   // from being triggered automatically.
   return 0 == MACexecute(@"xcode-select -p >/dev/null 2>&1");
}

@implementation MenuBarExtraController

NSMenu* menu;
NSImage* iconOffImage;
NSImage* iconOnImage;
NSStatusItem* pStatusBar;
NSMenuItem* pStopItem;
NSMenuItem* pStartItem;
NSMenuItem* pLogItem;
NSMenuItem* pQuitItem;

NSMenuItem* pDashboard;

void MACcheckStatus();

NSImage* MACloadImage(const char* ResourcePath){
   char* pData;
   int Size;
   APPloadResourceData(ResourcePath, &pData, &Size);
   NSData* pngData = [NSData dataWithBytes:pData length:Size];
   NSImage* pResult = [[NSImage alloc] initWithData:pngData];
   APPfreeData(pData);
   //pngData is automatically released in the current memory pool so no need to release it.
   return pResult;
}

-(void)applicationDidFinishLaunching:(NSNotification *)notification {
   // Create a status bar item
   self.statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
   pStatusBar = self.statusItem;

   self.statusItem.button.image = iconOffImage;
   // Add a menu to the status bar item
   menu = [[NSMenu alloc] initWithTitle:@"Menu"];
   [menu setAutoenablesItems:NO];
   pStartItem  = [menu addItemWithTitle:@"Start Iguana"    action:@selector(MACiguanaStart)     keyEquivalent:@""];
   [pStartItem setEnabled:YES];
   pStopItem  = [menu addItemWithTitle:@"Stop Iguana"      action:@selector(MACiguanaStop)      keyEquivalent:@""];
   [pStopItem setEnabled:NO];
   [menu addItem:[NSMenuItem separatorItem]];

   pDashboard = [menu addItemWithTitle:@"Iguana Dashboard" action:@selector(MACiguanaDashboard) keyEquivalent:@""];
   [pDashboard setEnabled:NO];
   [menu addItem:[NSMenuItem separatorItem]];

   pLogItem = [menu addItemWithTitle:@"View Service Logs" action:@selector(MACiguanaLog)      keyEquivalent:@""];
   [pLogItem setEnabled:NO];
   [menu addItem:[NSMenuItem separatorItem]];

   [menu addItemWithTitle:@"Iguana Help"      action:@selector(MACiguanaHelp)      keyEquivalent:@""];
   [menu addItem:[NSMenuItem separatorItem]];

   pQuitItem = [menu addItemWithTitle:@"Quit Monitor" action:@selector(MACquit) keyEquivalent:@""];
   [pQuitItem setEnabled:NO];
   [self.statusItem setMenu:menu];
   if(APPhomeHasSpaces()) {
      MACalert(@"Home directory contains spaces. Please rename your home directory without them.");
      APPhomeSpacesHelp();
      [self MACquit];
   }
   MACcheckStatus();
}

-(void)MACiguanaStart {
   if (!MACgitInstalled()){
      MACalert(@"Git is required to run IguanaX. Please make sure it's installed first.");
      APPgitHelp();
      return;
   }
   APPiguanaStart();
}
-(void)MACiguanaStop      { APPiguanaStop();       }
-(void)MACiguanaDashboard { APPiguanaDashboard();  }
-(void)MACiguanaLog       { APPiguanaServiceLog(); }
-(void)MACiguanaHelp      { APPiguanaHelp();       }
-(void)MACquit            {
   APPiguanaStop();
   [NSApp terminate:nil];
}
@end

@interface MyTimerClass : NSObject
-(void)startTimer;
@end

void MACcheckStatus(){
   bool Running = APPiguanaStatus();
   if (Running){
      [pStartItem setEnabled:NO];
      [pQuitItem  setEnabled:NO];
      [pStopItem  setEnabled:YES];
      [pDashboard setEnabled:YES];
      pStatusBar.button.image = iconOnImage;

   } else {
      [pStartItem setEnabled:YES];
      [pQuitItem  setEnabled:YES];
      [pStopItem  setEnabled:NO];
      [pDashboard setEnabled:NO];
      pStatusBar.button.image = iconOffImage;
   }
   bool Exists = APPiguanaLogExists();
   [pLogItem setEnabled:(Exists ? YES : NO)];
}

@implementation MyTimerClass
- (void)timerFired:(NSTimer *)timer {
   MACcheckStatus();
}

- (void)startTimer {
   NSTimeInterval interval = 2.0; // 2 second interval
   // Create the timer and schedule it on the current run loop
   NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:interval
                                                     target:self
                                                   selector:@selector(timerFired:)
                                                   userInfo:nil
                                                    repeats:YES];
   // Add the timer to the run loop in the default mode
   [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
}
@end

bool MACisDarkModeEnabled() {
   if (@available(macOS 10.14, *)) {
      NSAppearance *appearance = [NSAppearance currentAppearance];
      if ([appearance.name isEqualToString:NSAppearanceNameDarkAqua]) {
         return true;
      }
   }
   return false;
}

void MACcompareServiceExePath() {
   char* pExePath;
   char* pServicePath;
   APPexePath(&pExePath);
   APPservicePath(&pServicePath);
   if(*pExePath && *pServicePath && strstr(pExePath, pServicePath) == NULL) {
      NSString* formattedString =
                 [NSString stringWithFormat:@"Looks like you have different (older?) copy of Iguana running.\n\n"
                                            "The one you have running is installed at this path:\n%s\n\n"
                                            "But this controller is installed at this different path:\n%s\n\n"
                                            "If you stop Iguana from this controller and restart it then it will be run from\n"
                                            "the same path as this controller.", pExePath, pServicePath];
      MACalert(formattedString);
   }
   APPfreeData(pExePath);
   APPfreeData(pServicePath);
}

int APPcocoaMain() {
   APPwriteControllerPid();
   @autoreleasepool {
      if (MACisDarkModeEnabled()){
         iconOffImage = MACloadImage("/web/APP/APPoffDark.png");
         iconOnImage  = MACloadImage("/web/APP/APPonDark.png");
      } else {
         iconOffImage = MACloadImage("/web/APP/APPoff.png");
         iconOnImage  = MACloadImage("/web/APP/APPon.png");
      }
      CACHEfreeCache();
      if (!APPinstalled()){ APPiguanaStart(); }
      MACcompareServiceExePath();

      NSApplication *application = [NSApplication sharedApplication];
      MenuBarExtraController *controller = [[MenuBarExtraController alloc] init];
      [application setDelegate:controller];
      MyTimerClass* timerClass =[[MyTimerClass alloc] init];
      [timerClass startTimer];
      [application run];
   }
   return 0;
}