//
//  AppDelegate.m
//  IguanaController
//
//  Created by Scott C. Ripley on 2014-12-02.
//  Copyright (c) 2015 iNTERFACEWARE. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;

- (void)alert:(NSString*)message;
- (int)run:(NSString*)command output:(NSString**)output;

@end

@implementation AppDelegate

@synthesize statusBar = _statusBar;

// TODO: really don't like hard coded path, but it makes debugging a bit easier.
// Note that this is the iguana in /Applications, not the one in main2/Iguana.
NSString *binPath = @"/Applications/IguanaX.app/Contents/Resources/bin/";
double statusCheckInterval = 5.0;    // seconds

- (void)awakeFromNib {
    // don't show app icon in dock
    [NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];
    // ...
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // NOTE: Comment out the check below during development so you can debug in xcode
    NSString *bundlePath = [[NSBundle mainBundle] bundlePath];
    if (![bundlePath  containsString: @"/Applications/IguanaX.app"]) {
        [self alert: @"Please make sure IguanaX.app is installed in the /Applications folder and named IguanaX."];
        [NSApp terminate:self];
    }

    self.statusBar = [[NSStatusBar systemStatusBar]statusItemWithLength:NSVariableStatusItemLength];
    self.statusBar.menu = self.statusMenu;
    [self statusIguana];
    [self enableAutoStart];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

- (BOOL)validateMenuItem:(NSMenuItem *)item {
    // The delay between clicking the menu icon and the menu showing up is caused by running
    // the status command below, which happens every time we open the menu.
    NSString* output = nil;
    [self  run: [binPath stringByAppendingString:@"iguana --service status"] output: &output];
    if ([item.title isEqualToString:@"Start Iguana"]) {
        if ([output  containsString: @"Running"]) {
            return NO;
        }
    } else if ([item.title isEqualToString:@"Iguana Dashboard"] || [item.title isEqualToString:@"Stop Iguana"]) {
        if (![output  containsString: @"Running"]) {
            return NO;
        }
    }
    return YES;
}


- (void)enableAutoStart {
    NSString *label = @"com.interfaceware.IguanaX.controller";
    NSMutableDictionary *plist = [[NSMutableDictionary alloc] init];
    plist[@"Label"] = label;
    plist[@"Program"] = [[NSBundle mainBundle] executablePath];
    plist[@"RunAtLoad"] = @YES;
    NSString *launchAgentPath = [NSString stringWithFormat:@"%@/%@.%@", @"Library/LaunchAgents/", label, @"plist"];
    NSError *error;
    if (![plist writeToURL: [NSURL fileURLWithPath: launchAgentPath relativeToURL: [[NSFileManager defaultManager] homeDirectoryForCurrentUser]] error: &error]) {
        [self alert: [error localizedDescription]];
    }
}

- (void)turnOnStatusBarImage {
    NSImage *icon = [NSImage imageNamed:@"StatusBarIconOn"];
    self.statusBar.button.image = icon;
}

- (void)turnOffStatusBarImage {
    NSImage *icon = [NSImage imageNamed:@"StatusBarIconOff"];
    icon.template = YES;
    self.statusBar.button.image = icon;
}

- (int)run:(NSString*)command output:(NSString**)output {
    NSPipe* pipe = [NSPipe pipe];
    NSTask* task = [[NSTask alloc] init];
    [task setLaunchPath: @"/bin/sh"];
    [task setArguments:@[@"-c", [NSString stringWithFormat:@"%@", command]]];
    [task setStandardOutput:pipe];
    [task setStandardError:pipe];
    NSFileHandle* file = [pipe fileHandleForReading];
    [task launch];
    if (output != nil) {
        *output = [[NSString alloc] initWithData:[file readDataToEndOfFile] encoding:NSUTF8StringEncoding];
    }
    [task waitUntilExit];
    return [task terminationStatus];
}

- (void)alert:(NSString*)message {
    [NSApp activateIgnoringOtherApps:YES];
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:[NSString stringWithFormat:@"%@", message]];
    // show app icon in dock so it's easy to find the alert
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [alert runModal];
    // don't show app icon in dock
    [NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];
}


- (void)stopIguana {
    NSString* output = nil;
    int returnCode = [self  run: [binPath stringByAppendingString:@"iguana --service stop"] output: &output];
    if (returnCode != 0) {
        [self alert: output];   // why --service stop failed.
        return;
    }
    [self turnOffStatusBarImage];

    // uninstall the service so it's not auto started when user logs in
    returnCode = [self  run: [binPath stringByAppendingString:@"iguana --service uninstall"] output: &output];
    if (returnCode != 0) {
        [self alert: output];   // why --service install failed.
        return;
    }

}

- (void)statusIguana {
    NSString* output = nil;
    int returnCode = [self  run: [binPath stringByAppendingString:@"iguana --service status"] output: &output];
    if (returnCode != 0) {
        [self alert: output];   // why --service status failed.
    }
    if ([output  containsString: @"Running"]) {
        [self turnOnStatusBarImage];
    } else {    // "Stopped" or "Not found"
        [self turnOffStatusBarImage];
    }
    // check status again
    [NSTimer  scheduledTimerWithTimeInterval: statusCheckInterval target: self selector: @selector(statusIguana) userInfo: nil repeats: NO];
}


- (IBAction)startIguana:(id)sender {
    // install the service so it's auto started when user logs in
    NSString* output = nil;
    int returnCode = [self  run: [binPath stringByAppendingString:@"iguana --service install"] output: &output];
    if (returnCode != 0) {
        [self alert: output];   // why --service install failed.
        return;
    }

    returnCode = [self  run: [binPath stringByAppendingString:@"iguana --service start"] output: &output];
    if (returnCode != 0) {
        [self alert: output];   // why --service start failed.
    }
    [self turnOnStatusBarImage];
}

- (IBAction)stopIguana:(id)sender {
    [self stopIguana];
}

- (IBAction)quitIguanaController:(id)sender {
    NSString* output = nil;
    [self  run: [binPath stringByAppendingString:@"iguana --service status"] output: &output];
    if (![output  containsString: @"Running"]) {
        [NSApp terminate:self];
    }
    
    [NSApp activateIgnoringOtherApps:YES];
    
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"Proceed"];
    [alert addButtonWithTitle:@"Cancel"];
    [alert setMessageText:@"Quit Iguana Controller:"];
    [alert setInformativeText:@"you have selected to quit IguanaX Controller...\nstop IguanaX and exit?"];
    [alert setAlertStyle:NSAlertStyleWarning];
    // show app icon in dock so exit dialog is easy to find
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    if ([alert runModal] == NSAlertFirstButtonReturn) {
        [self stopIguana];
        [NSApp terminate:self];
    }
    
    // probably no necessary to hide app icon right before we quit
    [NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];
}

- (IBAction)launchDashboard:(id)sender {
    [self  run: [binPath stringByAppendingString:@"iguana --open_gui"] output: nil];
}

- (IBAction)launchKnowledgeBase:(id)sender {
    [self  run: @"open http://help.interfaceware.com/v6" output: nil];
}

- (IBAction)launchIguanaForums:(id)sender {
    [self  run: @"open http://help.interfaceware.com/forums" output: nil];
}

- (IBAction)launchIguanaApiReference:(id)sender {
    [self  run: @"open http://help.interfaceware.com/api" output: nil];
}

- (IBAction)launchIguanaCodeRepository:(id)sender {
    [self  run: @"open http://help.interfaceware.com/code" output: nil];
}

@end
