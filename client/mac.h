/*
 * mac.h
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#import <Cocoa/Cocoa.h>

extern int SDL_main(int argc, const char** argv);
extern void clean_exit(void);

@interface LMMain : NSObject
@end

@interface LMApplication : NSApplication
@end
