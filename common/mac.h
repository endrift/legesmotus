/*
 * mac.h
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#import <Cocoa/Cocoa.h>

int SDL_main(int argc, const char** argv);
void clean_exit();

@interface LMMain : NSObject
@end

@interface LMApplication : NSApplication
@end
