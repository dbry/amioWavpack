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

#import <Cocoa/Cocoa.h>

@interface SettingsDialogController : NSObject {
    IBOutlet NSMatrix* radioMatrix;
	IBOutlet NSPanel *dialogWindow;
	int radioValue;
	bool canceled;
	NSModalSession mModalSession;
}
- (id)init: (int)inRadioValue;
- (bool)runModal:(NSWindow *)parentwindow;
- (int)getRadioValue;
- (IBAction)doCancel:(id)sender;
- (IBAction)doOK:(id)sender;
@end
