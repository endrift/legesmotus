/*
 * mac.m
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#import "mac.h"
#import <unistd.h>
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
			lpath = [lpath stringByAppendingString:@"/.."];
			NSString *dataPath = [lpath stringByAppendingString:@"/data"];
			setenv("LM_DATA_DIR", [dataPath UTF8String], 0);
			// TODO LM_EXEC_DIR
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

// main

#ifdef main
#undef main
#endif

int main(int argc, char *argv[]) {
    return NSApplicationMain(argc, (const char **)argv);
}
