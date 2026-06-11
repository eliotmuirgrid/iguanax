//
//  AppDelegate.h
//  IguanaController
//
//  Created by Scott C. Ripley on 2014-12-02.
//  Copyright (c) 2015 iNTERFACEWARE. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (weak) IBOutlet NSMenu *statusMenu;
@property (strong, nonatomic) NSStatusItem *statusBar;

@end

