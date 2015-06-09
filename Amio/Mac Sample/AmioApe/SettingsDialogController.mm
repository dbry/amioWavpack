/**************************************************************************
 // ADOBE SYSTEMS INCORPORATED
 // Copyright  2008 - 2011 Adobe Systems Incorporated
 // All Rights Reserved
 //
 // NOTICE:  Adobe permits you to use, modify, and distribute this 
 // file in accordance with the terms of the Adobe license agreement
 // accompanying it.  If you have received this file from a source
 // other than Adobe, then your use, modification, or distribution
 // of it requires the prior written permission of Adobe.
 **************************************************************************/

#import "SettingsDialogController.h"


bool DisplaySettingsDialog(void* inParentWindow, int& inoutSetting)
{
	NSWindow* nsWindow = (NSWindow*)inParentWindow;
	
	bool retVal = false;
	SettingsDialogController* dlgController = [[SettingsDialogController alloc] init:inoutSetting];
	if ([dlgController runModal:nsWindow])
	{
		inoutSetting = [dlgController getRadioValue];
		retVal = true;
	}
	delete dlgController;
	return retVal;	
}


@implementation SettingsDialogController

- (id)init: (int)inRadioValue {
	
	[NSBundle loadNibNamed:@"SettingsDialog" owner:self];
	
	radioValue = inRadioValue;
	canceled = true;
	return self;
}

- (bool)runModal:(NSWindow *)parentwindow {
	
	[radioMatrix selectCellWithTag:radioValue];
	[dialogWindow center];
		
	if ([dialogWindow level] != -1) {
		NSInteger windowNumberValue = [dialogWindow windowNumber];
		
		[dialogWindow makeKeyAndOrderFront:NULL];
		
		if (windowNumberValue != -1) {
			[dialogWindow orderWindow:NSWindowAbove relativeTo:windowNumberValue];				
		}
	}			
	
	NSResponder *initialResponder = [dialogWindow initialFirstResponder];
	if (initialResponder && ([dialogWindow firstResponder] == dialogWindow))
	{
		[dialogWindow makeFirstResponder:initialResponder];
		[dialogWindow setInitialFirstResponder:NULL];
	}	

	mModalSession = [NSApp beginModalSessionForWindow:dialogWindow];
	
	NSInteger runResponse = NSRunContinuesResponse;
	while (mModalSession && ((runResponse != NSRunAbortedResponse) && (runResponse != NSRunStoppedResponse)))
	{
		{
			NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
			[NSApp nextEventMatchingMask:NSAnyEventMask
							   untilDate:[[NSDate date] addTimeInterval:(0.1)]
								  inMode:NSModalPanelRunLoopMode
								 dequeue:NO];	// wait for but don't process events
			[pool release];
		}		
		runResponse = NSRunContinuesResponse;
		if (mModalSession)
		{
			NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
			runResponse = [NSApp runModalSession:mModalSession];
			[pool release];
		}
	}
	
	[dialogWindow orderOut: nil];
	
	if (mModalSession)
	{
		NSModalSession macModalSession = mModalSession;
		mModalSession = NULL;
		[NSApp endModalSession:macModalSession];
	}
	
	if (!canceled)
	{
		radioValue = -1;
		radioValue = [radioMatrix selectedTag];
		return true;
	}
	return false;
}

- (int)getRadioValue {
	return radioValue;
}

- (IBAction)doOK:(id)sender {
	canceled = false;
	if (mModalSession)
	{
		NSModalSession macModalSession = mModalSession;
		mModalSession = NULL;
		[NSApp endModalSession:macModalSession];
	}	
}

- (IBAction)doCancel:(id)sender {
	if (mModalSession)
	{
		NSModalSession macModalSession = mModalSession;
		mModalSession = NULL;
		[NSApp endModalSession:macModalSession];
	}
}

@end


