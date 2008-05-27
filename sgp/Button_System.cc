// Rewritten mostly by Kris Morness
#include  <stdexcept>

#include "Button_Sound_Control.h"
#include "Button_System.h"
#include "Debug.h"
#include "Font.h"
#include "HImage.h"
#include "Input.h"
#include "MemMan.h"
#include "VObject.h"
#include "VObject_Blitters.h"
#include "VSurface.h"
#include "Video.h"
#include "WCheck.h"
#include "WordWrap.h"

#ifdef _JA2_RENDER_DIRTY
#	include "Font_Control.h"
#endif


// Names of the default generic button image files.
#define DEFAULT_GENERIC_BUTTON_OFF    "GENBUTN.STI"
#define DEFAULT_GENERIC_BUTTON_ON     "GENBUTN2.STI"
#define DEFAULT_GENERIC_BUTTON_OFF_HI "GENBUTN3.STI"
#define DEFAULT_GENERIC_BUTTON_ON_HI  "GENBUTN4.STI"


#define MSYS_STARTING_CURSORVAL 0


#define MAX_BUTTON_ICONS 40


#define GUI_BTN_NONE           0
#define GUI_BTN_DUPLICATE_VOBJ 1


/* Kris:  December 2, 1997
 * Special internal debugging utilities that will ensure that you don't attempt
 * to delete an already deleted button, or it's images, etc.  It will also
 * ensure that you don't create the same button that already exists.
 * TO REMOVE ALL DEBUG FUNCTIONALITY: simply comment out BUTTONSYSTEM_DEBUGGING
 * definition
 */
#if defined JA2 && defined _DEBUG
#	define BUTTONSYSTEM_DEBUGGING
#endif


#ifdef BUTTONSYSTEM_DEBUGGING

// Called immediately before assigning the button to the button list.
static void AssertFailIfIdenticalButtonAttributesFound(const GUI_BUTTON* b)
{
	for (INT32 x = 0; x < MAX_BUTTONS; ++x)
	{
		const GUI_BUTTON* const c = ButtonList[x];
		if (!c)                                                       continue;
		if (c->uiFlags                 &   BUTTON_DELETION_PENDING  ) continue;
		if (c->uiFlags                 &   BUTTON_NO_DUPLICATE      ) continue;
		if (b->Area.PriorityLevel      != c->Area.PriorityLevel     ) continue;
		if (b->Area.RegionTopLeftX     != c->Area.RegionTopLeftX    ) continue;
		if (b->Area.RegionTopLeftY     != c->Area.RegionTopLeftY    ) continue;
		if (b->Area.RegionBottomRightX != c->Area.RegionBottomRightX) continue;
		if (b->Area.RegionBottomRightY != c->Area.RegionBottomRightY) continue;
		if (b->ClickCallback           != c->ClickCallback          ) continue;
		if (b->MoveCallback            != c->MoveCallback           ) continue;
		if (b->XLoc                    != c->XLoc                   ) continue;
		if (b->YLoc                    != c->YLoc                   ) continue;
		/* if we get this far, it is reasonably safe to assume that the newly
		 * created button already exists.  Placing a break point on the following
		 * assert will allow the coder to easily isolate the case!
		 */
		AssertMsg(0, String("Attempting to create a button that has already been created (existing buttonID %d).", c->IDNum));
	}
}

#endif


/* Kris:
 * These are the variables used for the anchoring of a particular button.  When
 * you click on a button, it get's anchored, until you release the mouse button.
 * When you move around, you don't want to select other buttons, even when you
 * release it.  This follows the Windows 95 convention.
 */
static GUI_BUTTON* gpAnchoredButton;
static GUI_BUTTON* gpPrevAnchoredButton;
static BOOLEAN gfAnchoredState;

static INT8 gbDisabledButtonStyle;

BOOLEAN gfRenderHilights = TRUE;

// Struct definition for the QuickButton pictures.
struct BUTTON_PICS
{
	HVOBJECT vobj;      // The Image itself
	INT32    Grayed;    // Index to use for a "Grayed-out" button
	INT32    OffNormal; // Index to use when button is OFF
	INT32    OffHilite; // Index to use when button is OFF w/ hilite on it
	INT32    OnNormal;  // Index to use when button is ON
	INT32    OnHilite;  // Index to use when button is ON w/ hilite on it
	ButtonDimensions max; // width/height of largest image in use
	UINT32   fFlags;    // Special image flags
};

static BUTTON_PICS ButtonPictures[MAX_BUTTON_PICS];

SGPVSurface* ButtonDestBuffer;

GUI_BUTTON* ButtonList[MAX_BUTTONS];


const ButtonDimensions* GetDimensionsOfButtonPic(const BUTTON_PICS* const pics)
{
	return &pics->max;
}


static HVOBJECT GenericButtonOffNormal;
static HVOBJECT GenericButtonOffHilite;
static HVOBJECT GenericButtonOnNormal;
static HVOBJECT GenericButtonOnHilite;
static UINT16   GenericButtonFillColors;

static HVOBJECT GenericButtonIcons[MAX_BUTTON_ICONS];

static BOOLEAN gfDelayButtonDeletion   = FALSE;
static BOOLEAN gfPendingButtonDeletion = FALSE;

extern MOUSE_REGION* MSYS_PrevRegion;


// Finds an available slot for loading button pictures
static BUTTON_PICS* FindFreeButtonSlot(void)
{
	// Search for a slot
	for (BUTTON_PICS* i = ButtonPictures; i != endof(ButtonPictures); ++i)
	{
		if (i->vobj == NULL) return i;
	}
	throw std::runtime_error("Out of button image slots");
}


static void SetMaxSize(BUTTON_PICS* const pics, const INT32 img_idx)
{
	if (img_idx == BUTTON_NO_IMAGE) return;
	ETRLEObject const* const e = pics->vobj->SubregionProperties(img_idx);
	const UINT32             w = e->sOffsetX + e->usWidth;
	const UINT32             h = e->sOffsetY + e->usHeight;
	if (pics->max.w < w) pics->max.w = w;
	if (pics->max.h < h) pics->max.h = h;
}


static void InitButtonImage(BUTTON_PICS* const pics, const HVOBJECT VObj, const UINT32 Flags, const INT32 Grayed, const INT32 OffNormal, const INT32 OffHilite, const INT32 OnNormal, const INT32 OnHilite)
{
	pics->vobj = VObj;

	// Init the QuickButton image structure with indexes to use
	pics->Grayed    = Grayed;
	pics->OffNormal = OffNormal;
	pics->OffHilite = OffHilite;
	pics->OnNormal  = OnNormal;
	pics->OnHilite  = OnHilite;
	pics->fFlags    = Flags;

	// Fit the button size to the largest image in the set
	pics->max.w = 0;
	pics->max.h = 0;
	SetMaxSize(pics, Grayed);
	SetMaxSize(pics, OffNormal);
	SetMaxSize(pics, OffHilite);
	SetMaxSize(pics, OnNormal);
	SetMaxSize(pics, OnHilite);
}


BUTTON_PICS* LoadButtonImage(const char* filename, INT32 Grayed, INT32 OffNormal, INT32 OffHilite, INT32 OnNormal, INT32 OnHilite)
{
	AssertMsg(filename != NULL, "Attempting to LoadButtonImage() with null filename.");

	if (Grayed    == BUTTON_NO_IMAGE &&
			OffNormal == BUTTON_NO_IMAGE &&
			OffHilite == BUTTON_NO_IMAGE &&
			OnNormal  == BUTTON_NO_IMAGE &&
			OnHilite  == BUTTON_NO_IMAGE)
	{
		throw std::logic_error("No button pictures selected");
	}

	BUTTON_PICS* const UseSlot = FindFreeButtonSlot();
	SGPVObject*  const VObj    = AddVideoObjectFromFile(filename);
	InitButtonImage(UseSlot, VObj, GUI_BTN_NONE, Grayed, OffNormal, OffHilite, OnNormal, OnHilite);
	return UseSlot;
}


BUTTON_PICS* UseLoadedButtonImage(BUTTON_PICS* const LoadedImg, const INT32 Grayed, const INT32 OffNormal, const INT32 OffHilite, const INT32 OnNormal, const INT32 OnHilite)
{
	if (Grayed    == BUTTON_NO_IMAGE &&
			OffNormal == BUTTON_NO_IMAGE &&
			OffHilite == BUTTON_NO_IMAGE &&
			OnNormal  == BUTTON_NO_IMAGE &&
			OnHilite  == BUTTON_NO_IMAGE)
	{
		throw std::logic_error("No button pictures selected for pre-loaded button image");
	}

	// Is button image index given valid?
	const HVOBJECT vobj = LoadedImg->vobj;
	if (!vobj)
	{
		throw std::logic_error("Invalid button picture handle given for pre-loaded button image");
	}

	BUTTON_PICS* const UseSlot = FindFreeButtonSlot();
	InitButtonImage(UseSlot, vobj, GUI_BTN_DUPLICATE_VOBJ, Grayed, OffNormal, OffHilite, OnNormal, OnHilite);
	return UseSlot;
}


void UnloadButtonImage(BUTTON_PICS* const pics)
{
#if defined BUTTONSYSTEM_DEBUGGING
	AssertMsg(pics->vobj != NULL, "Attempting to UnloadButtonImage that has a null vobj (already deleted).");
#endif
	if (pics->vobj == NULL) return;

	// If this is a duplicated button image, then don't trash the vobject
	if (!(pics->fFlags & GUI_BTN_DUPLICATE_VOBJ))
	{
		/* Deleting a non-duplicate, so see if any dups present. if so, then convert
		 * one of them to an original!
		 */
		for (BUTTON_PICS* other = ButtonPictures; other != endof(ButtonPictures); ++other)
		{
			if (other == pics) continue;
			if (other->vobj != pics->vobj) continue;
			if (!(other->fFlags & GUI_BTN_DUPLICATE_VOBJ)) continue;

			/* If we got here, then we got a duplicate object of the one we want to
			 * delete, so convert it to an original!
			 */
			other->fFlags &= ~GUI_BTN_DUPLICATE_VOBJ;

			// Now remove this button, but not its vobject
			goto remove_pic;
		}

		DeleteVideoObject(pics->vobj);
	}

remove_pic:
	pics->vobj = NULL;
}


void EnableButton(GUIButtonRef const b)
{
	CHECKV(b != NULL); // XXX HACK000C
	b->uiFlags |= BUTTON_ENABLED | BUTTON_DIRTY;
}


void DisableButton(GUIButtonRef const b)
{
	CHECKV(b != NULL); // XXX HACK000C
	b->uiFlags &= ~BUTTON_ENABLED;
	b->uiFlags |= BUTTON_DIRTY;
}


/* Initializes the button image sub-system. This function is called by
 * InitButtonSystem.
 */
static BOOLEAN InitializeButtonImageManager(void)
try
{
	// Blank out all QuickButton images
	for (int x = 0; x < MAX_BUTTON_PICS; ++x)
	{
		BUTTON_PICS* const pics = &ButtonPictures[x];
		pics->vobj      = NULL;
		pics->Grayed    = -1;
		pics->OffNormal = -1;
		pics->OffHilite = -1;
		pics->OnNormal  = -1;
		pics->OnHilite  = -1;
	}

	// Blank out all Generic button data
	GenericButtonOffNormal  = NULL;
	GenericButtonOffHilite  = NULL;
	GenericButtonOnNormal   = NULL;
	GenericButtonOnHilite   = NULL;
	GenericButtonFillColors = 0;

	// Blank out all icon images
	for (int x = 0; x < MAX_BUTTON_ICONS; ++x)
		GenericButtonIcons[x] = NULL;

	// Load the default generic button images
	GenericButtonOffNormal = AddVideoObjectFromFile(DEFAULT_GENERIC_BUTTON_OFF);
	GenericButtonOnNormal  = AddVideoObjectFromFile(DEFAULT_GENERIC_BUTTON_ON);

	/* Load up the off hilite and on hilite images. We won't check for errors
	 * because if the file doesn't exists, the system simply ignores that file.
	 * These are only here as extra images, they aren't required for operation
	 * (only OFF Normal and ON Normal are required).
	 */
	try
	{
		GenericButtonOffHilite = AddVideoObjectFromFile(DEFAULT_GENERIC_BUTTON_OFF_HI);
	}
	catch (...) { /* see comment above */ }
	try
	{
		GenericButtonOnHilite  = AddVideoObjectFromFile(DEFAULT_GENERIC_BUTTON_ON_HI);
	}
	catch (...) { /* see comment above */ }

	UINT8 Pix = 0;
	if (!GetETRLEPixelValue(&Pix, GenericButtonOffNormal, 8, 0, 0))
	{
		DebugMsg(TOPIC_BUTTON_HANDLER, DBG_LEVEL_0, "Couldn't get generic button's background pixel value");
		return FALSE;
	}

	GenericButtonFillColors = GenericButtonOffNormal->Palette16()[Pix];

	return TRUE;
}
catch (...) { return FALSE; }


// Finds the next available slot for button icon images.
static INT16 FindFreeIconSlot(void)
{
	for (INT16 x = 0; x < MAX_BUTTON_ICONS; ++x)
	{
		if (GenericButtonIcons[x] == NULL) return x;
	}
	throw std::runtime_error("Out of generic button icon slots");
}


INT16 LoadGenericButtonIcon(const char* filename)
{
	AssertMsg(filename != NULL, "Attempting to LoadGenericButtonIcon() with null filename.");

	// Get slot for icon image
	INT16 const ImgSlot = FindFreeIconSlot();

	// Load the icon
	GenericButtonIcons[ImgSlot] = AddVideoObjectFromFile(filename);

	// Return the slot number
	return ImgSlot;
}


void UnloadGenericButtonIcon(INT16 GenImg)
{
	AssertMsg(0 <= GenImg && GenImg < MAX_BUTTON_ICONS, String("Attempting to UnloadGenericButtonIcon with out of range index %d.", GenImg));

#if defined BUTTONSYSTEM_DEBUGGING
	AssertMsg(GenericButtonIcons[GenImg], "Attempting to UnloadGenericButtonIcon that has no icon (already deleted).");
#endif
	if (!GenericButtonIcons[GenImg]) return;
	// If an icon is present in the slot, remove it.
	DeleteVideoObject(GenericButtonIcons[GenImg]);
	GenericButtonIcons[GenImg] = NULL;
}


// Cleans up, and shuts down the button image manager sub-system.
static void ShutdownButtonImageManager(void)
{
	// Remove all QuickButton images
	for (BUTTON_PICS* i = ButtonPictures; i != endof(ButtonPictures); ++i)
	{
		if (i->vobj != NULL) UnloadButtonImage(i);
	}

	// Remove all GenericButton images
	if (GenericButtonOffNormal != NULL)
	{
		DeleteVideoObject(GenericButtonOffNormal);
		GenericButtonOffNormal = NULL;
	}

	if (GenericButtonOffHilite != NULL)
	{
		DeleteVideoObject(GenericButtonOffHilite);
		GenericButtonOffHilite = NULL;
	}

	if (GenericButtonOnNormal != NULL)
	{
		DeleteVideoObject(GenericButtonOnNormal);
		GenericButtonOnNormal = NULL;
	}

	if (GenericButtonOnHilite != NULL)
	{
		DeleteVideoObject(GenericButtonOnHilite);
		GenericButtonOnHilite = NULL;
	}

	GenericButtonFillColors = 0;

	// Remove all button icons
	for (int x = 0; x < MAX_BUTTON_ICONS; ++x)
	{
		if (GenericButtonIcons[x] != NULL) UnloadGenericButtonIcon(x);
	}
}


BOOLEAN InitButtonSystem(void)
{
	ButtonDestBuffer = FRAME_BUFFER;

	// Clear out button list
	for (INT32 x = 0; x < MAX_BUTTONS; ++x)
	{
		ButtonList[x] = NULL;
	}

	// Initialize the button image manager sub-system
	if (!InitializeButtonImageManager())
	{
		DebugMsg(TOPIC_BUTTON_HANDLER, DBG_LEVEL_0, "Failed button image manager init\n");
		return FALSE;
	}

	return TRUE;
}


static void RemoveButtonInternal(INT32 const iButtonID);


void ShutdownButtonSystem(void)
{
	// Kill off all buttons in the system
	for (int x = 0; x < MAX_BUTTONS; ++x)
	{
		if (ButtonList[x] != NULL) RemoveButtonInternal(x);
	}
	ShutdownButtonImageManager();
}


static void RemoveButtonsMarkedForDeletion(void)
{
	for (INT32 i = 0; i < MAX_BUTTONS; ++i)
	{
		if (ButtonList[i] && ButtonList[i]->uiFlags & BUTTON_DELETION_PENDING)
		{
			RemoveButtonInternal(i);
		}
	}
}


static void RemoveButtonInternal(INT32 const btn_id)
{
	CHECKV(0 <= btn_id && btn_id < MAX_BUTTONS); // XXX HACK000C
	AssertMsg(0 <= btn_id && btn_id < MAX_BUTTONS, String("ButtonID %d is out of range.", btn_id));
	GUI_BUTTON* const b = ButtonList[btn_id];
	CHECKV(b != NULL); // XXX HACK000C
	AssertMsg(b != NULL, String("Accessing non-existent button %d.", btn_id));

	/* If we happen to be in the middle of a callback, and attempt to delete a
	 * button, like deleting a node during list processing, then we delay it till
	 * after the callback is completed.
	 */
	if (gfDelayButtonDeletion)
	{
		b->uiFlags |= BUTTON_DELETION_PENDING;
		gfPendingButtonDeletion = TRUE;
		return;
	}

	//Kris:
	if (b->uiFlags & BUTTON_SELFDELETE_IMAGE)
	{
		/* Checkboxes and simple create buttons have their own graphics associated
		 * with them, and it is handled internally.  We delete it here.  This
		 * provides the advantage of less micromanagement, but with the
		 * disadvantage of wasting more memory if you have lots of buttons using the
		 * same graphics.
		 */
		UnloadButtonImage(b->image);
	}

	MSYS_RemoveRegion(&b->Area);

	// Get rid of the text string
	if (b->string != NULL) MemFree(b->string);

	if (b == gpAnchoredButton)     gpAnchoredButton     = NULL;
	if (b == gpPrevAnchoredButton) gpPrevAnchoredButton = NULL;

	MemFree(b);
	ButtonList[btn_id] = NULL;
}


void RemoveButton(GUIButtonRef& btn_ref)
{
	GUIButtonRef const btn = btn_ref;
	btn_ref.Reset();
	RemoveButtonInternal(btn.ID());
}


// Finds the next available button slot.
static INT32 GetNextButtonNumber(void)
{
	/* Never hand out ID 0.  Slot 0 is always a null pointer */
	for (INT32 x = 1; x < MAX_BUTTONS; x++)
	{
		if (ButtonList[x] == NULL) return x;
	}
	throw std::runtime_error("No more button slots");
}


static void QuickButtonCallbackMButn(MOUSE_REGION* reg, INT32 reason);
static void QuickButtonCallbackMMove(MOUSE_REGION* reg, INT32 reason);


static GUI_BUTTON* AllocateButton(const UINT32 Flags, const INT16 Left, const INT16 Top, const INT16 Width, const INT16 Height, const INT8 Priority, const GUI_CALLBACK Click, const GUI_CALLBACK Move)
{
	AssertMsg(Left >= 0 && Top >= 0 && Width >= 0 && Height >= 0, String("Attempting to create button with invalid coordinates %dx%d+%dx%d", Left, Top, Width, Height));

	INT32 const BtnID = GetNextButtonNumber();

	GUI_BUTTON* const b = MALLOC(GUI_BUTTON);
	if (!b) throw std::bad_alloc();

	b->IDNum                   = BtnID;
	b->image                   = NULL;
	b->ClickCallback           = Click;
	b->MoveCallback            = Move;
	b->uiFlags                 = BUTTON_DIRTY | BUTTON_ENABLED | Flags;
	b->uiOldFlags              = 0;
	b->XLoc                    = Left;
	b->YLoc                    = Top;
	b->User.Data               = 0;
	b->bDisabledStyle          = GUI_BUTTON::DISABLED_STYLE_DEFAULT;
	b->string                  = NULL;
	b->usFont                  = 0;
	b->sForeColor              = 0;
	b->sShadowColor            = -1;
	b->sForeColorDown          = -1;
	b->sShadowColorDown        = -1;
	b->sForeColorHilited       = -1;
	b->sShadowColorHilited     = -1;
	b->bJustification          = GUI_BUTTON::TEXT_CENTER;
	b->bTextXOffset            = -1;
	b->bTextYOffset            = -1;
	b->bTextXSubOffSet         = 0;
	b->bTextYSubOffSet         = 0;
	b->fShiftText              = TRUE;
	b->sWrappedWidth           = -1;
	b->icon                    = 0;
	b->usIconIndex             = -1;
	b->bIconXOffset            = -1;
	b->bIconYOffset            = -1;
	b->fShiftImage             = TRUE;
	b->ubToggleButtonActivated = FALSE;

	memset(&b->Area, 0, sizeof(b->Area));
	MSYS_DefineRegion(
		&b->Area,
		Left,
		Top,
		Left + Width,
		Top  + Height,
		Priority,
		MSYS_STARTING_CURSORVAL,
		QuickButtonCallbackMMove,
		QuickButtonCallbackMButn
	);
	b->Area.SetUserPtr(b);

#ifdef BUTTONSYSTEM_DEBUGGING
	AssertFailIfIdenticalButtonAttributesFound(b);
#endif

	ButtonList[BtnID] = b;

	SpecifyButtonSoundScheme(b, BUTTON_SOUND_SCHEME_GENERIC);

	return b;
}


static void CopyButtonText(GUI_BUTTON* b, const wchar_t* text)
{
	if (text == NULL || text[0] == L'\0') return;

	wchar_t* const Buf = MALLOCN(wchar_t, wcslen(text) + 1);
	AssertMsg(Buf != NULL, "Out of memory error:  Couldn't allocate string in CopyButtonText.");
	wcscpy(Buf, text);
	b->string = Buf;
}


static void DefaultMoveCallback(GUI_BUTTON* btn, INT32 reason);


GUIButtonRef CreateIconButton(INT16 Icon, INT16 IconIndex, INT16 xloc, INT16 yloc, INT16 w, INT16 h, INT16 Priority, GUI_CALLBACK ClickCallback)
{
	// if button size is too small, adjust it.
	if (w < 4) w = 4;
	if (h < 3) h = 3;

	GUI_BUTTON* const b = AllocateButton(BUTTON_GENERIC, xloc, yloc, w, h, Priority, ClickCallback, DefaultMoveCallback);
	b->icon        = GenericButtonIcons[Icon];
	b->usIconIndex = IconIndex;
	return b;
}


GUIButtonRef CreateTextButton(const wchar_t *string, Font const font, INT16 sForeColor, INT16 sShadowColor, INT16 xloc, INT16 yloc, INT16 w, INT16 h, INT16 Priority, GUI_CALLBACK ClickCallback)
{
	// if button size is too small, adjust it.
	if (w < 4) w = 4;
	if (h < 3) h = 3;

	GUI_BUTTON* const b = AllocateButton(BUTTON_GENERIC, xloc, yloc, w, h, Priority, ClickCallback, DefaultMoveCallback);
	CopyButtonText(b, string);
	b->usFont       = font;
	b->sForeColor   = sForeColor;
	b->sShadowColor = sShadowColor;
	return b;
}


GUIButtonRef CreateHotSpot(INT16 xloc, INT16 yloc, INT16 Width, INT16 Height, INT16 Priority, GUI_CALLBACK ClickCallback)
{
	return AllocateButton(BUTTON_HOT_SPOT, xloc, yloc, Width, Height, Priority, ClickCallback, DefaultMoveCallback);
}


void GUI_BUTTON::SetCursor(UINT16 const cursor)
{
	Area.Cursor = cursor;
}


static GUIButtonRef QuickCreateButtonInternal(BUTTON_PICS* const pics, const INT16 xloc, const INT16 yloc, const INT32 Type, const INT16 Priority, const GUI_CALLBACK MoveCallback, const GUI_CALLBACK ClickCallback)
{
	// Is there a QuickButton image in the given image slot?
	if (!pics->vobj)
	{
		throw std::runtime_error("QuickCreateButton: Invalid button image");
	}

	GUI_BUTTON* const b = AllocateButton((Type & (BUTTON_CHECKBOX | BUTTON_NEWTOGGLE)) | BUTTON_QUICK, xloc, yloc, pics->max.w, pics->max.h, Priority, ClickCallback, MoveCallback);
	b->image = pics;
	return b;
}


GUIButtonRef QuickCreateButton(BUTTON_PICS* const image, const INT16 x, const INT16 y, const INT16 priority, const GUI_CALLBACK click)
{
	return QuickCreateButtonInternal(image, x, y, BUTTON_TOGGLE, priority, DefaultMoveCallback, click);
}


GUIButtonRef QuickCreateButtonNoMove(BUTTON_PICS* const image, const INT16 x, const INT16 y, const INT16 priority, const GUI_CALLBACK click)
{
	return QuickCreateButtonInternal(image, x, y, BUTTON_TOGGLE, priority, MSYS_NO_CALLBACK, click);
}


GUIButtonRef QuickCreateButtonToggle(BUTTON_PICS* const image, const INT16 x, const INT16 y, const INT16 priority, const GUI_CALLBACK click)
{
	return QuickCreateButtonInternal(image, x, y, BUTTON_NEWTOGGLE, priority, MSYS_NO_CALLBACK, click);
}


GUIButtonRef QuickCreateButtonImg(const char* gfx, INT32 grayed, INT32 off_normal, INT32 off_hilite, INT32 on_normal, INT32 on_hilite, INT16 x, INT16 y, INT16 priority, GUI_CALLBACK click)
{
	BUTTON_PICS* const img = LoadButtonImage(gfx, grayed, off_normal, off_hilite, on_normal, on_hilite);
	GUIButtonRef const btn = QuickCreateButton(img, x, y, priority, click);
	btn->uiFlags |= BUTTON_SELFDELETE_IMAGE;
	return btn;
}


GUIButtonRef CreateIconAndTextButton(BUTTON_PICS* const Image, const wchar_t* const string, Font const font, const INT16 sForeColor, const INT16 sShadowColor, const INT16 sForeColorDown, const INT16 sShadowColorDown, const INT16 xloc, const INT16 yloc, const INT16 Priority, const GUI_CALLBACK ClickCallback)
{
	GUIButtonRef const b = QuickCreateButton(Image, xloc, yloc, Priority, ClickCallback);
	CopyButtonText(b, string);
	b->usFont           = font;
	b->sForeColor       = sForeColor;
	b->sShadowColor     = sShadowColor;
	b->sForeColorDown   = sForeColorDown;
	b->sShadowColorDown = sShadowColorDown;
	return b;
}


GUIButtonRef CreateLabel(const wchar_t* text, Font const font, INT16 forecolor, INT16 shadowcolor, INT16 x, INT16 y, INT16 w, INT16 h, INT16 priority)
{
	GUIButtonRef const btn = CreateTextButton(text, font, forecolor, shadowcolor, x, y, w, h, priority, NULL);
	btn->SpecifyDisabledStyle(GUI_BUTTON::DISABLED_STYLE_NONE);
	DisableButton(btn);
	return btn;
}


void GUI_BUTTON::SpecifyText(wchar_t const* const text)
{
	//free the previous strings memory if applicable
	if (string) MemFree(string);
	string = NULL;

	CopyButtonText(this, text);
	uiFlags |= BUTTON_DIRTY;
}


void SpecifyButtonText(GUIButtonRef const b, const wchar_t* string)
{
	CHECKV(b != NULL); // XXX HACK000C
	b->SpecifyText(string);
}


void GUI_BUTTON::SpecifyDownTextColors(INT16 const fore_colour_down, INT16 const shadow_colour_down)
{
	sForeColorDown    = fore_colour_down;
	sShadowColorDown  = shadow_colour_down;
	uiFlags          |= BUTTON_DIRTY;
}


void GUI_BUTTON::SpecifyHilitedTextColors(INT16 const fore_colour_highlighted, INT16 const shadow_colour_highlighted)
{
	sForeColorHilited    = fore_colour_highlighted;
	sShadowColorHilited  = shadow_colour_highlighted;
	uiFlags             |= BUTTON_DIRTY;
}


void GUI_BUTTON::SpecifyTextJustification(Justification const j)
{
	bJustification  = j;
	uiFlags        |= BUTTON_DIRTY;
}


void GUI_BUTTON::SpecifyGeneralTextAttributes(wchar_t const* const string, Font const font, INT16 const fore_colour, INT16 const shadow_colour)
{
	SpecifyText(string);
	usFont        = font;
	sForeColor    = fore_colour;
	sShadowColor  = shadow_colour;
	uiFlags      |= BUTTON_DIRTY;
}


void GUI_BUTTON::SpecifyTextOffsets(INT8 const text_x_offset, INT8 const text_y_offset, BOOLEAN const shift_text)
{
	bTextXOffset = text_x_offset;
	bTextYOffset = text_y_offset;
	fShiftText   = shift_text;
}


void GUI_BUTTON::SpecifyTextSubOffsets(INT8 const text_x_offset, INT8 const text_y_offset, BOOLEAN const shift_text)
{
	bTextXSubOffSet = text_x_offset;
	bTextYSubOffSet = text_y_offset;
	fShiftText      = shift_text;
}


void GUI_BUTTON::SpecifyTextWrappedWidth(INT16 const wrapped_width)
{
	sWrappedWidth = wrapped_width;
}


void GUI_BUTTON::SpecifyDisabledStyle(DisabledStyle const style)
{
	bDisabledStyle = style;
}


void SpecifyDisabledButtonStyle(GUIButtonRef const b, GUI_BUTTON::DisabledStyle const style)
{
	CHECKV(b != NULL); // XXX HACK000C
	b->SpecifyDisabledStyle(style);
}


void GUI_BUTTON::SpecifyIcon(SGPVObject const* const icon_, UINT16 const usVideoObjectIndex, INT8 const bXOffset, INT8 const bYOffset, BOOLEAN const)
{
	icon        = icon_;
	usIconIndex = usVideoObjectIndex;

	if (!icon_) return;

	bIconXOffset = bXOffset;
	bIconYOffset = bYOffset;
	fShiftImage  = TRUE;

	uiFlags |= BUTTON_DIRTY;
}


void GUI_BUTTON::AllowDisabledFastHelp()
{
	Area.uiFlags |= MSYS_ALLOW_DISABLED_FASTHELP;
}


void GUI_BUTTON::SetFastHelpText(const wchar_t* const text)
{
	Area.SetFastHelpText(text);
}


void SetButtonFastHelpText(GUIButtonRef const b, const wchar_t* Text)
{
	CHECKV(b != NULL); // XXX HACK000C
	b->SetFastHelpText(Text);
}


/* Dispatches all button callbacks for mouse movement. This function gets
 * called by the Mouse System. *DO NOT CALL DIRECTLY*
 */
static void QuickButtonCallbackMMove(MOUSE_REGION* reg, INT32 reason)
{
	Assert(reg != NULL);
	GUI_BUTTON* const b = reg->GetUserPtr<GUI_BUTTON>();

	if (b->uiFlags & BUTTON_ENABLED &&
			reason & (MSYS_CALLBACK_REASON_LOST_MOUSE | MSYS_CALLBACK_REASON_GAIN_MOUSE))
	{
		b->uiFlags |= BUTTON_DIRTY;
	}

	// Mouse moved on the button, so reset it's timer to maximum.
	if (b->Area.uiFlags & MSYS_CALLBACK_REASON_GAIN_MOUSE)
	{
		//check for sound playing stuff
		if (b->ubSoundSchemeID)
		{
			if (&b->Area == MSYS_PrevRegion && !gpAnchoredButton)
			{
				INT32 snd = (b->uiFlags & BUTTON_ENABLED ? BUTTON_SOUND_MOVED_ONTO : BUTTON_SOUND_DISABLED_MOVED_ONTO);
				PlayButtonSound(b, snd);
			}
		}
	}
	else
	{
		//Check if we should play a sound
		if (b->ubSoundSchemeID)
		{
			if (b->uiFlags & BUTTON_ENABLED)
			{
				if (&b->Area == MSYS_PrevRegion && !gpAnchoredButton)
				{
					PlayButtonSound(b, BUTTON_SOUND_MOVED_OFF_OF);
				}
			}
			else
			{
				PlayButtonSound(b, BUTTON_SOUND_DISABLED_MOVED_OFF_OF);
			}
		}
	}

	// ATE: New stuff for toggle buttons that work with new Win95 paradigm
	if (b->uiFlags & BUTTON_NEWTOGGLE &&
			reason & MSYS_CALLBACK_REASON_LOST_MOUSE &&
			b->ubToggleButtonActivated)
	{
		b->uiFlags ^= BUTTON_CLICKED_ON;
		b->ubToggleButtonActivated = FALSE;
	}

	/* If this button is enabled and there is a callback function associated with
	 * it, call the callback function.
	 */
	if (b->uiFlags & BUTTON_ENABLED && b->MoveCallback != NULL)
		b->MoveCallback(b, reason);
}


/* Dispatches all button callbacks for button presses. This function is called
 * by the Mouse System. *DO NOT CALL DIRECTLY*
 */
static void QuickButtonCallbackMButn(MOUSE_REGION* reg, INT32 reason)
{
	Assert(reg != NULL);
	GUI_BUTTON* const b = reg->GetUserPtr<GUI_BUTTON>();

	BOOLEAN MouseBtnDown = (reason & (MSYS_CALLBACK_REASON_LBUTTON_DWN | MSYS_CALLBACK_REASON_RBUTTON_DWN)) != 0;
	BOOLEAN StateBefore  = (b->uiFlags & BUTTON_CLICKED_ON) != 0;

	// ATE: New stuff for toggle buttons that work with new Win95 paridigm
	if (b->uiFlags & BUTTON_NEWTOGGLE && b->uiFlags & BUTTON_ENABLED)
	{
		if (reason & MSYS_CALLBACK_REASON_LBUTTON_DWN)
		{
			if (!b->ubToggleButtonActivated)
			{
				b->uiFlags ^= BUTTON_CLICKED_ON;
				b->ubToggleButtonActivated = TRUE;
			}
		}
		else if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
		{
			b->ubToggleButtonActivated = FALSE;
		}
	}

	BOOLEAN StateAfter = TRUE; // XXX HACK000E

	/* Kris:
	 * Set the anchored button incase the user moves mouse off region while still
	 * holding down the button, but only if the button is up.  In Win95, buttons
	 * that are already down, and anchored never change state, unless you release
	 * the mouse in the button area.
	 */
	if (b->uiFlags & BUTTON_ENABLED)
	{
		if (b->MoveCallback == DefaultMoveCallback)
		{
			if (reason & MSYS_CALLBACK_REASON_LBUTTON_DWN)
			{
				gpAnchoredButton = b;
				gfAnchoredState = StateBefore;
				b->uiFlags |= BUTTON_CLICKED_ON;
			}
			else if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
			{
				b->uiFlags &= ~BUTTON_CLICKED_ON;
			}
		}
		else if (b->uiFlags & BUTTON_CHECKBOX)
		{
			if (reason & MSYS_CALLBACK_REASON_LBUTTON_DWN)
			{
				/* The check box button gets anchored, though it doesn't actually use the
				 * anchoring move callback.  The effect is different, we don't want to
				 * toggle the button state, but we do want to anchor this button so that
				 * we don't effect any other buttons while we move the mouse around in
				 * anchor mode.
				 */
				gpAnchoredButton = b;
				gfAnchoredState = StateBefore;

				/* Trick the before state of the button to be different so the sound will
				 * play properly as checkbox buttons are processed differently.
				 */
				StateBefore = (b->uiFlags & BUTTON_CLICKED_ON) ? FALSE : TRUE;
				StateAfter = !StateBefore;
			}
			else if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
			{
				b->uiFlags ^= BUTTON_CLICKED_ON; //toggle the checkbox state upon release inside button area.
				/* Trick the before state of the button to be different so the sound will
				 * play properly as checkbox buttons are processed differently.
				 */
				StateBefore = (b->uiFlags & BUTTON_CLICKED_ON) ? FALSE : TRUE;
				StateAfter = !StateBefore;
			}
		}
	}
	else
	{
		// Should we play a sound if clicked on while disabled?
		if (b->ubSoundSchemeID && MouseBtnDown)
		{
			PlayButtonSound(b, BUTTON_SOUND_DISABLED_CLICK);
		}
	}

	// If this button is disabled, and no callbacks allowed when disabled callback
	if (!(b->uiFlags & BUTTON_ENABLED)) return;

	// If there is a callback function with this button, call it
	if (b->ClickCallback != NULL)
	{
		/* Kris:  January 6, 1998
		 * Added these checks to avoid a case where it was possible to process a
		 * leftbuttonup message when the button wasn't anchored, and should have
		 * been.
		 */
		gfDelayButtonDeletion = TRUE;
		if (!(reason & MSYS_CALLBACK_REASON_LBUTTON_UP) ||
				b->MoveCallback != DefaultMoveCallback ||
				gpPrevAnchoredButton == b)
		{
			b->ClickCallback(b, reason);
		}
		gfDelayButtonDeletion = FALSE;
	}
	else if (reason & MSYS_CALLBACK_REASON_LBUTTON_DWN)
	{
		// Otherwise, do default action with this button.
		b->uiFlags ^= BUTTON_CLICKED_ON;
	}

	if (b->uiFlags & BUTTON_CHECKBOX)
	{
		StateAfter = (b->uiFlags & BUTTON_CLICKED_ON) != 0;
	}

	// Play sounds for this enabled button (disabled sounds have already been done)
	if (b->ubSoundSchemeID && b->uiFlags & BUTTON_ENABLED)
	{
		if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
		{
			if (b->ubSoundSchemeID && StateBefore && !StateAfter)
			{
				PlayButtonSound(b, BUTTON_SOUND_CLICKED_OFF);
			}
		}
		else if (reason & MSYS_CALLBACK_REASON_LBUTTON_DWN)
		{
			if (b->ubSoundSchemeID && !StateBefore && StateAfter)
			{
				PlayButtonSound(b, BUTTON_SOUND_CLICKED_ON);
			}
		}
	}

#if defined JA2
	if (StateBefore != StateAfter)
	{
		InvalidateRegion(b->Area.RegionTopLeftX, b->Area.RegionTopLeftY, b->Area.RegionBottomRightX, b->Area.RegionBottomRightY);
	}
#endif

	if (gfPendingButtonDeletion) RemoveButtonsMarkedForDeletion();
}


static void DrawButtonFromPtr(GUI_BUTTON* b);


void RenderButtons(void)
{
	SaveFontSettings();
	for (INT32 iButtonID = 0; iButtonID < MAX_BUTTONS; ++iButtonID)
	{
		// If the button exists, and it's not owned by another object, draw it
		// Kris:  and make sure that the button isn't hidden.
		GUI_BUTTON* b = ButtonList[iButtonID];
		if (b == NULL || !(b->Area.uiFlags & MSYS_REGION_ENABLED)) continue;

		if ((b->uiFlags ^ b->uiOldFlags) & (BUTTON_CLICKED_ON | BUTTON_ENABLED))
		{
			// Something is different, set dirty!
			b->uiFlags |= BUTTON_DIRTY;
		}

		// Set old flags
		b->uiOldFlags = b->uiFlags;

		if (b->uiFlags & BUTTON_FORCE_UNDIRTY)
		{
			b->uiFlags &= ~BUTTON_DIRTY;
			b->uiFlags &= ~BUTTON_FORCE_UNDIRTY;
		}

		// Check if we need to update!
		if (b->uiFlags & BUTTON_DIRTY)
		{
			// Turn off dirty flag
			b->uiFlags &= ~BUTTON_DIRTY;
			DrawButtonFromPtr(b);

#if defined JA2
			InvalidateRegion(b->Area.RegionTopLeftX, b->Area.RegionTopLeftY, b->Area.RegionBottomRightX, b->Area.RegionBottomRightY);
#endif
		}
	}

	RestoreFontSettings();
}


void MarkAButtonDirty(GUIButtonRef const b)
{
  // surgical dirtying -> marks a user specified button dirty, without dirty the whole lot of them
	CHECKV(b != NULL); // XXX HACK000C
	b->uiFlags |= BUTTON_DIRTY;
}


void MarkButtonsDirty(void)
{
	for (INT32 x = 0; x < MAX_BUTTONS; ++x)
	{
		if (ButtonList[x]) ButtonList[x]->uiFlags |= BUTTON_DIRTY;
	}
}


void UnMarkButtonDirty(GUIButtonRef const b)
{
	CHECKV(b != NULL); // XXX HACK000C
	b->uiFlags &= ~BUTTON_DIRTY;
}


void UnmarkButtonsDirty(void)
{
	for (INT32 x = 0; x < MAX_BUTTONS; ++x)
	{
		GUI_BUTTON* const b = ButtonList[x];
		if (b) UnMarkButtonDirty(b);
	}
}


void ForceButtonUnDirty(GUIButtonRef const b)
{
	CHECKV(b != NULL); // XXX HACK000C
	b->uiFlags &= ~BUTTON_DIRTY;
	b->uiFlags |= BUTTON_FORCE_UNDIRTY;
}


void GUI_BUTTON::Draw()
{
	if (string) SaveFontSettings();
	if (Area.uiFlags & MSYS_REGION_ENABLED) DrawButtonFromPtr(this);
	if (string) RestoreFontSettings();
}


void DrawButton(GUIButtonRef const b)
{
	CHECKV(b != NULL); // XXX HACK000C
	b->Draw();
}


static void DrawCheckBoxButton(const GUI_BUTTON* b);
static void DrawGenericButton( const GUI_BUTTON* b);
static void DrawHatchOnButton( const GUI_BUTTON* b);
static void DrawIconOnButton(  const GUI_BUTTON* b);
static void DrawQuickButton(   const GUI_BUTTON* b);
static void DrawShadeOnButton( const GUI_BUTTON* b);
static void DrawTextOnButton(  const GUI_BUTTON* b);


// Given a pointer to a GUI_BUTTON structure, draws the button on the screen.
static void DrawButtonFromPtr(GUI_BUTTON* b)
{
	// Draw the appropriate button according to button type
	gbDisabledButtonStyle = GUI_BUTTON::DISABLED_STYLE_NONE;
	switch (b->uiFlags & BUTTON_TYPES)
	{
		case BUTTON_QUICK:    DrawQuickButton(b);    break;
		case BUTTON_GENERIC:  DrawGenericButton(b);  break;
		case BUTTON_CHECKBOX: DrawCheckBoxButton(b); break;

		case BUTTON_HOT_SPOT:
			return; // hotspots don't have text, but if you want to, change this to a break!
	}
	if (b->icon)   DrawIconOnButton(b);
	if (b->string) DrawTextOnButton(b);
	/* If the button is disabled, and a style has been calculated, then draw the
	 * style last.
	 */
	switch (gbDisabledButtonStyle)
	{
		case GUI_BUTTON::DISABLED_STYLE_HATCHED: DrawHatchOnButton(b); break;
		case GUI_BUTTON::DISABLED_STYLE_SHADED:  DrawShadeOnButton(b); break;
	}
}


// Draws a QuickButton type button on the screen.
static void DrawQuickButton(const GUI_BUTTON* b)
{
	const BUTTON_PICS* const pics = b->image;

	INT32 UseImage = 0;
	if (b->uiFlags & BUTTON_ENABLED)
	{
		if (b->uiFlags & BUTTON_CLICKED_ON)
		{
			// Is the mouse over this area, and we have a hilite image?
			if (b->Area.uiFlags & MSYS_MOUSE_IN_AREA &&
					gfRenderHilights &&
					pics->OnHilite != -1)
			{
				UseImage = pics->OnHilite;
			}
			else if (pics->OnNormal != -1)
			{
				UseImage = pics->OnNormal;
			}
		}
		else
		{
			// Is the mouse over the button, and do we have hilite image?
			if (b->Area.uiFlags & MSYS_MOUSE_IN_AREA &&
					gfRenderHilights &&
					pics->OffHilite != -1)
			{
				UseImage = pics->OffHilite;
			}
			else if (pics->OffNormal != -1)
			{
				UseImage = pics->OffNormal;
			}
		}
	}
	else if (pics->Grayed != -1)
	{
		// Button is diabled so use the "Grayed-out" image
		UseImage = pics->Grayed;
	}
	else
	{
		UseImage = pics->OffNormal;
		switch (b->bDisabledStyle)
		{
			case GUI_BUTTON::DISABLED_STYLE_DEFAULT:
				gbDisabledButtonStyle = b->string ?
					GUI_BUTTON::DISABLED_STYLE_SHADED :
					GUI_BUTTON::DISABLED_STYLE_HATCHED;
				break;

			case GUI_BUTTON::DISABLED_STYLE_HATCHED:
			case GUI_BUTTON::DISABLED_STYLE_SHADED:
				gbDisabledButtonStyle = b->bDisabledStyle;
				break;
		}
	}

	BltVideoObject(ButtonDestBuffer, pics->vobj, UseImage, b->XLoc, b->YLoc);
}


static void DrawHatchOnButton(const GUI_BUTTON* b)
{
	SGPRect ClipRect;
	ClipRect.iLeft   = b->Area.RegionTopLeftX;
	ClipRect.iRight  = b->Area.RegionBottomRightX - 1;
	ClipRect.iTop    = b->Area.RegionTopLeftY;
	ClipRect.iBottom = b->Area.RegionBottomRightY - 1;
	SGPVSurface::Lock l(ButtonDestBuffer);
	Blt16BPPBufferHatchRect(l.Buffer<UINT16>(), l.Pitch(), &ClipRect);
}


static void DrawShadeOnButton(const GUI_BUTTON* b)
{
	const MOUSE_REGION* r = &b->Area;
	ShadowVideoSurfaceRect(ButtonDestBuffer, r->RegionTopLeftX, r->RegionTopLeftY, r->RegionBottomRightX, r->RegionBottomRightY);
}


void GUI_BUTTON::DrawCheckBoxOnOff(BOOLEAN const on)
{
	BOOLEAN const fLeftButtonState = gfLeftButtonState;

	gfLeftButtonState = on;
	Area.uiFlags |= MSYS_MOUSE_IN_AREA;
	Draw();

	gfLeftButtonState = fLeftButtonState;
}


static void DrawCheckBoxButton(const GUI_BUTTON *b)
{
	const BUTTON_PICS* const pics = b->image;

	INT32 UseImage = 0;
	if (b->uiFlags & BUTTON_ENABLED)
	{
		if (b->uiFlags & BUTTON_CLICKED_ON)
		{
			// Is the mouse over this area, and we have a hilite image?
			if (b->Area.uiFlags & MSYS_MOUSE_IN_AREA &&
					gfRenderHilights &&
					gfLeftButtonState &&
					pics->OnHilite != -1)
			{
				UseImage = pics->OnHilite;
			}
			else if (pics->OnNormal != -1)
			{
				UseImage = pics->OnNormal;
			}
		}
		else
		{
			// Is the mouse over the button, and do we have hilite image?
			if (b->Area.uiFlags & MSYS_MOUSE_IN_AREA &&
					gfRenderHilights &&
					gfLeftButtonState &&
					pics->OffHilite != -1)
			{
				UseImage = pics->OffHilite;
			}
			else if (pics->OffNormal != -1)
			{
				UseImage = pics->OffNormal;
			}
		}
	}
	else if (pics->Grayed != -1)
	{
		// Button is disabled so use the "Grayed-out" image
		UseImage = pics->Grayed;
	}
	else //use the disabled style
	{
		if (b->uiFlags & BUTTON_CLICKED_ON)
		{
			UseImage = pics->OnHilite;
		}
		else
		{
			UseImage = pics->OffHilite;
		}
		switch (b->bDisabledStyle)
		{
			case GUI_BUTTON::DISABLED_STYLE_DEFAULT:
				gbDisabledButtonStyle = GUI_BUTTON::DISABLED_STYLE_HATCHED;
				break;

			case GUI_BUTTON::DISABLED_STYLE_HATCHED:
			case GUI_BUTTON::DISABLED_STYLE_SHADED:
				gbDisabledButtonStyle = b->bDisabledStyle;
				break;
		}
	}

	BltVideoObject(ButtonDestBuffer, pics->vobj, UseImage, b->XLoc, b->YLoc);
}


static void DrawIconOnButton(const GUI_BUTTON* b)
{
	if (!b->icon) return;

	// Get width and height of button area
	INT32 width  = b->Area.RegionBottomRightX - b->Area.RegionTopLeftX;
	INT32 height = b->Area.RegionBottomRightY - b->Area.RegionTopLeftY;

	// Compute viewable area (inside borders)
	SGPRect NewClip;
	NewClip.iLeft   = b->XLoc + 3;
	NewClip.iRight  = b->XLoc + width - 3;
	NewClip.iTop    = b->YLoc + 2;
	NewClip.iBottom = b->YLoc + height - 2;

	// Get Icon's blit start coordinates
	INT32 IconX = NewClip.iLeft;
	INT32 IconY = NewClip.iTop;

	// Get current clip area
	SGPRect OldClip;
	GetClippingRect(&OldClip);

	// Clip button's viewable area coords to screen
	if (NewClip.iLeft < OldClip.iLeft) NewClip.iLeft = OldClip.iLeft;

	// Is button right off the right side of the screen?
	if (NewClip.iLeft > OldClip.iRight) return;

	if (NewClip.iRight > OldClip.iRight) NewClip.iRight = OldClip.iRight;

	// Is button completely off the left side of the screen?
	if (NewClip.iRight < OldClip.iLeft) return;

	if (NewClip.iTop < OldClip.iTop) NewClip.iTop = OldClip.iTop;

	// Are we right off the bottom of the screen?
	if (NewClip.iTop > OldClip.iBottom) return;

	if (NewClip.iBottom > OldClip.iBottom) NewClip.iBottom = OldClip.iBottom;

	// Are we off the top?
	if (NewClip.iBottom < OldClip.iTop) return;

	// Did we clip the viewable area out of existance?
	if (NewClip.iRight <= NewClip.iLeft || NewClip.iBottom <= NewClip.iTop) return;

	// Get the width and height of the icon itself
	const SGPVObject*  const hvObject = b->icon;
	ETRLEObject const* const pTrav    = hvObject->SubregionProperties(b->usIconIndex);

	/* Compute coordinates for centering the icon on the button or use the offset
	 * system.
	 */
	INT32 xp;
	if (b->bIconXOffset == -1)
	{
		const INT32 IconW = pTrav->usWidth  + pTrav->sOffsetX;
		xp = IconX + (width - 6 - IconW) / 2;
	}
	else
	{
		xp = b->Area.RegionTopLeftX + b->bIconXOffset;
	}

	INT32 yp;
	if (b->bIconYOffset == -1)
	{
		const INT32 IconH = pTrav->usHeight + pTrav->sOffsetY;
		yp = IconY + (height - 4 - IconH) / 2;
	}
	else
	{
		yp = b->Area.RegionTopLeftY + b->bIconYOffset;
	}

	/* Was the button clicked on? if so, move the image slightly for the illusion
	 * that the image moved into the screen.
	 */
	if (b->uiFlags & BUTTON_CLICKED_ON && b->fShiftImage)
	{
		xp++;
		yp++;
	}

	// Set the clipping rectangle to the viewable area of the button
	SetClippingRect(&NewClip);

	BltVideoObject(ButtonDestBuffer, hvObject, b->usIconIndex, xp, yp);

	// Restore previous clip region
	SetClippingRect(&OldClip);
}


// If a button has text attached to it, then it'll draw it last.
static void DrawTextOnButton(const GUI_BUTTON* b)
{
	// If this button actually has a string to print
	if (b->string == NULL) return;

	// Get the width and height of this button
	const INT32 width  = b->Area.RegionBottomRightX - b->Area.RegionTopLeftX;
	const INT32 height = b->Area.RegionBottomRightY - b->Area.RegionTopLeftY;

	// Compute the viewable area on this button
	SGPRect NewClip;
	NewClip.iLeft   = b->XLoc + 3;
	NewClip.iRight  = b->XLoc + width - 3;
	NewClip.iTop    = b->YLoc + 2;
	NewClip.iBottom = b->YLoc + height - 2;

	// Get the starting coordinates to print
	const INT32 TextX = NewClip.iLeft;
	const INT32 TextY = NewClip.iTop;

	// Get the current clipping area
	SGPRect OldClip;
	GetClippingRect(&OldClip);

	// Clip the button's viewable area to the screen
	if (NewClip.iLeft < OldClip.iLeft) NewClip.iLeft = OldClip.iLeft;

	// Are we off hte right side?
	if (NewClip.iLeft > OldClip.iRight) return;

	if (NewClip.iRight > OldClip.iRight) NewClip.iRight = OldClip.iRight;

	// Are we off the left side?
	if (NewClip.iRight < OldClip.iLeft) return;

	if (NewClip.iTop < OldClip.iTop) NewClip.iTop = OldClip.iTop;

	// Are we off the bottom of the screen?
	if (NewClip.iTop > OldClip.iBottom) return;

	if (NewClip.iBottom > OldClip.iBottom) NewClip.iBottom = OldClip.iBottom;

	// Are we off the top?
	if (NewClip.iBottom < OldClip.iTop) return;

	// Did we clip the viewable area out of existance?
	if (NewClip.iRight <= NewClip.iLeft || NewClip.iBottom <= NewClip.iTop) return;

	// Set the font printing settings to the buttons viewable area
	SetFontDestBuffer(ButtonDestBuffer, NewClip.iLeft, NewClip.iTop, NewClip.iRight, NewClip.iBottom);

	// Compute the coordinates to center the text
	INT32 yp;
	if (b->bTextYOffset == -1)
	{
		yp = TextY + (height - GetFontHeight(b->usFont)) / 2 - 1;
	}
	else
	{
		yp = b->Area.RegionTopLeftY + b->bTextYOffset;
	}

	INT32 xp;
	if (b->bTextXOffset == -1)
	{
		switch (b->bJustification)
		{
			case GUI_BUTTON::TEXT_LEFT:   xp = TextX + 3; break;
			case GUI_BUTTON::TEXT_RIGHT:  xp = NewClip.iRight - StringPixLength(b->string, b->usFont) - 3; break;
			default:
			case GUI_BUTTON::TEXT_CENTER: xp = TextX + (width - 6 - StringPixLength(b->string, b->usFont)) / 2; break;
		}
	}
	else
	{
		xp = b->Area.RegionTopLeftX + b->bTextXOffset;
	}

	// Set the printing font to the button text font
	SetFont(b->usFont);

	// print the text
	SetFontBackground(FONT_MCOLOR_BLACK);

	//Override the colors if necessary.
	INT16 sForeColor;
	if (b->uiFlags & BUTTON_ENABLED && b->Area.uiFlags & MSYS_MOUSE_IN_AREA && b->sForeColorHilited != -1)
	{
		sForeColor = b->sForeColorHilited;
	}
	else if (b->uiFlags & BUTTON_CLICKED_ON && b->sForeColorDown != -1)
	{
		sForeColor = b->sForeColorDown;
	}
	else
	{
		sForeColor = b->sForeColor;
	}
	SetFontForeground(sForeColor);

	if (b->uiFlags & BUTTON_ENABLED && b->Area.uiFlags & MSYS_MOUSE_IN_AREA && b->sShadowColorHilited != -1)
	{
		SetFontShadow(b->sShadowColorHilited);
	}
	else if (b->uiFlags & BUTTON_CLICKED_ON && b->sShadowColorDown != -1)
	{
		SetFontShadow(b->sShadowColorDown);
	}
	else if (b->sShadowColor != -1)
	{
		SetFontShadow(b->sShadowColor);
	}

	if (b->uiFlags & BUTTON_CLICKED_ON && b->fShiftText)
	{
		/* Was the button clicked on? if so, move the text slightly for the illusion
		 * that the text moved into the screen. */
		xp++;
		yp++;
	}

#if defined JA2
	if (b->sWrappedWidth != -1)
	{
		UINT8 bJustified = 0;
		switch (b->bJustification)
		{
			case GUI_BUTTON::TEXT_LEFT:    bJustified = LEFT_JUSTIFIED;    break;
			case GUI_BUTTON::TEXT_RIGHT:   bJustified = RIGHT_JUSTIFIED;   break;
			case GUI_BUTTON::TEXT_CENTER:  bJustified = CENTER_JUSTIFIED;  break;
			default:                       Assert(0);                      break;
		}
		if (b->bTextXOffset == -1)
		{
			/* Kris:
			 * There needs to be recalculation of the start positions based on the
			 * justification and the width specified wrapped width.  I was drawing a
			 * double lined word on the right side of the button to find it drawing
			 * way over to the left.  I've added the necessary code for the right and
			 * center justification.
			 */
			yp = b->Area.RegionTopLeftY + 2;

			switch (b->bJustification)
			{
				case GUI_BUTTON::TEXT_RIGHT:
					xp = b->Area.RegionBottomRightX - 3 - b->sWrappedWidth;
					if (b->fShiftText && b->uiFlags & BUTTON_CLICKED_ON)
					{
						xp++;
						yp++;
					}
					break;

				case GUI_BUTTON::TEXT_CENTER:
					xp = b->Area.RegionTopLeftX + 3 + b->sWrappedWidth / 2;
					if (b->fShiftText && b->uiFlags & BUTTON_CLICKED_ON)
					{
						xp++;
						yp++;
					}
					break;
			}
		}
		yp += b->bTextYSubOffSet;
		xp += b->bTextXSubOffSet;
		DisplayWrappedString(xp, yp, b->sWrappedWidth, 1, b->usFont, sForeColor, b->string, FONT_MCOLOR_BLACK, bJustified);
	}
	else
	{
		yp += b->bTextYSubOffSet;
		xp += b->bTextXSubOffSet;
		MPrint(xp, yp, b->string);
	}
#else
	MPrint(xp, yp, b->string);
#endif
	// Restore the old text printing settings
}


/* This function is called by the DrawIconicButton and DrawTextButton routines
 * to draw the borders and background of the buttons.
 */
static void DrawGenericButton(const GUI_BUTTON* b)
{
	// Select the graphics to use depending on the current state of the button
	HVOBJECT BPic;
	if (!(b->uiFlags & BUTTON_ENABLED))
	{
		BPic = GenericButtonOffNormal;
		switch (b->bDisabledStyle)
		{
			case GUI_BUTTON::DISABLED_STYLE_DEFAULT:
				gbDisabledButtonStyle = b->string ?
					GUI_BUTTON::DISABLED_STYLE_SHADED :
					GUI_BUTTON::DISABLED_STYLE_HATCHED;
				break;

			case GUI_BUTTON::DISABLED_STYLE_HATCHED:
			case GUI_BUTTON::DISABLED_STYLE_SHADED:
				gbDisabledButtonStyle = b->bDisabledStyle;
				break;
		}
	}
	else if (b->uiFlags & BUTTON_CLICKED_ON)
	{
		if  (b->Area.uiFlags & MSYS_MOUSE_IN_AREA && GenericButtonOnHilite != NULL && gfRenderHilights)
		{
			BPic = GenericButtonOnHilite;
		}
		else
		{
			BPic = GenericButtonOnNormal;
		}
	}
	else
	{
		if (b->Area.uiFlags & MSYS_MOUSE_IN_AREA && GenericButtonOffHilite != NULL && gfRenderHilights)
		{
			BPic = GenericButtonOffHilite;
		}
		else
		{
			BPic = GenericButtonOffNormal;
		}
	}

#if defined JA2
	const INT32 iBorderWidth  = 3;
	const INT32 iBorderHeight = 2;
#else
	/* DB - Added this to support more flexible sizing of border images.  The 3x2
	 * size was a bit limiting. JA2 should default to the original size, unchanged
	 */
	ETRLEObject const* const pTrav = &BPic->SubregionProperties(0);
	const INT32 iBorderHeight = pTrav->usHeight;
	const INT32 iBorderWidth  = pTrav->usWidth;
#endif

	// Compute the number of button "chunks" needed to be blitted
	const INT32 width         = b->Area.RegionBottomRightX - b->Area.RegionTopLeftX;
	const INT32 height        = b->Area.RegionBottomRightY - b->Area.RegionTopLeftY;
	const INT32 NumChunksWide = width  / iBorderWidth;
	INT32       NumChunksHigh = height / iBorderHeight;
	const INT32 hremain       = height % iBorderHeight;
	const INT32 wremain       = width  % iBorderWidth;

	INT32 cx = b->XLoc + (NumChunksWide - 1) * iBorderWidth  + wremain;
	INT32 cy = b->YLoc + (NumChunksHigh - 1) * iBorderHeight + hremain;

	// Fill the button's area with the button's background color
	ColorFillVideoSurfaceArea(ButtonDestBuffer, b->Area.RegionTopLeftX, b->Area.RegionTopLeftY, b->Area.RegionBottomRightX, b->Area.RegionBottomRightY, GenericButtonFillColors);

	SGPVSurface::Lock l(ButtonDestBuffer);
	UINT16* const pDestBuf         = l.Buffer<UINT16>();
	UINT32  const uiDestPitchBYTES = l.Pitch();

	SGPRect ClipRect;
	GetClippingRect(&ClipRect);

	// Draw the button's borders and corners (horizontally)
	for (INT32 q = 0; q < NumChunksWide; q++)
	{
		const INT32 ImgNum = (q == 0 ? 0 : 1);
		const INT32 x = b->XLoc + q * iBorderWidth;
		Blt8BPPDataTo16BPPBufferTransparentClip(pDestBuf, uiDestPitchBYTES, BPic, x,  b->YLoc, ImgNum,     &ClipRect);
		Blt8BPPDataTo16BPPBufferTransparentClip(pDestBuf, uiDestPitchBYTES, BPic, x,  cy,      ImgNum + 5, &ClipRect);
	}
	// Blit the right side corners
	Blt8BPPDataTo16BPPBufferTransparentClip(pDestBuf, uiDestPitchBYTES, BPic, cx, b->YLoc, 2, &ClipRect);
	Blt8BPPDataTo16BPPBufferTransparentClip(pDestBuf, uiDestPitchBYTES, BPic, cx, cy,      7, &ClipRect);
	// Draw the vertical members of the button's borders
	NumChunksHigh--;

	if (hremain != 0)
	{
		const INT32 y = b->YLoc + NumChunksHigh * iBorderHeight - iBorderHeight + hremain;
		Blt8BPPDataTo16BPPBufferTransparentClip(pDestBuf, uiDestPitchBYTES, BPic, b->XLoc, y, 3, &ClipRect);
		Blt8BPPDataTo16BPPBufferTransparentClip(pDestBuf, uiDestPitchBYTES, BPic, cx,      y, 4, &ClipRect);
	}

	for (INT32 q = 1; q < NumChunksHigh; q++)
	{
		const INT32 y = b->YLoc + q * iBorderHeight;
		Blt8BPPDataTo16BPPBufferTransparentClip(pDestBuf, uiDestPitchBYTES, BPic, b->XLoc, y, 3, &ClipRect);
		Blt8BPPDataTo16BPPBufferTransparentClip(pDestBuf, uiDestPitchBYTES, BPic, cx,      y, 4, &ClipRect);
	}
}


GUIButtonRef CreateCheckBoxButton(INT16 x, INT16 y, const char* filename, INT16 Priority, GUI_CALLBACK ClickCallback)
{
	Assert(filename != NULL);
	BUTTON_PICS* const ButPic = LoadButtonImage(filename, -1, 0, 1, 2, 3);
	GUIButtonRef const b      = QuickCreateButtonInternal(ButPic, x, y, BUTTON_CHECKBOX, Priority, MSYS_NO_CALLBACK, ClickCallback);

	//change the flags so that it isn't a quick button anymore
	b->uiFlags &= ~BUTTON_QUICK;
	b->uiFlags |= BUTTON_CHECKBOX | BUTTON_SELFDELETE_IMAGE;

	return b;
}


void MSYS_SetBtnUserData(GUIButtonRef const b, INT32 userdata)
{
	CHECKV(b != NULL); // XXX HACK000C
	b->User.Data = userdata;
}


INT32 MSYS_GetBtnUserData(const GUI_BUTTON* b)
{
	return b->User.Data;
}


/* Generic Button Movement Callback to reset the mouse button if the mouse is no
 * longer in the button region.
 */
static void DefaultMoveCallback(GUI_BUTTON* btn, INT32 reason)
{
	// If the button isn't the anchored button, then we don't want to modify the button state.
	if (btn != gpAnchoredButton) return;

	if (reason & MSYS_CALLBACK_REASON_LOST_MOUSE)
	{
		if (!gfAnchoredState)
		{
			btn->uiFlags &= ~BUTTON_CLICKED_ON;
			if (btn->ubSoundSchemeID)
			{
				PlayButtonSound(btn, BUTTON_SOUND_CLICKED_OFF);
			}
		}
#if defined JA2
		InvalidateRegion(btn->Area.RegionTopLeftX, btn->Area.RegionTopLeftY, btn->Area.RegionBottomRightX, btn->Area.RegionBottomRightY);
#endif
	}
	else if (reason & MSYS_CALLBACK_REASON_GAIN_MOUSE)
	{
		btn->uiFlags |= BUTTON_CLICKED_ON;
		if (btn->ubSoundSchemeID)
		{
			PlayButtonSound(btn, BUTTON_SOUND_CLICKED_ON);
		}
#if defined JA2
		InvalidateRegion(btn->Area.RegionTopLeftX, btn->Area.RegionTopLeftY, btn->Area.RegionBottomRightX, btn->Area.RegionBottomRightY);
#endif
	}
}


void ReleaseAnchorMode(void)
{
  if (!gpAnchoredButton) return;

	if (gusMouseXPos < gpAnchoredButton->Area.RegionTopLeftX     ||
			gusMouseXPos > gpAnchoredButton->Area.RegionBottomRightX ||
			gusMouseYPos < gpAnchoredButton->Area.RegionTopLeftY     ||
			gusMouseYPos > gpAnchoredButton->Area.RegionBottomRightY)
	{
		//released outside button area, so restore previous button state.
		if (gfAnchoredState)
		{
			gpAnchoredButton->uiFlags |= BUTTON_CLICKED_ON;
		}
		else
		{
			gpAnchoredButton->uiFlags &= ~BUTTON_CLICKED_ON;
		}
#if defined JA2
		InvalidateRegion(gpAnchoredButton->Area.RegionTopLeftX, gpAnchoredButton->Area.RegionTopLeftY, gpAnchoredButton->Area.RegionBottomRightX, gpAnchoredButton->Area.RegionBottomRightY);
#endif
	}
	gpPrevAnchoredButton = gpAnchoredButton;
	gpAnchoredButton = NULL;
}


void GUI_BUTTON::Hide()
{
	Area.uiFlags &= ~MSYS_REGION_ENABLED;
	uiFlags      |= BUTTON_DIRTY;
#if defined JA2
	InvalidateRegion(Area.RegionTopLeftX, Area.RegionTopLeftY, Area.RegionBottomRightX, Area.RegionBottomRightY);
#endif
}


void HideButton(GUIButtonRef const b)
{
	CHECKV(b != NULL); // XXX HACK000C
	b->Hide();
}


void GUI_BUTTON::Show()
{
	Area.uiFlags |= MSYS_REGION_ENABLED;
	uiFlags      |= BUTTON_DIRTY;
#if defined JA2
	InvalidateRegion(Area.RegionTopLeftX, Area.RegionTopLeftY, Area.RegionBottomRightX, Area.RegionBottomRightY);
#endif
}


void ShowButton(GUIButtonRef const b)
{
	CHECKV(b != NULL); // XXX HACK000C
	b->Show();
}


UINT16 GetGenericButtonFillColor(void)
{
	return GenericButtonFillColors;
}
