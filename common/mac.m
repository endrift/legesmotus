/*
 * mac.m
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#import <Cocoa/Cocoa.h>
#import <unistd.h>
//#import <stdio.h>

// LMMain

@interface LMMain : NSObject
@end

@implementation LMMain

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
	int status;

	NSProcessInfo* pInfo = [NSProcessInfo processInfo];
	NSArray* args = [[pInfo arguments] retain];
	
	NSUInteger argc = [args count];
	char const **argv = NSZoneMalloc(NSDefaultMallocZone(),argc);
	NSUInteger i;

	//FILE* log = fopen("log","w");

	// TODO see how Unicode paths are handled
	if (argc >= 2 && [[(NSString*)[args objectAtIndex:1] substringToIndex:4] isEqualToString:@"-psn"]) {
		//fprintf(log,"Launched from finder %s\n",[[[NSBundle mainBundle] bundlePath] UTF8String]);
		NSString* lpath = [[NSBundle mainBundle] bundlePath];
		lpath = [lpath stringByAppendingString:@"/.."];
		NSString *dataPath = [lpath stringByAppendingString:@"/data"];
		setenv("LM_DATA_DIR",[dataPath UTF8String],0);
		// TODO LM_EXEC_DIR
	}

	for (i = 0; i < argc; ++i) {
		NSString* arg = [args objectAtIndex:i];
		argv[i] = [arg UTF8String];
		//fprintf(log,"%s\n",argv[i]);
	}

	status = SDL_main([args count],argv);

	NSZoneFree(NSDefaultMallocZone(),argv);
	[args release];

	exit(status);
}

@end

// LMApplication

@interface LMApplication : NSApplication
@end

@implementation LMApplication

- (void)terminate:(id)sender {
	raise(SIGINT);
}

@end

// main

#ifdef main
#undef main
#endif

int main(int argc, char *argv[]) {
	[LMApplication poseAsClass:[NSApplication class]];
    return NSApplicationMain(argc,  (const char **) argv);
}
