/*
 * mac.m
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
 * 
 * Leges Motus is free and open source software.  You may redistribute it and/or
 * modify it under the terms of version 2, or (at your option) version 3, of the
 * GNU General Public License (GPL), as published by the Free Software Foundation.
 * 
 * Leges Motus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the full text of the GNU General Public License for
 * further detail.
 * 
 * For a full copy of the GNU General Public License, please see the COPYING file
 * in the root of the source code tree.  You may also retrieve a copy from
 * <http://www.gnu.org/licenses/gpl-2.0.txt>, or request a copy by writing to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 * 
 */

#import "mac.h"
#import <unistd.h>
#include "mac_bridge.h"
//#import <stdio.h>

// LMMain

@implementation LMMain

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
	int status;

	NSProcessInfo* pInfo = [NSProcessInfo processInfo];
	NSArray* args = [[pInfo arguments] retain];
	
	unsigned int argc = [args count];
	char const **argv = NSZoneMalloc(NSDefaultMallocZone(), argc*sizeof(char const**));
	unsigned int i;

	//FILE* log = fopen("log","w");

	// TODO see how Unicode paths are handled
	if (argc >= 2) {
		NSString *arg1 = (NSString*)[args objectAtIndex:1];
		if ([arg1 length] > 4 && [[arg1 substringToIndex:4] isEqualToString:@"-psn"]) {
			//fprintf(log,"Launched from finder %s\n",[[[NSBundle mainBundle] bundlePath] UTF8String]);
			NSString* lpath = [[NSBundle mainBundle] bundlePath];
			lpath = [lpath stringByAppendingString:@"/Contents"];
			NSString *dataPath = [lpath stringByAppendingString:@"/Resources/data"];
			NSString *execPath = [lpath stringByAppendingString:@"/MacOS"];
			setenv("LM_DATA_DIR", [dataPath UTF8String], 0);
			setenv("LM_EXEC_DIR", [execPath UTF8String], 0);
		}
	}

	for (i = 0; i < argc; ++i) {
		NSString* arg = [args objectAtIndex:i];
		argv[i] = [arg UTF8String];
		//fprintf(log,"%s\n",argv[i]);
	}

	status = SDL_main([args count], argv);

	NSZoneFree(NSDefaultMallocZone(), argv);
	[args release];

	exit(status);
}

@end

// LMApplication

@implementation LMApplication

- (void)terminate:(id)sender {
	clean_exit();
}

@end

// misc

void toplevel_exception(const char* text) {
	NSAlert* alert = [[NSAlert alertWithMessageText:[NSString stringWithUTF8String:text] defaultButton:nil alternateButton:nil
		otherButton:nil informativeTextWithFormat:@"Please ensure that Leges Motus was installed to the primary hard drive"] retain];
	[alert runModal];
	[alert release];
}

#ifdef main
#undef main
#endif

int main(int argc, char *argv[]) {
    return NSApplicationMain(argc, (const char **)argv);
}
