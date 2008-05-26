// by Kris Morness (originally created by Bret Rowden)

#ifndef BUTTON_SYSTEM_H
#define BUTTON_SYSTEM_H

#include "MouseSystem.h"


#define MAX_BUTTONS     400
#define MAX_BUTTON_PICS 256


// Some GUI_BUTTON system defines
#define BUTTON_NO_IMAGE    -1

//effects how the button is rendered.
#define BUTTON_TYPES (BUTTON_QUICK | BUTTON_GENERIC | BUTTON_HOT_SPOT | BUTTON_CHECKBOX)

//button flags
#define BUTTON_TOGGLE           0x00000000
#define BUTTON_QUICK            0x00000000
#define BUTTON_ENABLED          0x00000001
#define BUTTON_CLICKED_ON       0x00000002
#define BUTTON_GENERIC          0x00000020
#define BUTTON_HOT_SPOT         0x00000040
#define BUTTON_SELFDELETE_IMAGE 0x00000080
#define BUTTON_DELETION_PENDING 0x00000100
#define BUTTON_DIRTY            0x00000400
#define BUTTON_CHECKBOX         0x00001000
#define BUTTON_NEWTOGGLE        0x00002000
#define BUTTON_FORCE_UNDIRTY    0x00004000 // no matter what happens this buttons does NOT get marked dirty
#define BUTTON_NO_DUPLICATE     0x80000000 // Exclude button from duplicate check

#define BUTTON_SOUND_NONE                  0x00
#define BUTTON_SOUND_CLICKED_ON            0x01
#define BUTTON_SOUND_CLICKED_OFF           0x02
#define BUTTON_SOUND_MOVED_ONTO            0x04
#define BUTTON_SOUND_MOVED_OFF_OF          0x08
#define BUTTON_SOUND_DISABLED_CLICK        0x10
#define BUTTON_SOUND_DISABLED_MOVED_ONTO   0x20
#define BUTTON_SOUND_DISABLED_MOVED_OFF_OF 0x40


extern SGPVSurface* ButtonDestBuffer;

typedef struct GUI_BUTTON GUI_BUTTON;

// GUI_BUTTON callback function type
typedef void (*GUI_CALLBACK)(struct GUI_BUTTON*, INT32);

// GUI_BUTTON structure definitions.
struct GUI_BUTTON
{
	// Set the text that will be displayed as the FastHelp
	void SetFastHelpText(wchar_t const* text);

	void Hide();
	void Show();

	void SpecifyDownTextColors(INT16 fore_colour_down, INT16 shadow_colour_down);
	void SpecifyHilitedTextColors(INT16 fore_colour_highlighted, INT16 shadow_colour_highlighted);

	enum Justification
	{
		TEXT_LEFT   = -1,
		TEXT_CENTER =  0,
		TEXT_RIGHT  =  1
	};
	void SpecifyTextJustification(Justification);

	void SpecifyText(wchar_t const* text);
	void SpecifyGeneralTextAttributes(wchar_t const* string, Font, INT16 fore_colour, INT16 shadow_colour);
	void SpecifyTextOffsets(INT8 text_x_offset, INT8 text_y_offset, BOOLEAN shift_text);
	void SpecifyTextSubOffsets(INT8 text_x_offset, INT8 text_y_offset, BOOLEAN shift_text);
	void SpecifyTextWrappedWidth(INT16 wrapped_width);

	void AllowDisabledFastHelp();

	/* Note:  Text is always on top
	 * If fShiftImage is true, then the image will shift down one pixel and right
	 * one pixel just like the text does.
	 */
	void SpecifyIcon(SGPVObject const* icon, UINT16 usVideoObjectIndex, INT8 bXOffset, INT8 bYOffset, BOOLEAN fShiftImage);

	// will simply set the cursor for the mouse region the button occupies
	void SetCursor(UINT16 cursor);

	INT32        IDNum;         // ID Number, contains it's own button number
	BUTTON_PICS* image;         // Image to use (see DOCs for details)
	MOUSE_REGION Area;          // Mouse System's mouse region to use for this button
	GUI_CALLBACK ClickCallback; // Button Callback when button is clicked
	GUI_CALLBACK MoveCallback;  // Button Callback when mouse moved on this region
	UINT32       uiFlags;       // Button state flags etc.( 32-bit )
	UINT32       uiOldFlags;    // Old flags from previous render loop
	INT16        XLoc;          // Coordinates where button is on the screen
	INT16        YLoc;
	union                       // Place holder for user data etc.
	{
		INT32 Data;
		void* Ptr;
	} User;
	INT8         bDisabledStyle; // Button disabled style

	// For buttons with text
	wchar_t*     string;              // the string
	Font         usFont;              // font for text
	INT16        sForeColor;          // text colors if there is text
	INT16        sShadowColor;
	INT16        sForeColorDown;      // text colors when button is down (optional)
	INT16        sShadowColorDown;
	INT16        sForeColorHilited;   // text colors when button is down (optional)
	INT16        sShadowColorHilited;
	INT8         bJustification;      // BUTTON_TEXT_LEFT, BUTTON_TEXT_CENTER, BUTTON_TEXT_RIGHT
	INT8         bTextXOffset;
	INT8         bTextYOffset;
	INT8         bTextXSubOffSet;
	INT8         bTextYSubOffSet;
	BOOLEAN      fShiftText;
	INT16        sWrappedWidth;

	// For buttons with icons (don't confuse this with quickbuttons which have up to 5 states)
	const SGPVObject* icon;
	INT16        usIconIndex;
	INT8         bIconXOffset; // -1 means horizontally centered
	INT8         bIconYOffset; // -1 means vertically centered
	BOOLEAN      fShiftImage;  // if true, icon is shifted +1,+1 when button state is down.

	UINT8        ubToggleButtonActivated;

	UINT8        ubSoundSchemeID;
};


extern GUI_BUTTON* ButtonList[MAX_BUTTONS]; // Button System's Main Button List


class GUIButtonRef
{
	public:
		GUIButtonRef() : btn_id_(0) {}

		GUIButtonRef(GUI_BUTTON* const b) : btn_id_(b->IDNum) {}

		void Reset() { btn_id_ = 0; }

		INT32 ID() const { return btn_id_; }

		GUI_BUTTON* operator ->() const { return ButtonList[btn_id_]; }

		operator GUI_BUTTON*() const { return ButtonList[btn_id_]; }

	private:
		INT32 btn_id_;
};


/* Initializes the GUI button system for use. Must be called before using any
 * other button functions.
 */
BOOLEAN InitButtonSystem(void);

/* Shuts down and cleans up the GUI button system. Must be called before exiting
 * the program.  Button functions should not be used after calling this
 * function.
 */
void ShutdownButtonSystem(void);

// Set the text that will be displayed as the FastHelp
void SetButtonFastHelpText(GUIButtonRef, wchar_t const* text);

#if defined _JA2_RENDER_DIRTY

void RenderButtonsFastHelp(void);
#	define RenderButtonsFastHelp() RenderFastHelp()

#endif

// Loads an image file for use as a button icon.
INT16 LoadGenericButtonIcon(const char* filename);

// Removes a button icon graphic from the system
void UnloadGenericButtonIcon(INT16 GenImg);

// Load images for use with QuickButtons.
BUTTON_PICS* LoadButtonImage(const char* filename, INT32 Grayed, INT32 OffNormal, INT32 OffHilite, INT32 OnNormal, INT32 OnHilite);

/* Uses a previously loaded quick button image for use with QuickButtons.  The
 * function simply duplicates the vobj!
 */
BUTTON_PICS* UseLoadedButtonImage(BUTTON_PICS* LoadedImg, INT32 Grayed, INT32 OffNormal, INT32 OffHilite, INT32 OnNormal, INT32 OnHilite);

// Removes a QuickButton image from the system.
void UnloadButtonImage(BUTTON_PICS*);

// Enables an already created button.
void EnableButton(GUIButtonRef);

/* Disables a button. The button remains in the system list, and can be
 * reactivated by calling EnableButton.  Diabled buttons will appear "grayed
 * out" on the screen (unless the graphics for such are not available).
 */
void DisableButton(GUIButtonRef);

/* Removes a button from the system's list. All memory associated with the
 * button is released.
 */
void RemoveButton(GUIButtonRef&);

void HideButton(GUIButtonRef);
void ShowButton(GUIButtonRef);

void RenderButtons(void);

// Draws a single button on the screen.
void DrawButton(GUIButtonRef);

extern BOOLEAN gfRenderHilights;

/* Creates a QuickButton. QuickButtons only have graphics associated with them.
 * They cannot be re-sized, nor can the graphic be changed.  Providing you have
 * allocated your own image, this is a somewhat simplified function.
 */
GUIButtonRef QuickCreateButton(BUTTON_PICS* image, INT16 x, INT16 y, INT16 priority, GUI_CALLBACK click);
GUIButtonRef QuickCreateButtonNoMove(BUTTON_PICS* image, INT16 x, INT16 y, INT16 priority, GUI_CALLBACK click);
GUIButtonRef QuickCreateButtonToggle(BUTTON_PICS* image, INT16 x, INT16 y, INT16 priority, GUI_CALLBACK click);

GUIButtonRef QuickCreateButtonImg(const char* gfx, INT32 grayed, INT32 off_normal, INT32 off_hilite, INT32 on_normal, INT32 on_hilite, INT16 x, INT16 y, INT16 priority, GUI_CALLBACK click);

GUIButtonRef CreateCheckBoxButton(INT16 x, INT16 y, const char* filename, INT16 Priority, GUI_CALLBACK ClickCallback);

// Creates an Iconic type button.
GUIButtonRef CreateIconButton(INT16 Icon, INT16 IconIndex, INT16 xloc, INT16 yloc, INT16 w, INT16 h, INT16 Priority, GUI_CALLBACK ClickCallback);

/* Creates a button like HotSpot. HotSpots have no graphics associated with
 * them.
 */
GUIButtonRef CreateHotSpot(INT16 xloc, INT16 yloc, INT16 Width, INT16 Height, INT16 Priority, GUI_CALLBACK ClickCallback);

// Creates a generic button with text on it.
GUIButtonRef CreateTextButton(const wchar_t* string, Font, INT16 sForeColor, INT16 sShadowColor, INT16 xloc, INT16 yloc, INT16 w, INT16 h, INT16 Priority, GUI_CALLBACK ClickCallback);

GUIButtonRef CreateIconAndTextButton(BUTTON_PICS* Image, const wchar_t* string, Font, INT16 sForeColor, INT16 sShadowColor, INT16 sForeColorDown, INT16 sShadowColorDown, INT16 xloc, INT16 yloc, INT16 Priority, GUI_CALLBACK ClickCallback);

/* This is technically not a clickable button, but just a label with text. It is
 * implemented as button */
GUIButtonRef CreateLabel(const wchar_t* text, Font, INT16 forecolor, INT16 shadowcolor, INT16 x, INT16 y, INT16 w, INT16 h, INT16 priority);

void SpecifyButtonText(GUIButtonRef, const wchar_t* string);

enum // for use with SpecifyDisabledButtonStyle
{
	DISABLED_STYLE_NONE,    // for dummy buttons, panels, etc.  Always displays normal state.
	DISABLED_STYLE_DEFAULT, // if button has text then shade, else hatch
	DISABLED_STYLE_HATCHED, // always hatches the disabled button
	DISABLED_STYLE_SHADED   // always shades the disabled button 25% darker
};
void SpecifyDisabledButtonStyle(GUIButtonRef, INT8 bStyle);

/* Note:  Text is always on top
 * If fShiftImage is true, then the image will shift down one pixel and right
 * one pixel just like the text does.
 */
void SpecifyButtonIcon(GUIButtonRef, const SGPVObject* icon, UINT16 usVideoObjectIndex, INT8 bXOffset, INT8 bYOffset, BOOLEAN fShiftImage);

void MSYS_SetBtnUserData(GUIButtonRef, INT32 userdata);
INT32 MSYS_GetBtnUserData(const GUI_BUTTON* b);
void MarkAButtonDirty(GUIButtonRef); // will mark only selected button dirty
void MarkButtonsDirty(void);// Function to mark buttons dirty ( all will redraw at next RenderButtons )
void UnMarkButtonDirty(GUIButtonRef);  // unmark button
void UnmarkButtonsDirty(void); // unmark ALL the buttoms on the screen dirty
void ForceButtonUnDirty(GUIButtonRef); // forces button undirty no matter the reason, only lasts one frame


void DrawCheckBoxButtonOnOff(GUIButtonRef, BOOLEAN on);

typedef struct ButtonDimensions
{
	UINT32 w;
	UINT32 h;
} ButtonDimensions;

const ButtonDimensions* GetDimensionsOfButtonPic(const BUTTON_PICS*);

UINT16 GetGenericButtonFillColor(void);

void ReleaseAnchorMode(void);

#endif
