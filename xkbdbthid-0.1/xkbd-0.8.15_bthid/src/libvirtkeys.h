/*
 * virtkeyslib.h - Definitions for the virtkeys library.
 * Copyright (c) 2000, Merle F. McClelland for CompanionLink
 * 
 * See the files COPYRIGHT and LICENSE for distribution information.
 *
 * Note that these routines are somewhat oriented to a virtual keyboard
 * design, but are also intended to support handwriting input windows
 * that have very similar needs.
 * 
 */

#define SHOWSEQUENCES
#include <sys/param.h>

extern int debug;

#ifndef TRUE
#define	TRUE	1
#define	FALSE	0
#endif

enum keyType { regularKey, shiftKey, capsKey, controlKey, metaKey, altKey, specialCharKey, numKeyTypes };

enum modifierMode { unshiftedMode, shiftedMode, unshiftedSpecialCharMode, shiftedSpecialCharMode, numModes };

enum keyDirection { keyDown, keyUp, keyDownUp };

struct keycodeEntry
{
	    KeyCode 		keycode;
	    enum keyDirection     	direction;		// Indicates key down, key up, or down/up
};

#define MAXKEYCODES	10
#define MAXLABELLEN	10

struct keyDescriptionEntry
{
	enum keyType	theType;
	int 		col;
	int		row;
	int		width;

	// The following array is not used for the modifier keys Shift, Caps, and SpecialChar, as pressing
	// these keys do not actually send key codes. 

	struct 		keycodeEntry outputSequence[numModes][MAXKEYCODES];	// Terminated with 0
	void		*objectData;	// Can be used to hold a pointer to data associated with the key
	char		*label[numModes];
	int		labelIsPath;
	int		regularCAPS;	// Indicates that the key, in regular mode, responds to the CAPS lock modifier
	int		specialCAPS;	// Indicates that the key, in special mode, responds to the CAPS lock modifier
};

#define MAXLINECHARS    300

#ifdef __cplusplus
extern "C" {
#endif

int setupKeyboardVariables(Display *dpy);

int lookupKeyCodeSequence(KeySym ks, struct keycodeEntry *keycodeEntryTable, char **labelBuffer);

int loadKeySymTable();

// Routines to manage and use the XTest extension

int setupXTest();

void closeXTest();

// And these routines actually send sequences of keycodes and up/down indicators via the XTest extension

void sendKeySequence(struct keycodeEntry *entries, int controlMode, int metaMode, int altMode, int shiftMode);


void sendKey(KeyCode character, enum keyDirection keydirection);

#ifdef __cplusplus
}
#endif
