/*
 * virtkeyslib - Routines to support virtual keyboards and handwriting input under X.
 * Copyright (c) 2000, Merle F. McClelland for CompanionLink
 * 
 * See the files COPYRIGHT and LICENSE for distribution information.
 * 
 */

/***************************************************************************** 
 * Includes
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <X11/Xos.h>
#include <X11/Xproto.h>
#include <ctype.h>

// Note that we use this local copy here, because the installed version may or
// may not exist, and may not (yet) be up to date.

//#include "hidcd.h"
#include "libvirtkeys.h"

int debug;

// We keep these here as static variables so that Tk/TCL routines don't have to deal with them

static Display *dpy = NULL;
static KeySym *keymap = NULL;
static int minKeycode = 0;
static int maxKeycode = 0;
static int keysymsPerKeycode = 0;

// #define USEMODEMODIFIERS 0
// #define USEMODIFIERS 1

// Set USEMODIFIERS only if there is no XKB entension in the server - modifiers do not work
// the same way if it is enabled. For testing on my desktop system, and for general use, I
// leave this option off and it works fine on both XKB-enabled and non-enabled servers.

// These variables are set when we parse the Modifier Map - we look up the KeySyms
// for the modifiers and match them to the specific KeySyms for Alt and Meta (left or right)
// and remember the index for use later on.

#define noModifierMapIndex (Mod5MapIndex + 1)
#define numModifierIndexes (noModifierMapIndex + 1)
int ShiftModifierIndex = 0;
int AltModifierIndex = 0;
int MetaModifierIndex = 0;
#ifdef USEMODEMODIFER
int ModeModifierIndex = 0;
int ModeModifierMask = 0;
#else
int ModeSwitchKeyCode = 0;
#endif

// Table of modifiers

KeyCode modifierTable[numModifierIndexes];

static int createModifierTable()
{
	XModifierKeymap *modifiers;
	KeyCode *kp;
	KeyCode kc;
	KeySym ks;
	int modifier_index;
	int modifier_key;

	if (dpy == NULL)
		return FALSE;

	// Right off the bat, we check to see if the XK_Mode_switch keycode is assigned to a Keycode.
	// It must be in order for everything to work. Also, this must not be a shifted code, either.

	kc = XKeysymToKeycode(dpy, XK_Mode_switch);

	if (kc == 0)
	{
		fprintf(stderr, "Mode_switch must be assigned to an unshifted keycode and a modifier.\n");
		return FALSE;
	}

	// Now we look up the Keycode, using index 0 (unshifted). If the returned KeySym is not equal
	// to XK_Mode_switch, then it's an error.

	if (XKeycodeToKeysym(dpy, kc, 0) != XK_Mode_switch)
	{
		fprintf(stderr, "Mode_switch must be assigned to an unshifted keycode and a modifier.\n");
		return FALSE;
	}

	// We fake-out the lookup of the Mode_switch modifer for now, since the Mode_switch scheme isn't
	// directly usable when the KXB extension is used. Actually, it doesn't interfere with the use
	// of Mode_switch, but we can't generate fake events properly to do XLookupString in the
	// lookup routine (XKB uses different modifier bits for Mode_switch state). So, we worked
	// around that below, and all we have to do here is save the KeyCode for the XK_Mode_switch
	// character.

	ModeSwitchKeyCode = kc;
	
	// Get get the list of all modifiers from the server. Note that a single modifier
	// can be represented by multiple KeyCodes. For example, there are two Shift keys, each with
	// its own KeyCode. Pressing either indicates the "Shift" modifier state. We just need to
	// get the first KeyCode for each modifier, and put that KeyCode in our modifierTable for
	// use when sending the character KeyCodes. Entries in the table returned by XGetModifierMapping
	// are 0 if the entry indicates no code exists for the entry.

	modifiers = XGetModifierMapping(dpy);

	kp = modifiers->modifiermap;    

	// Now, iterate through the list, finding the first non-zero keycode for each
	// modifier index. If no modifier keycode is found, that's ok, since not all
	// modifiers exist on all systems. We have a fake modifer representing the "None"
	// case that immediately follows Mod5MapIndex in the index list and tables. This
	// doesn't correspond to any real modifier.

	for (modifier_index = 0; modifier_index < 8; modifier_index++)
	{
		modifierTable[modifier_index] = 0; // Initialze the table entry

		// Now, look through the array of modifiers for the first non-zero value

		for (modifier_key = 0; modifier_key < modifiers->max_keypermod; modifier_key++)
		{
			int keycode = kp[modifier_index * modifiers->max_keypermod + modifier_key]; 

  			if (keycode != 0)
			{
				modifierTable[modifier_index] = keycode;
				break;
			}
		}
	}

	modifierTable[noModifierMapIndex] = 0; // Initialze the "None" entry

#ifdef USEMODEMODIFIER
	// We need to find the modifier associated with the Mode_switch key. - ACTUALLY, DOESN'T WORK with XKB EXTENSION!

	ModeModifierIndex = -1;
#endif

	// Now determine which of the Mod1 through Mod5 codes correspond to the Alt, Meta, etc. modifiers

   	for (modifier_index = Mod1MapIndex; modifier_index <= Mod5MapIndex; modifier_index++)
   	{
		if (modifierTable[modifier_index])
		{
			ks = XKeycodeToKeysym(dpy, modifierTable[modifier_index], 0);
			switch (ks)
			{
			case XK_Meta_R:
			case XK_Meta_L:

				MetaModifierIndex = modifier_index;
				break;

			case XK_Alt_R:
			case XK_Alt_L:

				AltModifierIndex = modifier_index;
				break;

			case XK_Shift_R:
			case XK_Shift_L:

				ShiftModifierIndex = modifier_index;
				break;

#ifdef USEMODEMODIFER
			case XK_Mode_switch:

				ModeModifierIndex = modifier_index;
				ModeModifierMask = (1 << modifier_index);	// Create a mask for the ModeModifier 
				break;
#endif
			}
		}
   	}

#ifdef USEMODEMODIFER
	// If the Mode_switch key is not assigned to a modifier, we have an error

	if (ModeModifierIndex == -1)
		return FALSE;
#endif
	if (debug)
	{
		for (modifier_index = ShiftMapIndex; modifier_index < numModifierIndexes; modifier_index++)
		fprintf(stderr, "Keycode for modifier index %d is %x\n", modifier_index, modifierTable[modifier_index]);
		fprintf(stderr, "Meta index is %d\n", MetaModifierIndex);
		fprintf(stderr, "Alt index is %d\n", AltModifierIndex);
		fprintf(stderr, "Shift index is %d\n", ShiftModifierIndex);
#ifdef USEMODEMODIFER
		fprintf(stderr, "Mode_switch index is %d\n", ModeModifierIndex);
		fprintf(stderr, "ModeModiferMask is %d\n", ModeModifierMask);
#else
		fprintf(stderr, "Mode_switch KeyCode is %d\n", ModeSwitchKeyCode);
#endif
	}

	return TRUE;
}

// This routine looks up the KeySym table in the server. If the table contains 4 columns, it is already
// set up for use with the Mode_Shift key. If so, we just return it as-is. If not, a copy is made of the
// table into a new table that has empty definitions for the extra columns.

int loadKeySymTable() 
{
	if (dpy == NULL)
		return FALSE;

	// Now we load other variables fromthe server that we use in the other routines.

	XDisplayKeycodes(dpy, &minKeycode, &maxKeycode);
	keymap = XGetKeyboardMapping(dpy, minKeycode, 
				 	(maxKeycode - minKeycode + 1), 
				 	&keysymsPerKeycode);

	if (debug)
	{
		int k;
		int n;

		for (k = 0; k < (maxKeycode - minKeycode + 1); k++)
		{
			fprintf(stderr, "%-10d", (k + minKeycode));
			for (n = 0; n < keysymsPerKeycode; n++)
				fprintf(stderr, "%-10s\t", XKeysymToString(keymap[(k * keysymsPerKeycode + n)]));
			fprintf(stderr, "\n");
		}
	}

	// We take the KeySym table that the server gave us, and check to see if it 
	// contains 4 columns (i.e. 4 keysymsPerKeycode). If it does, the routine just
	// returns, and we will use the table as-is. If it doesn't contain 4 columns, the
	// routine copies the passed table into one that does, and returns that. That allows
	// us to utilitize the "Mode_Shift" key to access columns 2 and 3 (of 0..3), and to
	// assign those columns to any KeySyms that don't currently exist in the table. That
	// allows the program to autoconfigure the server to include definitions for KeySyms 
	// that the Keyboard config file references, whether or not they exist in the table
	// before the program runs.

 	// We check to see if the table contains 2 or 4 columns. Any other configuration is
	// NOT supported! If 4, then just return the table that was returned by the GetKeyboardMapping
	// call.

	if (keysymsPerKeycode == 4)
		return TRUE;

	if (keysymsPerKeycode == 2)
	{
		// We have to make a copy of the table by allocating one that has 4 columns instead of
		// 2, copying the table entries, and then initializing all unused entries to NoSymbol.

		int k;
		int n;
		KeySym *newKeymap = Xmalloc((maxKeycode - minKeycode + 1) * 4 * sizeof(KeySym));

		for (k = 0; k < (maxKeycode - minKeycode + 1); k++)
		{
			// Initialize the new entries

			for (n = 2; n < 4; n++)
				newKeymap[((k * 4) + n)] = NoSymbol;

			// Copy over the existing ones

			for (n = 0; n < keysymsPerKeycode; n++)
				newKeymap[((k * 4) + n)] = keymap[((k * keysymsPerKeycode) + n)];
		}

		// Indicate that the new table has 4 entries per Keycode

		keysymsPerKeycode = 4;

		// Discard the old keymap

		XFree(keymap);

		keymap = newKeymap;

		return TRUE;
	}
	else
	{
		fprintf(stderr, "Sorry - server Keyboard map doesn't contain either 2 or 4 KeySyms per Keycode - unsupported!\n");
		return FALSE;
	}
}

// This routine takes a KeySym, a pointer to a keycodeEntry table array, and an optional labelBuffer
// pointer. It looks up the specified KeySym in the table of KeySym&KeyCodes, and stores the proper
// sequence of KeyCodes that should be generated, including Modifier keys, to cause the XServer to
// generate the KeySym on the other end of the wire. The optional labelBuffer pointer will be
// set to point to an allocated buffer containing the ASCII string corresponding to the label on the
// key. Not all programs care about this, so passing NULL for the pointer means no label will be
// returned.

int lookupKeyCodeSequence(KeySym ks, struct keycodeEntry *table, char **labelBuffer)
{
	int keycode;
	int column;
	int availableKeycode;
	int availableColumn;
	int assignedKeycode;
	int assignedColumn;
	int found = FALSE;
#ifdef USEMODIFIERS
	int len;
	XEvent fakeEvent;
	int modifiers;
#else
	KeyCode ModeModifier;
#endif

	// If these aren't set, then we aren't initialized

	if ((dpy == NULL) || (keymap == NULL))
		return FALSE;

	assignedColumn = -1;
	assignedKeycode = -1;

	availableColumn = -1;
	availableKeycode = -1;

	// We do two things here - we look to see if the KeySym is already assigned, and if so,
	// save its position in the table, while at the same time looking for the next available
	// NoSymbol entry (for possible assignment).

	for (keycode = 0; ((keycode < (maxKeycode - minKeycode + 1)) && !found); keycode++)
	{
		for (column = 0; ((column < keysymsPerKeycode) && !found); column++)
		{
			if (keymap[(keycode * keysymsPerKeycode + column)] == ks)
			{
				found = TRUE;
				assignedKeycode = keycode;
				assignedColumn = column;
			}
			else if (availableColumn == -1)
			{
				// We only save the first one we find, but only if the unshifted column
				// is NOT one of the modifier keys. This is extremely important. If we tack-on
				// definitions to the columns 2 and 3 of a modifier key, expecially Shift,
				// we will actually send the wrong code if Mode_switch is followed by Shift.

				if (!IsModifierKey(keymap[(keycode * keysymsPerKeycode + 0)]))
				{
					if (keymap[(keycode * keysymsPerKeycode + column)] == NoSymbol)
					{
						availableColumn = column;
						availableKeycode = keycode;
					}
				}
			}
		}
	}

	if (!found)
	{

		if (debug)
			fprintf(stderr, "KeySym not found - will assign at Keycode %d, Column %d\n", 
				(availableKeycode + minKeycode), availableColumn);

		// We assign the KeySym to the next available NoSymbol entry, assuming there
		// is one! We can tell because availableColumn will not be -1 if we found an entry
		// that can be used.

		if (availableColumn == -1)
			return FALSE;

		// Ok. We can assign the KeySym to the entry in the table at the available Column and Keycode.
		// We must update the server when we do this, so we can look up the string associated with the
		// assigned KeySym. This can cause a lot of server thrashing the first time it's done.

		keymap[(availableKeycode * keysymsPerKeycode + availableColumn)] = ks;


		// We point to only the row that we are changing, and say that we are chaing just one. Note that
		// the keycode index passed must be based on minKeycode.

		XChangeKeyboardMapping(dpy, (availableKeycode + minKeycode), 
				keysymsPerKeycode, &keymap[(availableKeycode * keysymsPerKeycode)], 1);

		assignedKeycode = availableKeycode;
		assignedColumn  = availableColumn;

	}
	else if (debug)
		fprintf(stderr, "KeySym %x found at Keycode %d, Column %d\n", (unsigned int)ks, (assignedKeycode + minKeycode), assignedColumn);


	// If we get here, we assigned it. Now set up the table with the appropriate
	// information

#ifdef USEMODIFIERS
	modifiers = 0;
	ModeModifier = modifierTable[ModeModifierIndex]; 
#else
	ModeModifier = ModeSwitchKeyCode;
#endif

	switch (assignedColumn)
	{
	case 0:	// Unshifted case

		table[0].keycode = (assignedKeycode + minKeycode); 	// Store the keycode
		table[0].direction = keyDownUp; 	// Store the key direction (in this case, Down and Up)
		table[1].keycode = 0;	 		// Store the sequence terminator

		break;

	case 1:	// Shifted case - we have to simulate pressing down the shift modifier, 
		// then the character key, then releasing shift

		table[0].keycode = modifierTable[ShiftMapIndex];// Store the keycode for the shift key
		table[0].direction = keyDown;	 	// Store the key direction (in this case, just Down)

		table[1].keycode = (assignedKeycode + minKeycode); 	// Store the keycode
		table[1].direction = keyDownUp; 	// Store the key direction (in this case, Down and Up)

		table[2].keycode = modifierTable[ShiftMapIndex];// Store the keycode for the shift key
		table[2].direction = keyUp; 		// Store the key direction (in this case, just Up)

		table[3].keycode = 0;	 		// Store the sequence terminator

#ifdef USEMODEMODIFER
		modifiers |= ShiftMask;			// Add-in the modifier bit for the Mode_switch modifier
#endif
		break;

	case 2:	// Unshifted Mode_switch case
		table[0].keycode = ModeModifier;	// Store the keycode for the Mode switch code
		table[0].direction = keyDown; 		// Store the key direction (in this case, Down)

		table[1].keycode = (assignedKeycode + minKeycode); 	// Store the keycode
		table[1].direction = keyDownUp; 	// Store the key direction (in this case, Down and Up)

		table[2].keycode = ModeModifier;	// Store the keycode for the Mode switch code
		table[2].direction = keyUp;	 	// Store the key direction (in this case, Up)

		table[3].keycode = 0; 			// Store the sequence terminator
#ifdef USEMODIFIERS
		modifiers |= ModeModifierMask;		// Add-in the modifier bit for the Mode_switch modifier
#endif
		break;

	case 3:

		// Note that the order is important here - we first do the shift, so that the interpretation
		// of the shift key is not impacted by the ModeModifier. This accounts for key maps where the
		// interpretation of the shift key is not defined when Mode_switch is pressed first.

		table[0].keycode = modifierTable[ShiftMapIndex];// Store the keycode for the shift key
		table[0].direction = keyDown; 	// Store the key direction (in this case, just Down)

		table[1].keycode = ModeModifier;	// Store the keycode for the Mode switch code
		table[1].direction = keyDown; 		// Store the key direction (in this case, Down)

		table[2].keycode = (assignedKeycode + minKeycode); 	// Store the keycode
		table[2].direction = keyDownUp; 	// Store the key direction (in this case, Down and Up)

		table[3].keycode = ModeModifier;	// Store the keycode for the Mode switch code
		table[3].direction = keyUp; 		// Store the key direction (in this case, Up)

		table[4].keycode = modifierTable[ShiftMapIndex];// Store the keycode for the shift key
		table[4].direction = keyUp; 	// Store the key direction (in this case, just Up)

		table[5].keycode = 0; 		// Store the sequence terminator

#ifdef USEMODEMODIFER
		modifiers |= (ShiftMask | ModeModifierMask);	// Add-in the modifier bit for the Mode_switch and Shift modifiers
#endif
		break;

	}

	// If the server is compiled with XKB, this does not work!!! The XKB extension uses additional state bits for
	// Mode_switch, and the use of the ModeModifierMask doesn't work. So, we just interpret the KeySym directly for
	// the label string.

#ifdef USEMODEMODIFER
	// Now look up the string that represents the keycode in the correct state, taking
	// into account the Shift and Mode_switch modifiers (set above).

	fakeEvent.xkey.type = KeyPress;
	fakeEvent.xkey.display = dpy;
	fakeEvent.xkey.time = CurrentTime;
	fakeEvent.xkey.x = fakeEvent.xkey.y = 0;
	fakeEvent.xkey.x_root = fakeEvent.xkey.y_root = 0;
	fakeEvent.xkey.state = modifiers;
	fakeEvent.xkey.keycode = (assignedKeycode + minKeycode);

	if (labelBuffer)
	{
		*labelBuffer = malloc(MAXLABELLEN+1);

		len = XLookupString((XKeyEvent *)&fakeEvent, *labelBuffer, MAXLABELLEN, NULL, NULL);

		(*labelBuffer)[len] = '\0';

		if (debug)
			fprintf(stderr, "modifiers = %x, keycode = %d, len = %d, labelBuffer = '%s'\n", 
				modifiers, fakeEvent.xkey.keycode, len, (len > 0 ? *labelBuffer : "(null)"));
	}
#else
	if (labelBuffer)
	{
		*labelBuffer = malloc(2);
		if ((ks & 0xff00) == 0xff00)
			(*labelBuffer)[0] = ks;
		else
			(*labelBuffer)[0] = ks & 0xff;
		(*labelBuffer)[1] = '\0';
		if (debug)
			fprintf(stderr, "labelBuffer = '%s'\n", *labelBuffer);
	}
#endif

	return TRUE;
}

// Routine to test for and set up the XTest extension. Returns FALSE if the set up fails or if the extension
// isn't installed.

int setupXTest()
{
	int event, error;
	int major, minor;

	if (dpy == NULL)
		return FALSE;

	// does the display have the Xtest-extension?

	if (!XTestQueryExtension(dpy, &event, &error, &major, &minor))
	{
        	// nope, extension not supported

        	fprintf(stderr, "XTest extension not supported on server \"%s\"\n.", DisplayString(dpy));

		return FALSE;
	}

	// sync the server
	XSync(dpy, True);

	return TRUE;
}

void closeXTest()
{
	if (dpy == NULL)
		return;

	// discard and even flush all events on the remote display

	XTestDiscard(dpy);

	XFlush(dpy);
}


void sendKeySequence(struct keycodeEntry *entries, int controlMode, int metaMode, int altMode, int shiftMode)
{
	int s = 0;
	KeyCode kc;

	if (entries == NULL)
		return;

	// The Control, Meta, and Alt modifiers are set and unset outside the scope of the
	// sequences in the table. The table sequences determine which column in the key table
	// is selected, whereas the control, meta, and alt keys do not (they just set modifier
	// bits in the receiving application. Thus, we press and unpress these modifiers before
	// and after sending the sequence.

	if (controlMode)
		sendKey(modifierTable[ControlMapIndex], keyDown); 	// Send a down event for the control modifier key

	if (metaMode)
		sendKey(modifierTable[MetaModifierIndex], keyDown); 	// Send a down event for the meta modifier key

	if (altMode)
		sendKey(modifierTable[AltModifierIndex], keyDown); 	// Send a down event for the alt modifier key

	if (shiftMode)
		sendKey(modifierTable[ShiftModifierIndex], keyDown); 	// Send a down event for the shift modifier key

        while ((kc = entries[s].keycode))
        {
        	enum keyDirection kd = entries[s].direction;

		sendKey(kc, kd);
        	s++;
        }

	// Now send the corresponding up events for the modifiers

	if (controlMode)
		sendKey(modifierTable[ControlMapIndex], keyUp); 	// Send an up event for the control modifier key

	if (metaMode)
		sendKey(modifierTable[MetaModifierIndex], keyUp); 	// Send an up event for the meta modifier key

	if (altMode)
		sendKey(modifierTable[AltModifierIndex], keyUp); 	// Send an up event for the alt modifier key

	if (shiftMode)
		sendKey(modifierTable[ShiftModifierIndex], keyUp); 	// Send an up event for the shift modifier key

}

void sendKey(KeyCode character, enum keyDirection keydirection)
{
	if (dpy == NULL)
		return;

    	switch (keydirection)
    	{
    	case keyDown:
	
        	if (debug)
                	fprintf(stderr, "sending %04x key down\n", character);
	
		//XTestFakeKeyEvent(dpy, (unsigned int) character, TRUE, 0);
		//bthid_send(character, 1);
        	break;
	
    	case keyUp:
	
        	if (debug)
                	fprintf(stderr, "sending %04x key up\n", character);
	
		//XTestFakeKeyEvent(dpy, (unsigned int) character, FALSE, 0);
		//bthid_send(character, 0);
        	break;
	
    	case keyDownUp:
	
        	if (debug)
                	fprintf(stderr, "sending %04x key down\n", character);
	
		//XTestFakeKeyEvent(dpy, (unsigned int) character, TRUE, 0);
		//bthid_send(character, 1);
	
        	if (debug)
                	fprintf(stderr, "sending %04x key up\n", character);
	
		//XTestFakeKeyEvent(dpy, (unsigned int) character, FALSE, 0);
		//bthid_send(character, 0);
        	break;
    	}
}

//
// This routine does the basic setup needed for loading X server key tables and such
//

int setupKeyboardVariables(Display *display)
{
	// If the dpy variable is set, we've already been called once. Just return.

	if (dpy)
		return TRUE;

	// Get the Keyboard Mapping table. This is indexed by keycode in one
	// direction, and by the modifier index in the other. The loadKeyboardTable
	// routine will take these two tables and convert the config file into a lookup
	// table between stroke sequences and keycode/modifier keycode pairs.

	// We set up a local static variable used by all of these routines. It is done this
	// way for easy integration into Tk/TCL code, which quite often has no notion of
	// the X display variable.

	dpy = display;
	
	// Call to test for and set up the XTest extension

	if (debug)
		fprintf(stderr, "Setting up XTest\n");

	if (setupXTest() == FALSE)
		return FALSE;

        // Load the modifer map

	if (debug)
		fprintf(stderr, "Creating modifier table\n");
	
	if (createModifierTable() == FALSE)
		return FALSE;

	if (debug)
		fprintf(stderr, "Loading KeySym table\n");

	if (loadKeySymTable() == FALSE)
		return FALSE;

	return TRUE;
}

