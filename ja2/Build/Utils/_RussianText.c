#include "Language_Defines.h"

#ifdef RUSSIAN

/*

******************************************************************************************************
**                                  IMPORTANT TRANSLATION NOTES                                     **
******************************************************************************************************

GENERAL TOPWARE INSTRUCTIONS
- Always be aware that German strings should be of equal or shorter length than the English equivalent.
	I know that this is difficult to do on many occasions due to the nature of the German language when
	compared to English.  By doing so, this will greatly reduce the amount of work on both sides.  In
	most cases (but not all), JA2 interfaces were designed with just enough space to fit the English word.
	The general rule is if the string is very short (less than 10 characters), then it's short because of
	interface limitations.  On the other hand, full sentences commonly have little limitations for length.
	Strings in between are a little dicey.
- Never translate a string to appear on multiple lines.  All strings L"This is a really long string...",
	must fit on a single line no matter how long the string is.  All strings start with L" and end with ",
- Never remove any extra spaces in strings.  In addition, all strings containing multiple sentences only
	have one space after a period, which is different than standard typing convention.  Never modify sections
	of strings contain combinations of % characters.  These are special format characters and are always
	used in conjunction with other characters.  For example, %S means string, and is commonly used for names,
	locations, items, etc.  %d is used for numbers.  %c%d is a character and a number (such as A9).
	%% is how a single % character is built.  There are countless types, but strings containing these
	special characters are usually commented to explain what they mean.  If it isn't commented, then
	if you can't figure out the context, then feel free to ask SirTech.
- Comments are always started with // Anything following these two characters on the same line are
	considered to be comments.  Do not translate comments.  Comments are always applied to the following
	string(s) on the next line(s), unless the comment is on the same line as a string.
- All new comments made by SirTech will use "//@@@ comment" (without the quotes) notation.  By searching
	for @@@ everytime you recieve a new version, it will simplify your task and identify special instructions.
  Commonly, these types of comments will be used to ask you to abbreviate a string.  Please leave the
	comments intact, and SirTech will remove them once the translation for that particular area is resolved.
- If you have a problem or question with translating certain strings, please use "//!!! comment"
	(without the quotes).  The syntax is important, and should be identical to the comments used with @@@
	symbols.  SirTech will search for !!! to look for Topware problems and questions.  This is a more
	efficient method than detailing questions in email, so try to do this whenever possible.



FAST HELP TEXT -- Explains how the syntax of fast help text works.
**************

1) BOLDED LETTERS
	The popup help text system supports special characters to specify the hot key(s) for a button.
	Anytime you see a '|' symbol within the help text string, that means the following key is assigned
	to activate the action which is usually a button.

	EX:  L"|Map Screen"

	This means the 'M' is the hotkey.  In the game, when somebody hits the 'M' key, it activates that
	button.  When translating the text to another language, it is best to attempt to choose a word that
	uses 'M'.  If you can't always find a match, then the best thing to do is append the 'M' at the end
	of the string in this format:

	EX:  L"Ecran De Carte (|M)"  (this is the French translation)

	Other examples are used multiple times, like the Esc key  or "|E|s|c" or Space -> (|S|p|a|c|e)

2) NEWLINE
  Any place you see a \n within the string, you are looking at another string that is part of the fast help
	text system.  \n notation doesn't need to be precisely placed within that string, but whereever you wish
	to start a new line.

	EX:  L"Clears all the mercs' positions,\nand allows you to re-enter them manually."

	Would appear as:

				Clears all the mercs' positions,
				and allows you to re-enter them manually.

	NOTE:  It is important that you don't pad the characters adjacent to the \n with spaces.  If we did this
	       in the above example, we would see

	WRONG WAY -- spaces before and after the \n
	EX:  L"Clears all the mercs' positions, \n and allows you to re-enter them manually."

	Would appear as: (the second line is moved in a character)

				Clears all the mercs' positions,
 				 and allows you to re-enter them manually.


@@@ NOTATION
************

	Throughout the text files, you'll find an assortment of comments.  Comments are used to describe the
	text to make translation easier, but comments don't need to be translated.  A good thing is to search for
	"@@@" after receiving new version of the text file, and address the special notes in this manner.

!!! NOTATION
************

	As described above, the "!!!" notation should be used by Topware to ask questions and address problems as
	SirTech uses the "@@@" notation.

*/

UINT16 ItemNames[MAXITEMS][80] =
{
	L""
};


UINT16 ShortItemNames[MAXITEMS][80] =
{
	L""
};

// Different weapon calibres
// CAWS is Close Assault Weapon System and should probably be left as it is
// NATO is the North Atlantic Treaty Organization
// WP is Warsaw Pact
// cal is an abbreviation for calibre
UINT16 AmmoCaliber[][20] =
{
	L"0",
	L",38 ���",
	L"9��",
	L",45 ���",
	L",357 ���",
	L"12 ���",
	L"���",
	L"5,45��",
	L"5,56��",
	L"7,62�� ����",
	L"7,62�� ��",
	L"4,7��",
	L"5,7��",
	L"������",
	L"������",
	L"", // ������
	L"", // �����
};

// This BobbyRayAmmoCaliber is virtually the same as AmmoCaliber however the bobby version doesnt have as much room for the words.
//
// Different weapon calibres
// CAWS is Close Assault Weapon System and should probably be left as it is
// NATO is the North Atlantic Treaty Organization
// WP is Warsaw Pact
// cal is an abbreviation for calibre
UINT16 BobbyRayAmmoCaliber[][20] =
{
	L"0",
	L",38 ���",
	L"9��",
	L",45 ���",
	L",357 ���",
	L"12 ���",
	L"���",
	L"5,45��",
	L"5,56��",
	L"7,62�� �.",
	L"7,62�� ��",
	L"4,7��",
	L"5.7��",
	L"������",
	L"������",
	L"", // ������
};


UINT16 WeaponType[][30] =
{
	L"������",
	L"��������",
	L"������������",
	L"�����������",
	L"��������",
	L"�����.��������",
	L"������",
	L"������ �������",
	L"���������"
};

UINT16 TeamTurnString[][STRING_LENGTH] =
{
	L"��� ������", // player's turn
	L"��� ���������",
	L"��� ��������",
	L"��� ���������",
	L"��� �������"
	// planning turn
};

UINT16 Message[][STRING_LENGTH] =
{
	L"",

	// In the following 8 strings, the %S is the merc's name, and the %d (if any) is a number.

	L"%S:��������� � ������. ������ � ��������!",
	L"%S �������� ���� ����� � ������ � ��������!",
	L"%S �������� ���� � ����� � ������ � ����!",
	L"%S �������� ���� ��� � ������ � �����������!",
	L"%S �������� ���� ������ � ������ %d ����� ��������!",
	L"%S �������� ���� � ����� ������ %d ����� ��������!",
	L"%S �������� ���� � ����� � ������ %d ����� ����!",
	L"%S �������� ���� ��� � ������ %d ����� �����������!",
	L"�������!",

	// The first %S is a merc's name, the second is a string from pNoiseVolStr,
	// the third is a string from pNoiseTypeStr, and the last is a string from pDirectionStr

	L"", //OBSOLETE
	L"������������ �������!",

	// In the following four lines, all %S's are merc names

	L"%S ��������",
	L"%S ������������ ����� ��������!",
	L"%S ��������� ����.������.(���.�������-������)",
	L"%S � %S ��������� ����.������. (���.�������-������.)",
	// the following 17 strings are used to create lists of gun advantages and disadvantages
	// (separated by commas)
	L"�������",
	L"���������",
	L"����� ��������",
	L"������ �����.",
	L"�����.�������.",
	L"����.�������.",
	L"�����.�����",
	L"����.�����",
	L"������� ���",
	L"������� ���",
	L"������",
	L"�������",
	L"�����",
	L"���������",
	L"�� ���������",
	L"���.������",
	L"���.������",

	// In the following two lines, all %S's are merc names

	L"%S:�������� �������",
	L"%S:������� ��������� �����",

	// The first %S is a merc name and the second %S is an item name

	L"����.������:��� ��������!",
	L"%S ������ %S.",

	// The %S is a merc name

	L"%S:������ �� �����.������.",

	L"�� ������������!",
	L"����������?",

	// Both %S's are item names

	L"������ ��������� %S � %S.",

	L"������",
	L"���������",
	L"����������",

	//You cannot use "item(s)" and your "other item" at the same time.
	//Ex:  You cannot use sun goggles and you gas mask at the same time.
	L"������ ������������ %S � %S ������.",

	L"����,�� �-��� ��������� ������,����� ������������ � ������ �����,�������� �� � ���� �� ������� �����.",
	L"����,�� �-��� ��������� ������,����� ������������ � ������ �����,�������� �� � ���� �� ������� �����.(������ ��� ���� ������������.)",
	L"� ���� ������� ��� �������� �����!",
	L"���� ����� ���� %S %S",
	L"%S:��������� � ������!",
	L"�������� �����?",
	L"��� ���� ��������� � ����.�������� ��?",
	L"%S ��������� ������ �������!",
	L"%S �������� �� �������!",
	L"%S �� �������� %S!",
	L"%S ����� %S",
	L"�������� ��� ",
	L"�������?",
	L"������� ������ ���� ������",
	L"� ��� �� �����!",
  L"����� ��������������� ����������� ��������, �������� �������������� � ������.",
	L"%S �������� �������� ������ ���� ��������",
	L"��� �����-������",
};


// the names of the towns in the game

STR16 pTownNames[] =
{
	L"",
	L"������",
	L"�������",
	L"�����",
	L"����",
	L"�����",
	L"�������",
	L"��� ����",
	L"������",
	L"����",
	L"������",
	L"������",
	L"�������",
};

// the types of time compression. For example: is the timer paused? at normal speed, 5 minutes per second, etc.
// min is an abbreviation for minutes

STR16 sTimeStrings[] =
{
	L"�����",
	L"�����",
	L"5 ���",
	L"30 ���",
	L"60 ���",
	L"6 ���",
};


// Assignment Strings: what assignment does the merc  have right now? For example, are they on a squad, training,
// administering medical aid (doctor) or training a town. All are abbreviated. 8 letters is the longest it can be.

STR16 pAssignmentStrings[] =
{
	L"�����1",
	L"�����2",
	L"�����3",
	L"�����4",
	L"�����5",
	L"�����6",
	L"�����7",
	L"�����8",
	L"�����9",
	L"�����10",
	L"�����11",
	L"�����12",
	L"�����13",
	L"�����14",
	L"�����15",
	L"�����16",
	L"�����17",
	L"�����18",
	L"�����19",
	L"�����20",
	L"�� ������", // on active duty
	L"������", // ��������� ���������
	L"�������", //��������� ���������
	L"������", // in a vehicle
	L"� ����", //��������� - ����������
	L"������", // �������������
	L"��������", // �����������
  L"���������", //������� ��������� ����� �������
	L"������", // training a teammate
	L"�������", // being trained by someone else
	L"�����", // �����
	L"������.", // abbreviation for incapacitated
	L"��", // Prisoner of war - captured
	L"���������", // patient in a hospital
	L"�����",	// Vehicle is empty
};


STR16 pMilitiaString[] =
{
	L"���������", // the title of the militia box
	L"�� ���������", //the number of unassigned militia troops
	L"�� �� ������ ���������������� ���������, ����� ������ �����!",
};


STR16 pMilitiaButtonString[] =
{
	L"����", // auto place the militia troops for the player
	L"������", // done placing militia troops
};

STR16 pConditionStrings[] =
{
	L"�������", //��������� �������..�������� ��������
	L"������", //������� ��������
	L"��������", //���������� ��������
	L"�����", //����
	L"�����", // �������
	L"����������", // �������� ������
	L"��� ��������", // � ��������
	L"�������", //�������
	L"�����", // �����
};

STR16 pEpcMenuStrings[] =
{
	L"�� ������", // set merc on active duty
	L"�������", // set as a patient to receive medical aid
	L"������", // tell merc to enter vehicle
	L"��� �������", // ������ �������� �����
	L"������", // ����� �� ����� ����
};


// look at pAssignmentString above for comments

STR16 pPersonnelAssignmentStrings[] =
{
	L"�����1",
	L"�����2",
	L"�����3",
	L"�����4",
	L"�����5",
	L"�����6",
	L"�����7",
	L"�����8",
	L"�����9",
	L"�����10",
	L"�����11",
	L"�����12",
	L"�����13",
	L"�����14",
	L"�����15",
	L"�����16",
	L"�����17",
	L"�����18",
	L"�����19",
	L"�����20",
	L"�� ������",
	L"������",
	L"�������",
	L"������",
	L"� ����",
	L"������",
	L"��������",
  L"���������� �����",
	L"������",
	L"������",
	L"�����",
	L"������.",
	L"��",
	L"���������",
	L"�����",	// Vehicle is empty
};


// refer to above for comments

STR16 pLongAssignmentStrings[] =
{
	L"�����1",
	L"�����2",
	L"�����3",
	L"�����4",
	L"�����5",
	L"�����6",
	L"�����7",
	L"�����8",
	L"�����9",
	L"�����10",
	L"�����11",
	L"�����12",
	L"�����13",
	L"�����14",
	L"�����15",
	L"�����16",
	L"�����17",
	L"�����18",
	L"�����19",
	L"�����20",
	L"�� ������",
	L"������",
	L"�������",
	L"������",
	L"� ����",
	L"������",
	L"��������",
  L"������� �����",
	L"������� �������",
	L"������",
	L"�����",
	L"������.",
	L"��",
	L"���������", // patient in a hospital
	L"�����",	// Vehicle is empty
};


// the contract options

STR16 pContractStrings[] =
{
	L"������ ���������:",
	L"", // a blank line, required
	L"�����.1 ����", // offer merc a one day contract extension
	L"���������� 7��", // 1 week
	L"���������� 14��", // 2 week
	L"�������", // end merc's contract
	L"������", // stop showing this menu
};

STR16 pPOWStrings[] =
{
	L"��",  //an acronym for Prisoner of War
	L"??",
};

STR16 pLongAttributeStrings[] =
{
    L"����",
	L"��������",
	L"�����������",
	L"��������",
	L"��������",
	L"��������",
	L"��������",
	L"���������",
	L"��������",
	L"�������",
};

STR16 pInvPanelTitleStrings[] =
{
	L"�����", // the armor rating of the merc
	L"���", // the weight the merc is carrying
	L"��������", // the merc's camouflage rating
};

STR16 pShortAttributeStrings[] =
{
	L"���", // the abbreviated version of : agility
	L"���", // dexterity
	L"���", // strength
	L"���", // leadership
	L"���", // wisdom
	L"���", // experience level
	L"���", // marksmanship skill
	L"���", // explosive skill
	L"���", // mechanical skill
	L"���", // medical skill};
};


STR16 pUpperLeftMapScreenStrings[] =
{
	L"��������������", // the mercs current assignment
	L"��������", // the contract info about the merc
	L"��������", // the health level of the current merc
	L"���", // the morale of the current merc
	L"����.",	// the condition of the current vehicle
	L"�������",	// the fuel level of the current vehicle
};

STR16 pTrainingStrings[] =
{
	L"��������", // tell merc to train self
    L"���������", // tell merc to train town
	L"������", // tell merc to act as trainer
	L"������", // tell merc to be train by other
};

STR16 pGuardMenuStrings[] =
{
	L"����.��������:", // the allowable rate of fire for a merc who is guarding
	L" ����������� �����", // the merc can be aggressive in their choice of fire rates
	L" ������ �������", // conserve ammo
	L" ���� �� ��������", // fire only when the merc needs to
	L"������ �����:", // other options available to merc
	L" ���������", // merc can retreat
	L" ������ �������",  // merc is allowed to seek cover
	L" ����� ������ �������", // merc can assist teammates
	L"������", // done with this menu
	L"������", // cancel this menu
};

// This string has the same comments as above, however the * denotes the option has been selected by the player

STR16 pOtherGuardMenuStrings[] =
{
	L"��� ����:",
	L" *����������� �����*",
	L" *������ �������*",
	L" *�������.�� ��������*",
	L"������ �����:",
	L" *���������*",
	L" *������ �������*",
	L" *������ �������*",
	L"������",
	L"������",
};

STR16 pAssignMenuStrings[] =
{
	L"�� ������", // merc is on active duty
	L"������", // the merc is acting as a doctor
	L"�������", // the merc is receiving medical attention
	L"������", // the merc is in a vehicle
	L"������", // the merc is repairing items
	L"�������", // the merc is training
	L"������", // cancel this menu
};

STR16 pRemoveMercStrings[] =
{
	L"������ ��������", // remove dead merc from current team
	L"������",
};

STR16 pAttributeMenuStrings[] =
{
	L"����",
	L"��������",
	L"�����������",
	L"��������",
	L"��������",
	L"��������",
	L"��������",
	L"���������",
	L"��������",
	L"������",
};

STR16 pTrainingMenuStrings[] =
{
 L"��������", // train yourself
 L"�����.", // train the town
 L"������", // train your teammates
 L"������",  // be trained by an instructor
 L"������", // cancel this menu
};


STR16 pSquadMenuStrings[] =
{
	L"�����  1",
	L"�����  2",
	L"�����  3",
	L"�����  4",
	L"�����  5",
	L"�����  6",
	L"�����  7",
	L"�����  8",
	L"�����  9",
	L"����� 10",
	L"����� 11",
	L"����� 12",
	L"����� 13",
	L"����� 14",
	L"����� 15",
	L"����� 16",
	L"����� 17",
	L"����� 18",
	L"����� 19",
	L"����� 20",
	L"������",
};

STR16 pPersonnelTitle[] =
{
	L"��������", // the title for the personnel screen/program application
};

STR16 pPersonnelScreenStrings[] =
{
	L"��������: ", // health of merc
	L"�����������: ",
	L"��������: ",
	L"����: ",
	L"���������: ",
	L"��������: ",
	L"���������: ", // experience level
	L"��������: ",
	L"��������: ",
	L"��������.: ",
	L"��������: ",
	L"���.�������: ", // amount of medical deposit put down on the merc
	L"��������: ", // cost of current contract
	L"��������: ", // number of kills by merc
	L"���������: ", // number of assists on kills by merc
	L"���� � ����:", // daily cost of merc
	L"����� ���������:", // total cost of merc
	L"��������:", // cost of current contract
	L"��� ������:", // total service rendered by merc
	L"����� �� �/�:", // amount left on MERC merc to be paid
	L"����.���������:", // percentage of shots that hit target
	L"�����:", // number of battles fought
	L"���-�� ���:", // number of times merc has been wounded
	L"������:",
	L"��� �������",
};


//These string correspond to enums used in by the SkillTrait enums in SoldierProfileType.h
STR16 gzMercSkillText[] =
{
	L"��� �������",
	L"���.� ��������",
	L"������ � �����",
	L"�����������",
	L"������ ����.",
	L"������",
	L"��������",
	L"���.������",
	L"�����.������",
	L"��������",
	L"��.���������",
	L"���",
	L"������� ���-��",
	L"������� ����",
	L"�������� � �����",
	L"����������",
	L"(�������)",
};


// This is pop up help text for the options that are available to the merc

STR16 pTacticalPopupButtonStrings[] =
{
	L"������/���� (|S)",
	L"C��������/�������� (|C)",
	L"������/������ (|R)"
	L"����/������ (|P)",
	L"�������� (|L)",
	L"��������",
	L"��������",
	L"��������� (|C|t|r|l)",

	// Pop up door menu
	L"������� �������",
	L"����� �������",
	L"����� �������",
	L"��������� �������",
	L"�����",
	L"����������",
	L"�������",
	L"��������",
	L"���.����������",
	L"�����",
	L"������(|E|s|c)"
};

// Door Traps. When we examine a door, it could have a particular trap on it. These are the traps.

STR16 pDoorTrapStrings[] =
{
	L"������� ���",
	L"�����-�������",
	L"��������������",
	L"�������-������",
	L"����� ������������"
};

// Contract Extension. These are used for the contract extension with AIM mercenaries.

STR16 pContractExtendStrings[] =
{
	L"����",
	L"7��.",
	L"14��.",
};

// On the map screen, there are four columns. This text is popup help text that identifies the individual columns.

STR16 pMapScreenMouseRegionHelpText[] =
{
	L"������� ��������",
	L"��������� ��������",
	L"���������",
	L"����� |�������� (|C)",
	L"������ �����",
	L"�����",
};

// volumes of noises

STR16 pNoiseVolStr[] =
{
	L"�����",
	L"������",
	L"�������",
	L"��.�������"
};

// types of noises

STR16 pNoiseTypeStr[] = // OBSOLETE
{
	L"����������",
	L"���� ��������",
	L"�����",
	L"�����",
	L"����",
	L"�������",
	L"�����",
	L"����",
	L"����",
	L"����",
	L"����",
	L"������"
};

// Directions that are used to report noises

STR16 pDirectionStr[] =
{
	L"���-������",
	L"������",
	L"���-������",
	L"��",
	L"���-�����",
	L"�����",
	L"���-�����",
	L"�����"
};

// These are the different terrain types.

STR16 pLandTypeStrings[] =
{
	L"�����",
	L"������",
	L"�������",
	L"�������",
	L"����",
	L"����",
	L"������",
	L"����",
	L"�����",
	L"�����������",
	L"����",	//river from north to south
	L"����",	//river from east to west
	L"����� ������",
	//NONE of the following are used for directional travel, just for the sector description.
	L"�������",
	L"�����",
	L"����, ������",
	L"����, ������",
	L"�����, ������",
	L"�������,������",
	L"����, ������",
	L"�����",
	L"����, ������",
	L"���������,������",
	L"�������, ������",
	L"������, ������",
	L"����,���",
	L"�������,���",
	L"�������,���",
	L"������,���",

	//These are descriptions for special sectors
	L"������.�������",
	L"�����.��������",
	L"�����.������",
	L"���",
	L"���� ��������.", //The rebel base underground in sector A10
	L"������.�����",	//The basement of the Tixa Prison (J9)
	L"������ �������",	//Any mine sector with creatures in it
	L"������� ����",	//The basement of Orta (K4)
	L"�������",				//The tunnel access from the maze garden in Meduna
										//leading to the secret shelter underneath the palace
	L"�������",				//The shelter underneath the queen's palace
	L"",							//Unused
};

STR16 gpStrategicString[] =
{
	L"",	//Unused
	L"%S ��������� � ������� %c%d � ���-��� �������� ��� ���� �����.",	//STR_DETECTED_SINGULAR
	L"%S ��������� � ������� %c%d � ���-��� �������� ��� ������.",	//STR_DETECTED_PLURAL
	L"�� ������ �������������� ������������� ��������?",													//STR_COORDINATE

	//Dialog strings for enemies.

	L"���� ���� ���� �������.",			//STR_ENEMY_SURRENDER_OFFER
	L"���� �������� ������ ��������, ������� ��������� ��� ��������.",	//STR_ENEMY_CAPTURED

	//The text that goes on the autoresolve buttons

	L"�������.", 		//The retreat button				//STR_AR_RETREAT_BUTTON
	L"������",		//The done button				//STR_AR_DONE_BUTTON

	//The headers are for the autoresolve type (MUST BE UPPERCASE)

	L"������",								//STR_AR_DEFEND_HEADER
	L"�����",								//STR_AR_ATTACK_HEADER
	L"������",								//STR_AR_ENCOUNTER_HEADER
	L"������",		//The Sector A9 part of the header		//STR_AR_SECTOR_HEADER

	//The battle ending conditions

	L"������!",								//STR_AR_OVER_VICTORY
	L"������!",								//STR_AR_OVER_DEFEAT
	L"������!",							//STR_AR_OVER_SURRENDERED
	L"��������!",								//STR_AR_OVER_CAPTURED
	L"��������!",								//STR_AR_OVER_RETREATED

	//These are the labels for the different types of enemies we fight in autoresolve.

	L"�������",							//STR_AR_MILITIA_NAME,
	L"�����",								//STR_AR_ELITE_NAME,
	L"������",								//STR_AR_TROOP_NAME,
	L"�����",								//STR_AR_ADMINISTRATOR_NAME,
	L"��������",								//STR_AR_CREATURE_NAME,

	//Label for the length of time the battle took

	L"����� �������",							//STR_AR_TIME_ELAPSED,

	//Labels for status of merc if retreating.  (UPPERCASE)

	L"��������",								//STR_AR_MERC_RETREATED,
	L"���������",								//STR_AR_MERC_RETREATING,
	L"�����������",								//STR_AR_MERC_RETREAT,

	//PRE BATTLE INTERFACE STRINGS
	//Goes on the three buttons in the prebattle interface.  The Auto resolve button represents
	//a system that automatically resolves the combat for the player without having to do anything.
	//These strings must be short (two lines -- 6-8 chars per line)

	L"���� �����",							//STR_PB_AUTORESOLVE_BTN,
	L"���� � ������",							//STR_PB_GOTOSECTOR_BTN,
	L"���������",							//STR_PB_RETREATMERCS_BTN,

	//The different headers(titles) for the prebattle interface.
	L"������",									//STR_PB_ENEMYENCOUNTER_HEADER,
	L"�������. ������",							//STR_PB_ENEMYINVASION_HEADER, // 30
	L"����. ������",							//STR_PB_ENEMYAMBUSH_HEADER
	L"�������� �� ����. ������",				//STR_PB_ENTERINGENEMYSECTOR_HEADER
	L"����� �������",							//STR_PB_CREATUREATTACK_HEADER
	L"������ �����",							//STR_PB_BLOODCATAMBUSH_HEADER
	L"���� � ������ �����-������",			//STR_PB_ENTERINGBLOODCATLAIR_HEADER

	//Various single words for direct translation.  The Civilians represent the civilian
	//militia occupying the sector being attacked.  Limited to 9-10 chars

	L"�����",
	L"�����",
	L"��������",
	L"���������",
	L"��������",
	L"�����-��",
	L"������",
	L"������",		//If there are no uninvolved mercs in this fight.
	L"�/�",			//Acronym of Not Applicable
	L"�",			//One letter abbreviation of day
	L"�",			//One letter abbreviation of hour

	//TACTICAL PLACEMENT USER INTERFACE STRINGS
	//The four buttons

	L"��������",
	L"�������",
	L"������",
	L"������",

	//The help text for the four buttons.  Use \n to denote new line (just like enter).

	L"������ ������� ��������� \n��� ���������� �� ����� ( |C).",
	L"�������������� ��������� ������� (|S).",
	L"������� ����� ����� ��������� (|G).",
	L"������� ��� ������, ����� ��������� \n����� ������� ��� ���������. (|E|n|t|e|r)",
	L"�� ������ ���������� ���� �����. \n����� ������� �����.",

	//Various strings (translate word for word)

	L"������",
	L"������� ����� �����",

	//Strings used for various popup message boxes.  Can be as long as desired.

	L"�������� ����������������. ����� ����������. �������� ������ �����.",
	L"��������� ����� ��������� � ���������� ����� �� �����.",

	//This message is for mercs arriving in sectors.  Ex:  Red has arrived in sector A9.
	//Don't uppercase first character, or add spaces on either end.

	L"������ � ������",

	//These entries are for button popup help text for the prebattle interface.  All popup help
	//text supports the use of \n to denote new line.  Do not use spaces before or after the \n.
	L"����� ����������� �������������\n��� �������� �����(|A)",
	L"������ ���.�������������� �����\n����� �������.",
	L"����� � ������:������ � ������ (|E).",
	L"������ ��������� � ������� ������ (|R).",				//singular version
	L"��� ������ ��������� � ������� ������� (|R)", //multiple groups with same previous sector
//!!!What about repeated "R" as hotkey?
	//various popup messages for battle conditions.

	//%c%d is the sector -- ex:  A9
	L"����� ������� ���� ��������� � ������� %c%d.",
	//%c%d ������ -- ����:  A9
	L"�������� ������� ���� �����.� ������� %c%d.",
	//1st %d refers to the number of civilians eaten by monsters,  %c%d is the sector -- ex:  A9
	//Note:  the minimum number of civilians eaten will be two.
	L"�������� ������� � ������� %d ������� � ������� %S.",
	//%S is the sector location -- ex:  A9: Omerta
	L"����� ������� ����� �����.� ������� %S. ����� �� ��������� �� ����� �������!",
	//%S is the sector location -- ex:  A9: Omerta
	L"�������� ������� ����� �����.� �������%S. ����� �� ��������� �� ����� �������!",

};

STR16 gpGameClockString[] =
{
	//This is the day represented in the game clock.  Must be very short, 4 characters max.
	L"����",
};

//When the merc finds a key, they can get a description of it which
//tells them where and when they found it.
STR16 sKeyDescriptionStrings[2] =
{
	L"����.�������:",
	L"���� �������:",
};

//The headers used to describe various weapon statistics.

INT16		gWeaponStatsDesc[][ 14 ] =
{
	L"��� (%S):",
	L"������:",
	L"����:", 		// Number of bullets left in a magazine
	L"����:",		// Range
	L"����:",		// Damage
	L"��:",			// abbreviation for Action Points
	L"",
	L"=",
	L"=",
};

//The headers used for the merc's money.

INT16 gMoneyStatsDesc[][ 13 ] =
{
	L"���-��",
	L"��������:", //this is the overall balance
	L"���-��",
	L"��������:", // the amount he wants to separate from the overall balance to get two piles of money

	L"�������",
	L"������",
	L"���-��",
	L"�����",
};

//The health of various creatures, enemies, characters in the game. The numbers following each are for comment
//only, but represent the precentage of points remaining.

UINT16 zHealthStr[][13] =
{
	L"�������",		//	>= 0
	L"��������", 		//	>= 15
	L"����",		//	>= 30
	L"�����",    	//	>= 45
	L"������",    	//	>= 60
	L"�����",     	// 	>= 75
  L"�������",		// 	>= 90
};

STR16	gzMoneyAmounts[6] =
{
	L"1000$",
	L"100$",
	L"10$",
	L"������",
	L"��������",
	L"�����"
};

// short words meaning "Advantages" for "Pros" and "Disadvantages" for "Cons."
INT16		gzProsLabel[10] =
{
	L"��:",
};

INT16		gzConsLabel[10] =
{
	L"����:",
};

//Conversation options a player has when encountering an NPC
UINT16 zTalkMenuStrings[6][ SMALL_STRING_LENGTH ] =
{
	L"��� ���?", 	//meaning "Repeat yourself"
	L"��������",		//approach in a friendly
	L"�����",		//approach directly - let's get down to business
	L"��������",		//approach threateningly - talk now, or I'll blow your face off
	L"����",
	L"������"
};

//Some NPCs buy, sell or repair items. These different options are available for those NPCs as well.
UINT16 zDealerStrings[4][ SMALL_STRING_LENGTH ]=
{
	L"���/����",
	L"���.",
	L"����.",
	L"������",
};

UINT16 zDialogActions[1][ SMALL_STRING_LENGTH ] =
{
	L"������",
};


//These are vehicles in the game.

STR16 pVehicleStrings[] =
{
 L"���������",
 L"������", // a hummer jeep/truck -- military vehicle
 L"���� � �����",
 L"����",
 L"����",
 L"��������",
};

STR16 pShortVehicleStrings[] =
{
	L"������",
	L"������",			// the HMVV
	L"����",
	L"����",
	L"����",
	L"����", 				// the helicopter
};

STR16	zVehicleName[] =
{
	L"���������",
	L"������",		//a military jeep. This is a brand name.
	L"����",			// Ice cream truck
	L"����",
	L"����",
	L"����", 		//an abbreviation for Helicopter
};


//These are messages Used in the Tactical Screen

UINT16 TacticalStr[][ MED_STRING_LENGTH ] =
{
	L"��������� ����",
	L"��������� ����.������ �����?",

	// CAMFIELD NUKE THIS and add quote #66.

	L"%S ��������, ��� ��������� �������� �� ���������.",

	// The %S is a string from pDoorTrapStrings

	L"����� (%S).",
	L"��� ��� �����.",
	L"�����!",
	L"������.",
	L"�����!",
	L"������",
	L"����� ��� �������",
	L"�����!",
	// The %S is a merc name
	L"%S:��� ������� �����",
	L"����� ��� �������",
	L"����� ��� �������",
	L"�������",
	L"�����",
	L"�������",
	L"�������",
	L"���������",
	L"�����������",
	L"��� ���� �����������.������?",
	L"��������� �������?",
	L"����...",
	L"����...",
	L"���...",

	// In the next 2 strings, %S is an item name

	L"%S �������(�) �� �����.",
	L"%S �����(�) %S.",

	// In the next 2 strings, %S is a name

	L"%S.�������� ������.",
	L"%S.��� ������ %d.",
	L"������� ������� ����������:",  	//in this case, frequency refers to a radio signal
	L"���-�� ����� ����� �������:",	//how much time, in turns, until the bomb blows
	L"�����.������� �������.����������:", 	//in this case, frequency refers to a radio signal
	L"��������� �������?",
	L"������ ������� ����?",
	L"���������� ������� ����?",
	L"����������� ���",

	// In the next string, %S is a name. Stance refers to way they are standing.

	L"������,��� ������ ������� �� %S ?",
	L"������ �� ����� ������ ���������.",
	L"����� �� ����� ������ ���������.",

	// In the next 3 strings, %S is a name

	L"%S �� ����� �������� ��������� �����.",
	L"%S �� ����� �������� ����.������.",
	L"%S �� ��������� � ����.������.",
	L"���� ���� ������.",
	L"������� �������.���� ���.",	//there's no room for a recruit on the player's team

	// In the next string, %S is a name

	L"%S �����.",

	// Here %S is a name and %d is a number

	L"%S ������ �������� $%d.",

	// In the next string, %S is a name

	L"������. %S?",

	// In the next string, the first %S is a name and the second %S is an amount of money (including $ sign)

	L"������ %S �� %S � ����?",

	// This line is used repeatedly to ask player if they wish to participate in a boxing match.

	L"������ �������?",

	// In the next string, the first %S is an item name and the
	// second %S is an amount of money (including $ sign)

	L"������ %S �� %S?",

	// In the next string, %S is a name

	L"%S ����������� � ����� %d.",

	// These messages are displayed during play to alert the player to a particular situation

	L"���������",					//weapon is jammed.
	L"������ ����� ���� %S �������.",		//Robot is out of ammo
	L"������� ����? ���. �� ������.",		//Merc can't throw to the destination he selected

	// These are different buttons that the player can turn on and off.

	L"������� (|Z)",
	L"���� ����� (|M)",
	L"������ (|D)(��������� ���)",
	L"��������",
	L"��� �����",
	L"��������� (|P|g|U|p)",
	L"���.�������(|T|a|b)",
	L"������./ ����.",
	L"���������� (|P|g|D|n)",
	L"��������� (|C|t|r|l)",
	L"����.�������",
	L"����.������� (|S|p|a|c|e)",
	L"��������� (|O)",
	L"������� (|B)",
	L"��������/����������� (|L)",
	L"��������: %d/%d\n�����.: %d/%d\n���: %S",
	L"����?",					//this means "what?"
	L"�������.",					//an abbrieviation for "Continued"
	L"���.���� ��� %S.",
	L"����.���� ��� %S.",
	L"��������: %d/%d\n����: %d/%d",
	L"����� �� ������" ,
	L"�������� ����� ( |S|h|i|f|t |S|p|a|c|e )",
	L"�����",
	L"�/�",						//this is an acronym for "Not Applicable."
	L"��� ( ���� � ���� )",
	L"��� ( �������.��. )",
	L"��� ( ������ )",
	L"��� ( ���������� )",
	L"��� ( ������� )",
	L"(�������)",
	L"(������������)",
	L"(����)",
	L"%S ���������.",
	L"%S ������.",
	L"%S:��� ����� ��������.",
	L"%S ����������.",
	L"%S ���� � ������.",
	L"%S:����� �����.",
	L"���� � �������!",
	L"����� �� �����.",
	L"�� ������� ����� ��������.",
	L"����� �� ���.�������.���.",
	L"������ ��������!",
	L"������",
	L"��������",
	L"���������",
	L"������",
	L"���� �� �������",
	L"OK",
	L"������",
	L"��������� �������",
	L"��� �������� � ������",
	L"���� � ������",
	L"���� �� �����",
	L"���� ������ ������ �������� ������.",
	L"%S ������� ������.",
	L"�������� �������",
	L"�������� �������",
	L"������",				//Crow, as in the large black bird
	L"���",
	L"������",
	L"����",
	L"����",
	L"������� �������� ��,��� ��� ����� �����?",
	L"��������� ������� ��������",
	L"��������� ��������. ������ �� ���������",
	L"���� ���������",
	L"���� ����������",
	L"��������/����� ������ �� �����",		//Help text over the $ button on the Single Merc Panel
	L"��������� ������ �� �����.",
	L"����.",											// Short form of JAMMED, for small inv slots
	L"���� �� ���������.",					// used ( now ) for when we click on a cliff
	L"���� ����������. ������ ���������� ������� � ���� ���������?",
	L"������� ������������ ���������.",
	// In the following message, '%S' would be replaced with a quantity of money (e.g. $200)
	L"�� �������� ��������� %S?",
	L"������� ���������� �������?",
	L"�������� ������ �������?",
	L"������� ������ ����������",
	L"� �������������� ����� ������� ������.",
	L"�������� ��������?",
	L"��� ������������ ��� ������",
	L"������",
	L"������ ����� ������ ����� ������.",
	L"������������� ������� ������ ������",
	L"���� ��� %S ����������",
	L"��������, ����������� ������ ���������, ������� �����",
	L"����� �������",
	L"����� ��������",
	L"���-�� ��� �������� ����������.���� ������.",
	L"��������: %d/%d\n����: %d/%d",
  L"%S �� ����� %S.",  // Cannot see person trying to talk to
};

//Varying helptext explains (for the "Go to Sector/Map" checkbox) what will happen given different circumstances in the "exiting sector" interface.
STR16 pExitingSectorHelpText[] =
{
	//Helptext for the "Go to Sector" checkbox button, that explains what will happen when the box is checked.
	L"����� �������� �������� ������ ����� ����� ������.",
	L"����� �������� �� ������������� ������������ � ���� ����� �\n������ �������� ����������� ����� �� ������.",

	//If you attempt to leave a sector when you have multiple squads in a hostile sector.
	L"���� ������ ����� ������� � ����� ��������� ��������� ������.\n���� ������ ��� �������� ����� ��� ��� �������� ������ �������.",

	//Because you only have one squad in the sector, and the "move all" option is checked, the "go to sector" option is locked to on.
	//The helptext explains why it is locked.
	L"������ ���������� ��������� �� ����� �������,\n����, ��� �������� ������ ����� ����� ����������.",
	L"������ ���������� ��������� �� ����� �������,\n�� ������������� ������������� � ���� ����� \n������ �������� ����������� ����� �� ������.",

	//If an EPC is the selected merc, it won't allow the merc to leave alone as the merc is being escorted.  The "single" button is disabled.
	L"%S �� ����� �������� ���� ������ ����, ��� ���� �����������.",

	//If only one conscious merc is left and is selected, and there are EPCs in the squad, the merc will be prohibited from leaving alone.
	//There are several strings depending on the gender of the merc and how many EPCs are in the squad.
	//DO NOT USE THE NEWLINE HERE AS IT IS USED FOR BOTH HELPTEXT AND SCREEN MESSAGES!
	L"%S �� ����� �������� ������ ����-�� ������������ %S.", //male singular
	L"%S �� ����� �������� ������ ����-��� ������������ %S.", //female singular
	L"%S �� ����� �������� ������ ����-��  ������������ ������.", //male plural
	L"%S �� ����� �������� ������ ����-��� ������������ ������.", //female plural

	//If one or more of your mercs in the selected squad aren't in range of the traversal area, then the  "move all" option is disabled,
	//and this helptext explains why.
	L"����� ���� ����� ��� �����,\n��� ���� �������� ����� ���� � �����.",

	L"", //UNUSED

	//Standard helptext for single movement.  Explains what will happen (splitting the squad)
	L"����� �������� %S ������ ����, �\n������������� ������� � ���������� �����.",

	//Standard helptext for all movement.  Explains what will happen (moving the squad)
	L"����� �������� ��������� ���� ������ \n����� ������� ���� ������.",

	//This strings is used BEFORE the "exiting sector" interface is created.  If you have an EPC selected and you attempt to tactically
	//traverse the EPC while the escorting mercs aren't near enough (or dead, dying, or unconscious), this message will appear and the
	//"exiting sector" interface will not appear.  This is just like the situation where
	//This string is special, as it is not used as helptext.  Do not use the special newline character (\n) for this string.
	L"%S �� ����� �������� ���� ������ ����, ��� ���� �����������. ��������� �������� �������� ���� � ����.",
};



STR16 pRepairStrings[] =
{
	L"����", 		// tell merc to repair items in inventory
	L"���", 		// tell merc to repair SAM site - SAM is an acronym for Surface to Air Missile
	L"������", 		// cancel this menu
	L"�����", 		// repair the robot
};


// NOTE: combine prestatbuildstring with statgain to get a line like the example below.
// "John has gained 3 points of marksmanship skill."

STR16 sPreStatBuildString[] =
{
	L"�������", 			// the merc has lost a statistic
	L"��������", 		// the merc has gained a statistic
	L"����",	// singular
	L"����",	// plural
	L"�������",	// singular
	L"������",	// plural
};

STR16 sStatGainStrings[] =
{
	L"����.",
	L"�������.",
	L"�������.",
	L"��������.",
	L"��������",
	L"������.������.",
	L"��������.",
	L"��������.",
	L"���������.",
	L"����.",
	L"���������.",
};


STR16 pHelicopterEtaStrings[] =
{
	L"����� �����.:  ", 			// total distance for helicopter to travel
	L" ���������:  ", 			// distance to travel to destination
	L" ������:", 			// distance to return from destination to airport
	L"���.����: ", 		// total cost of trip by helicopter
	L"���:  ", 			// ETA is an acronym for "estimated time of arrival"
	L"� ��������� ���� �������, �� ������.����� �� ����.����������!",	// warning that the sector the helicopter is going to use for refueling is under enemy control ->
  L"���������: ",
  L"������� ������� �������� ��� �����������?",
  L"�������",
  L"�����������",
};

STR16 sMapLevelString[] =
{
	L"���������� ", 			// what level below the ground is the player viewing in mapscreen
};

STR16 gsLoyalString[] =
{
	L"�����",	// the loyalty rating of a town ie : Loyal 53%
};


// error message for when player is trying to give a merc a travel order while he's underground.

STR16 gsUndergroundString[] =
{
	L"�� ������.�������� ���� ��� ������.",
};

STR16 gsTimeStrings[] =
{
	L"�",				// hours abbreviation
	L"�",				// minutes abbreviation
	L"�",				// seconds abbreviation
	L"�",				// days abbreviation
};

// text for the various facilities in the sector

STR16 sFacilitiesStrings[] =
{
	L"������",
	L"������.",
	L"������",
	L"������",
	L"�������.",
	L"��������",
	L"����������",		// a field for soldiers to practise their shooting skills
};

// text for inventory pop up button

STR16 pMapPopUpInventoryText[] =
{
	L"���������",
	L"�����",
};

// town strings

STR16 pwTownInfoStrings[] =
{
	L"������",					// 0 // size of the town in sectors
	L"", 						// blank line, required
	L"��������", 					// how much of town is controlled
	L"������", 					// none of this town
	L"����� ������", 				// mine associated with this town
	L"��������",					// 5 // the loyalty level of this town
	L"������", 					// the forces in the town trained by the player
	L"",
	L"���.������.", 				// main facilities in this town
	L"�������", 					// the training level of civilians in this town
	L"���������� �������",				// 10 // state of civilian training in town
	L"���������", 					// the state of the trained civilians in the town
};

// Mine strings

STR16 pwMineStrings[] =
{
	L"�����",						// 0
	L"�������",
	L"������",
	L"����������/����",
	L"�����������.����-��",
	L"�������",				// 5
	L"�������",
	L"����������",
	L"��������",
	L"������",
	L"������������������",
	L"��� ����",				// 10
	L"����� ��������",
	L"��������� ������",
//	L"�����.�������",
};

// blank sector strings

STR16 pwMiscSectorStrings[] =
{
	L"���� �����",
	L"������",
	L"# �����",
	L"�����.",
	L"��� �����.",
	L"��",
	L"���",
};

// error strings for inventory

STR16 pMapInventoryErrorString[] =
{
	L"%S ������������ ������",	//Merc is in sector with item but not close enough
	L"������ ������� �����.",  //MARK CARTER
	L"%S �� � ������� � �� ����� ����� ��� ����",
	L"�� ����� ����� ���� ��������� ��� ���� �������",
	L"�� ����� ����� ���� ������� ���� �������.",
	L"%S �� � �������,����� ������� ����.",
};

STR16 pMapInventoryStrings[] =
{
	L"�����", 			// sector these items are in
	L"����� �����", 		// total number of items in sector
};


// help text for the user

STR16 pMapScreenFastHelpTextList[] =
{
	L"����� ���� �������� ����� ������� ��� ���� � ��.�����, ������� ��� ������,�������� ������ � '��������������'",
	L"����� ��������� �������� � ������ ������, �������� ������ � ������� '����'",
	L"����� �������� �������� ������ ������ ��������,���������� �������� �� ��� �������.",
	L"����� ������-������� ������. ��� ��� ����� ������-���� �������� ������� ������ ��������,������ ������-����� ���������� � �������.",
	L"������'h' � ����� �����, ����� ������� ���������.",
	L"��������",
	L"��������",
	L"��������",
	L"��������",
	L"���� ������� �� ��������� �� �������,� ���� ���� ��� ������ ������.����� �� ������������� ���� �������,������� �� ������ ������ ������� � ������ ������ ���� ������.������� ��������� ������� �������.",
};

// movement menu text

STR16 pMovementMenuStrings[] =
{
	L"���������� �����.� ������", 	// title for movement box
	L"����", 		// done with movement menu, start plotting movement
	L"������", 		// cancel this menu
	L"������",		// title for group of mercs not on squads nor in vehicles
};


STR16 pUpdateMercStrings[] =
{
	L"��!:", 			// an error has occured
	L"��������� �����������:", 	// this pop up came up due to a merc contract ending
	L"������� �������� �������:", // this pop up....due to more than one merc finishing assignments
	L"�����.����� ��������:", // this pop up ....due to more than one merc waking up and returing to work
	L"�������� ���� �����:", // this pop up ....due to more than one merc being tired and going to sleep
	L"��������� ����� ��������:", // this pop up came up due to a merc contract ending
};

// map screen map border buttons help text

STR16 pMapScreenBorderButtonHelpText[] =
{
	L"�������� ������ (|W)",
	L"�������� ����� (|M)",
	L"�����.������� � ������(|T)",
	L"�������� ��������� ������������(|A)",
	L"�������� ���� (|I)",
	L"�����.�������.� ������(|Z)",
};


STR16 pMapScreenBottomFastHelp[] =
{
	L"������ (|L)",
	L"�������(|E|s|c)",
	L"��������� (|O)",
	L"������ ����.(|+)", 	// time compress more
	L"������ ����.(|-)", 	// time compress less
	L"��������.����� (|U|p)\n��������.���. (|P|g|U|p)", 	// previous message in scrollable list
	L"����.�����. (|D|o|w|n)\n����.���. (|P|g|D|n)", 	// next message in the scrollable list
	L"�������/���������� ����� (|S|p|a|c|e)",	// start/stop time compression
};

STR16 pMapScreenBottomText[] =
{
	L"������� ������", 	// current balance in player bank account
};

STR16 pMercDeadString[] =
{
	L"%S �����.",
};


STR16 pDayStrings[] =
{
	L"����",
};

// the list of email sender names

STR16 pSenderNameList[] =
{
	L"������",
	L"���� ��� ���",
	L"������",
	L"����.��� ���",
	L"����",
	L"R.I.S.",		//5
	L"�����",
	L"����",
	L"����",
	L"������",
	L"����",			//10
	L"������",
	L"����",
	L"����",
	L"��������",
	L"�����",			//15
	L"����",
	L"�����",
	L"�����������",
	L"������",
	L"�����",				//20
	L"������",
	L"���",
	L"�����",
	L"���",
	L"����",		//25
	L"�����",
	L"���",
	L"�������",
	L"�����",
	L"������",		//30
	L"����",
	L"������",
	L"���",
	L"�����",
	L"���",
	L"������",
	L"�����",
	L"�������",
	L"������ ���",
	L"������",
	L"���",
	L"�������",
	L"����",
	L"����",
	L"����",
	//----------
	L"M.I.S.�����.",
	L"����� ���",
	L"����",
	L"���� �����",
	L"�.I.�.",
};


// next/prev strings

STR16 pTraverseStrings[] =
{
  L"������",
  L"����",
};

// new mail notify string

STR16 pNewMailStrings[] =
{
 L"���� ����� ���������...",
};


// confirm player's intent to delete messages

STR16 pDeleteMailStrings[] =
{
 L"������� ���������?",
 L"������� ������������?",
};


// the sort header strings

STR16 pEmailHeaders[] =
{
	L"��:",
	L"����:",
	L"����:",
};

// email titlebar text

STR16 pEmailTitleText[] =
{
	L"�������� ����",
};


// the financial screen strings
STR16 pFinanceTitle[] =
{
	L"��������� ����",		//the name we made up for the financial program in the game
};

STR16 pFinanceSummary[] =
{
	L"������:", 				// credit (subtract from) to player's account
	L"�����:", 				// debit (add to) to player's account
	L"������ �� ��������� ����:",
	L"�������� �� �������.����:",
	L"����� �� �������. ����:",
	L"������ �� ����� ���:",
	L"������ �� �������:",
	L"�������� �� �������:",
	L"����� �� �������:",
	L"������� ������:",
	L"�������������� ������:",
	L"�������������� ������:", 		// projected balance for player for tommorow
};


// headers to each list in financial screen

STR16 pFinanceHeaders[] =
{
  L"Day", 					// the day column
	L"������", 				// the credits column
	L"�����",				// the debits column
	L"�������", 			// transaction type - see TransactionText below
	L"������", 				// balance at this point in time
	L"���.", 				// page number
	L"��.", 				// the day(s) of transactions this page displays
};


STR16 pTransactionText[] =
{
	L"�������",			// interest the player has accumulated so far
	L"��������� ������",
	L"���� �� �������",
	L"�����", 				// Merc was hired
	L"�������� ����� ���", 		// Bobby Ray is the name of an arms dealer
	L"���������.����� � M.E.R.C.",
	L"��� �������: %S", 		// medical deposit for merc
	L"IMP ������", 		// IMP is the acronym for International Mercenary Profiling
	L"������� ���������:%S",
	L"�������� ���������: %S",
	L"��������� ���������: %S", 				// johnny contract extended
	L"�������� ���������: %S",
	L"��������� ������: %S", 		// insurance claim for merc
	L"� ����", 				// merc's contract extended for a day
	L"7 ����", 				// merc's contract extended for a week
	L"14 ����", 				// ... for 2 weeks
	L"����� � ����",
	L"", //String nuked
	L"�������� �������",
	L"������ ������ ��������.: %S",
	L"�������.������ ��������: %S",
	L"��������� �� ��������: %S",
	L"�������: %S",		// %S is the name of the npc being paid
	L"������� ������� �� ��� %S", 			// transfer funds to a merc
	L"������� ������� �� %S", 		// transfer funds from a merc
	L"�����.���������� �����: %S", // initial cost to equip a town's militia
	L"������� � %S.",	//is used for the Shop keeper interface.  The dealers name will be appended to the end of the string.
	L"%S ������� ������.",
};

STR16 pTransactionAlternateText[] =
{
	L"���������", 				// insurance for a merc
	L"%S:�������� �������� �� 1 ����", 				// entend mercs contract by a day
	L"%S:�������� �������� �� 7 ����",
	L"%S:�������� �������� �� 14 ����",
};

// helicopter pilot payment

STR16 pSkyriderText[] =
{
	L"�������� ��������� $%d", 			// skyrider was paid an amount of money
	L"�������� ����������� $%d", 		// skyrider is still owed an amount of money
	L"�������. �������� ���������",	// skyrider has finished refueling
	L"",//unused
	L"",//unused
	L"������� ����� � ������.", // Skyrider was grounded but has been freed
	L"� �������� ��� ����������.���� �� ������ ��������� ��������� � ���� ������, �������� �������. � ������"
};


// strings for different levels of merc morale

STR16 pMoralStrings[] =
{
	L"�������",
	L"������",
	L"����.",
	L"�������",
	L"������",
	L"����",
};

// Mercs equipment has now arrived and is now available in Omerta or Drassen.

STR16 pLeftEquipmentString[] =
{
	L"%S:���������� ����� �������� � ������( A9 ).",
	L"%S:���������� ����� �������� � ��������( B13 ).",
};

// Status that appears on the Map Screen

STR16 pMapScreenStatusStrings[] =
{
	L"��������",
	L"�������",
	L"���",
	L"����.",	// the condition of the current vehicle (its "health")
	L"������",	// the fuel level of the current vehicle (its "energy")
};


STR16 pMapScreenPrevNextCharButtonHelpText[] =
{
	L"����.������� (|L|e|f|t)", 			// previous merc in the list
	L"����.������� (|R|i|g|h|t)", 				// next merc in the list
};


STR16 pEtaString[] =
{
	L"���:", 				// eta is an acronym for Estimated Time of Arrival
};

STR16 pTrashItemText[] =
{
	L"�� ��������� ��� ��������.���������?", 	// do you want to continue and lose the item forever
	L"���,�������,� ������� ������ ����.�� ������ �������, ��� ������ ��������� ��?", // does the user REALLY want to trash this item
};


STR16 pMapErrorString[] =
{
	L"����� �� ����� ��������� �� ������ �����.",

//1-5
	L"������ �������� ����� �� �����.",
	L"������ ���������? ��� �� ������ �����!",
	L"�����.������ ���� �������� � ������ ��� ������,����� �����.",
	L"� ��� � ������� ��� ������ ���",	// you have no members, can't do anything
	L"�����.�� ����� ���������.",		// merc can't comply with your order
//6-10
	L"����� ���������,����� ������.���������� ��� ��������", // merc can't move unescorted .. for a male
	L"����� ���������, ����� ������.���������� �� ��������.", // for a female
	L"������� ��� �� ������ � �������!",
	L"�������,������� ����� ������� ��� �������� � ����������.",
	L"",
//11-15
	L"������ ���������? ��� �� ����� ����!",
	L"�� ���������� �� ������ �����-������ � ������� %S!",
	L"�� ������ � ������ �����-����� � ������� I16!",
	L"",
	L"��� � %S ������ ������.",
//16-20
	L"����� � %S �����. ��� ���������� ����� ���� �� %S � ����.",
	L"��������� ���� ������ %S, �� �������� �������������.",
	L"��� ������� ������ �� ����� �����.������ ����.�� ��� �������.",
	L"%S ������ �������.� %S. ��� �����",
	L"%S ������ �������.� %S. ������� ������.",
//21-25
	L"����� � %S ��������� �������� ���������!",
	L"������ ��������� ������ ��� ��������� ��� � %S",
	L"������ ��������� ������ ��� ��������� %S",
	L"������ ��������� ������ ��� ���� �������� � %S.",
	L"������ ��������������� ��� ��������� %S.",
//26-30
	L"��� ������� ���� �� ����� ��������� ���������� ������� �����.",
	L"��� ������� ������ �� ����� ��������� ���������� ���������.",
	L"��������� �� ������, ���� �� ���������� ��� ��������.",
	L"%S ������ �� ����� ������� ������ ���������.",
	L"���������, ������� ��������� ��� ������,������ ����������� � ������ ������.",
//31-35
	L"������ ������� ��������� � %S.",
	L"������ ������ �� ����� ���������!",
	L"%S ������� �������, ����� ����!",
	L"������ ���� �������� �����!",
	L"%S �����!",
//36-40
	L"%S �� ����� ������� � %S: �� � ��������",
	L"%S �� ����� ����� � ������ ���",
	L"%S �� ����� �������. � %S",
	L"������ ������� ����� ���� ��� ���������!",
	L"��� ������ ����� ������ ������ �� �������!",
//41-45
	L"������ ������������� ���������� ���������",
	L"� ������ �������� ������!",
	L"%S ������� �����,����� �������������.",
	L"����� �� ������� � ������ �� ����� ��������� ��.",
	L"������ ����/����.�����.����� ������ �� ����� ���������.",
//46-50
	L"������ ����/����.������ �����.�� ����� ���������.",
	L"������ ������� ������!",
	L"����������� ��������� ����� ������ 2 �����.� �������",
	L"����� �� ����� ��������� ��� ������������.��������� �� � ���� �����.",
};


// help text used during strategic route plotting
STR16 pMapPlotStrings[] =
{
	L"�������� �� �����,����� ����������� �������� �����������,��� �������� �� ������� �������.",
	L"����������� ������������.",
	L"����� ����.�� ����������.",
	L"����������� ��������.",
	L"���� ��������.",
};


// help text used when moving the merc arrival sector
STR16 pBullseyeStrings[] =
{
	L"�������� �� ��� ������, ���� �� ������ ��������� ��������.",
	L"OK.����������� ������� ����� ������� � %S",
	L"�������� ������ ���� ������,�������.���� �����������!",
	L"������. ������ �������� ��� ��",
	L"����.������������ ��� %S �����������!������ �������� ��������� � %S.",
};


// help text for mouse regions

STR16 pMiscMapScreenMouseRegionHelpText[] =
{
	L"�������� ���������(|E|n|t|e|r)",
	L"�������� ����",
	L"����� �� ���������(|E|n|t|e|r)",
};



// male version of where equipment is left
STR16 pMercHeLeaveString[] =
{
	L"%S ������ �������� ���� ���������� ����� (%S) ��� ����� � �������� (B13)�� ����� ������ �� �������?",
	L"%S ������ �������� ���� ���������� ����� (%S) ��� ����� � ������ (�9) �� ����� ������ �� �������?",
	L"������������ � ������� ���� ���������� � ������ (A9).",
	L"������������ � ������� ���� ���������� � �������� (B13).",
	L"%S ������������ � ������� ���� ���������� � %S.",
};


// female version
STR16 pMercSheLeaveString[] =
{
	L"%S ������ �������� ���� ���������� ����� (%S) ��� ����� � �������� (B13)�� ����� ������ �� �������?",
	L" ������ �������� ���� ���������� ����� (%S) ��� ����� � ������ (�9)�� ����� ������ �� �������?",
	L"������������ � ������� ���� ���������� � ������ (A9).",
	L"������������ � ������� ���� ���������� � �������� (B13).",
	L"%S ������������ � ������� ���� ���������� � %S.",
};


STR16 pMercContractOverStrings[] =
{
	L":��� �������� ����������,�� ����� �����.", 		// merc's contract is over and has departed
	L":�� �������� ����������,��� ������ �����.", 		// merc's contract is over and has departed
	L":�������� ��������,�� �����.", 		// merc's contract has been terminated
	L":�������� �������� ��� ������.",		// merc's contract has been terminated
	L"�� ������ M.E.R.C. ������� �����,%S �����.", // Your M.E.R.C. account is invalid so merc left
};

// Text used on IMP Web Pages

STR16 pImpPopUpStrings[] =
{
	L"�������� ��� �����������",
	L"�� �������, ��� ������ ������ ������� ������ �������� ������?",
	L"������� ������ ��� � ���",
	L"���������.������ ����� �������� �������, ��� � ��� ������������ ����� �� ������.",
  L"������ �� �� ������ ������� ���.",
	L"����� ��������� ������,����� ����� ����� ��� ���� �� ��� ������ ����� �������.",
	L"������ ��� ��������.",
};


// button labels used on the IMP site

STR16 pImpButtonText[] =
{
	L"���������", 			// about the IMP site
	L"������", 			// begin profiling
	L"��������", 		// personality section
	L"��������", 		// personal stats/attributes section
	L"�������", 			// the personal portrait selection
	L"����� %d", 			// the voice selection
	L"������", 			// done profiling
	L"������", 		// start over profiling
	L"��,������� ���������� �����.",
	L"��",
	L"���",
	L"���������", 			// finished answering questions
	L"����", 			// previous question..abbreviated form
	L"����", 			// next question
	L"��.", 		// yes, I am certain
	L"���, ���� ������ �������.", // no, I want to start over the profiling process
	L"��.",
	L"���",
	L"�����", 			// back one page
	L"��������", 			// cancel selection
	L"��,������.",
	L"���,����������� ��� ���.",
	L"���������.", 			// the IMP site registry..when name and gender is selected
	L"������", 			// analyzing your profile results
	L"OK",
	L"�����",
};

STR16 pExtraIMPStrings[] =
{
	L"����� ������ ��������������, �������� ��������.",
	L"����� �������� ���������, �������� ���� ��������.",
	L"�������� ���������,���������� � ��������.",
	L"����� ��������� �������,�������� �����,������� ��� ��������."
};

STR16 pFilesTitle[] =
{
  L"�������� ������",
};

STR16 pFilesSenderList[] =
{
  L"����� ��������", 		// the recon report sent to the player. Recon is an abbreviation for reconissance
	L"�������� #1", 		// first intercept file .. Intercept is the title of the organization sending the file...similar in function to INTERPOL/CIA/KGB..refer to fist record in files.txt for the translated title
	L"�������� #2",	   // second intercept file
	L"�������� #3",			 // third intercept file
	L"�������� #4", // fourth intercept file
	L"�������� #5", // fifth intercept file
	L"�������� #6", // sixth intercept file
};

// Text having to do with the History Log

STR16 pHistoryTitle[] =
{
	L"�������",
};

STR16 pHistoryHeaders[] =
{
	L"����", 			// the day the history event occurred
	L"���.", 			// the current page in the history report we are in
	L"����", 			// the days the history report occurs over
	L"�����", 			// location (in sector) the event occurred
	L"�������", 			// the event label
};

// various history events
// THESE STRINGS ARE "HISTORY LOG" STRINGS AND THEIR LENGTH IS VERY LIMITED.
// PLEASE BE MINDFUL OF THE LENGTH OF THESE STRINGS. ONE WAY TO "TEST" THIS
// IS TO TURN "CHEAT MODE" ON AND USE CONTROL-R IN THE TACTICAL SCREEN, THEN
// GO INTO THE LAPTOP/HISTORY LOG AND CHECK OUT THE STRINGS. CONTROL-R INSERTS
// MANY (NOT ALL) OF THE STRINGS IN THE FOLLOWING LIST INTO THE GAME.
STR16 pHistoryStrings[] =
{
	L"",																						// leave this line blank
	//1-5
	L"%S ����� �� A.I.M.", 										// merc was hired from the aim site
	L"%S ����� �� M.E.R.C.", 									// merc was hired from the aim site
	L"%S ����.", 															// merc was killed
	L"���������.����� � M.E.R.C.",								// paid outstanding bills at MERC
	L"������� ���������� �� ������ ����������",
	//6-10
	L"IMP ������� ������������",
	L"�������� ��������� �������� ��� %S.", 				// insurance contract purchased
	L"������� ��������� �������� ��� %S.", 				// insurance contract canceled
	L"��������� ������� ��� %S.", 							// insurance claim payout for merc
	L"%S:�������� ������� �� ����.", 						// Extented "mercs name"'s for a day
	//11-15
	L"%S:�������� ������� �� 7��.", 					// Extented "mercs name"'s for a week
	L"%S:�������� ������� �� 14��.", 					// Extented "mercs name"'s 2 weeks
	L"%S ������.", 													// "merc's name" was dismissed.
	L"%S ����.", 																		// "merc's name" quit.
	L"������.", 															// a particular quest started
	//16-20
	L"��������.",
	L"�������� � �������.���� � 						 s",									// talked to head miner of town
	L"���������� %S",
	L"��� ����������� �����",
	L"���� ������ ���� � ������ �� ������",
	//21-25
	L"%S �������� ������� � ����� ����� �� ������ ����",
	L"%S:���� ��������� �����.",
	L"%S �����.",
	L"������ �������� �� ��������� ���������",
	L"����� ��������",
	//26-30
	L"%S:� ����� ��������� ����",
	L"%S: ����� ����������",
	L"%S: ����� �������",
	L"%S: ����� ����� �������",
	L"������� �������� � ������ �����.",
	//31-35
	L"������� � ��������� ������� ������ ����.",
	L"������ � ���� ����� � �������� ������.",
	L"��������� ����� ���������� ������.",
	L"����� ������� � ���� � ��� ����.",
	L"������� ������,��� �� ���-�� ����� � ������.",
	//36-40
	L"�������� ������-�� ������ - ������� �����������.",
	L"���������� �� ���������� ������!",
	L"�������� ���� - �� ���������� �������.",
	L"������� �������� ����� �� �������� ������.",
	L"����� ������������� �� ������� ������ �������� �����.",
	//41-45
	L"��� ���������� ������� ������.",
	L"������ ����� ������� ������ ������ ��� �����.",
	L"���� ������ �� ��������.",
	L"������ ������������ ��������� ��������.",
	L"�������� ����� - ������ �������� � �������.",
	//46-50
	L"�������� ������� - ���������� �� ������.",
	L"�������� ����� - � ���� ��������� ��������� ������.",
	L"�������� ���� �� ������� - � ���� ������������ �������.",
	L"����� ���������� ������������ � ������� ������.",
	L"� �������� ��������� ������� � �����.",
	//51-55
	L"����� ����������� ����������� � �����.",
	L"�������� ������������� �� �������� ����� �� �������.",
	L"�������� ����������� �� ����� �����.",
	L"��� ���� ����������� ����.",
	L"�������� ��������, ����� �� ������ ����� � ������.",
	//56-60
	L"���� � ������ ���� ������,�� ��� ��� ���� ���������.",
	L"��� ������ �����.",
	L"���� ������ ������ � ����� ��� ����.",
	L"%S ������� �������� ���",
	L"%S �������� �������� ���",
	//61-65
	L"%S ����������������� � �������� ���",
	L"����� ����� ����� � ����������� �����.",
	L"�������� ������, ������������ ������.",
	L"������� �������� ��� ��������",				//ENEMY_INVASION_CODE
	L"������� ������",
	//66-70
	L"�������� �����",							//ENEMY_ENCOUNTER_CODE
	L"������",						//ENEMY_AMBUSH_CODE
	L"������ ��������",
	L"����������� �����",			//ENTERING_ENEMY_SECTOR_CODE
	L"�������� �����!",
	//71-75
	L"�������� ���������",			//CREATURE_ATTACK_CODE
	L"���� ������-�������",			//BLOODCAT_AMBUSH_CODE
	L"������� �����-�����",
	L"%S ����",
	L"����� ������ ���������� ����",
	L"���� ����",
	L"���� %S",
};

STR16 pHistoryLocations[] =
{
	L"�/�",						// N/A is an acronym for Not Applicable
};

// icon text strings that appear on the laptop

STR16 pLaptopIcons[] =
{
	L"�����",
	L"����",
	L"�������",
	L"�����",
	L"������",
	L"�����",
	L"���������",
	L"���-��� 4.0",			// our play on the company name (Sirtech) and web surFER
};

// bookmarks for different websites
// IMPORTANT make sure you move down the Cancel string as bookmarks are being added

STR16 pBookMarkStrings[] =
{
	L"�.I.M.",
	L"����� ���",
	L"I.M.P.",
	L"�.�.R.�.",
	L"����",
	L"�����",
	L"���������",
	L"������",
};

STR16 pBookmarkTitle[] =
{
	L"��������",
	L"����� ������� � ��� ���� ����� - ������ ������.",
};

// When loading or download a web page

STR16 pDownloadString[] =
{
	L"��������",
	L"������������",
};

//This is the text used on the bank machines, here called ATMs for Automatic Teller Machine

STR16 gsAtmSideButtonText[] =
{
	L"OK",
	L"�����", 			// take money from merc
	L"����", 			// give money to merc
	L"������", 			// cancel transaction
	L"�����.", 			// clear amount being displayed on the screen
};

STR16 gsAtmStartButtonText[] =
{
	L"��������� $", 		// transfer money to merc -- short form
	L"����.", 			// view stats of the merc
	L"���������", 			// view the inventory of the merc
	L"���������",
};

STR16 sATMText[ ]=
{
	L"��������� ������?", 		// transfer funds to merc?
	L"Ok?", 			// are we certain?
	L"������� �����", 		// enter the amount you want to transfer to merc
	L"�������� ���", 		// select the type of transfer to merc
	L"������������ �����", 	// not enough money to transfer to merc
	L"����� ������ ���� ������� $10", // transfer amount must be a multiple of $10
};

// Web error messages. Please use German equivilant for these messages.
// DNS is the acronym for Domain Name Server
// URL is the acronym for Uniform Resource Locator

STR16 pErrorStrings[] =
{
	L"������",
	L"������ �� ����� DNS-�����.",
	L"��������� URL ����� � ���������� ��� ���.",
	L"OK",
	L"������ ����������.���������� �������.",
};


STR16 pPersonnelString[] =
{
	L"�����:", 			// mercs we have
};


STR16 pWebTitle[ ]=
{
	L"���-��� 4.0",		// our name for the version of the browser, play on company name
};


// The titles for the web program title bar, for each page loaded

STR16 pWebPagesTitles[] =
{
	L"�.I.M.",
	L"����� A.I.M.",
	L"���� A.I.M.",		// a mug shot is another name for a portrait
	L"A.I.M. ����������",
	L"A.I.M.",
	L"A.I.M.-�������", //$$
	L"A.I.M.-��������",
	L"A.I.M.-������",
	L"A.I.M.-������",
	L"M.E.R.C.",
	L"M.E.R.C.-�����",
	L"M.E.R.C.-�����������",
	L"M.E.R.C.-������",
	L"����� ���",
	L"����� ��� - ����.",
	L"����� ��� - ����.",
	L"����� ��� - �����",
	L"����� ��� - ������",							//misc is an abbreviation for miscellaneous
	L"����� ��� - �.�.",
	L"����� ��� - �����",
	L"I.M.P.",
	L"I.M.P.",
	L"�����.������ ������",
	L"�����.������ ������ - �������",
	L"�����.������ ������ - ����� ������",
	L"�����.������ ������ - ��������",
	L"������,����� � ������:��������� ������",
	L"����������",
	L"��������",
	L"�����������",
	L"���� ������������",
	L"",
	L"URL �� ������.",
	L"����� ��� - ��������� �������.",//@@@3 Translate new text
	L"",
	L"",
};

STR16 pShowBookmarkString[] =
{
	L"Sir-Help",
	L"��������:�������� ��� ��� �� Web.",
};

STR16 pLaptopTitles[] =
{
	L"�������� ����",
	L"�������� ������",
	L"��������",
	L"��������� ����",
	L"������",
};

STR16 pPersonnelDepartedStateStrings[] =
{
	//reasons why a merc has left.
	L"���� � ���",
	L"������",
	L"������",
	L"�����",
	L"�������� �������",
	L"�����",
};
// personnel strings appearing in the Personnel Manager on the laptop

STR16 pPersonelTeamStrings[] =
{
	L"������� �����",
	L"�����������",
	L"�������/����:",
	L"����.������:",
	L"����.������:",
	L"���� � ���:",
	L"������:",
	L"������:",
};


STR16 pPersonnelCurrentTeamStatsStrings[] =
{
	L"������",
	L"�������",
	L"�������",
};


STR16 pPersonnelTeamStatsStrings[] =
{
	L"����",
	L"���",
	L"���",
	L"���",
	L"���",
	L"���",
	L"���",
	L"���",
	L"���",
	L"����",
	L"���",
};


// horizontal and vertical indices on the map screen

STR16 pMapVertIndex[] =
{
	L"X",
	L"A",
	L"B",
	L"C",
	L"D",
	L"E",
	L"F",
	L"G",
	L"H",
	L"I",
	L"J",
	L"K",
	L"L",
	L"M",
	L"N",
	L"O",
	L"P",
};

STR16 pMapHortIndex[] =
{
	L"X",
	L"1",
	L"2",
	L"3",
	L"4",
	L"5",
	L"6",
	L"7",
	L"8",
	L"9",
	L"10",
	L"11",
	L"12",
	L"13",
	L"14",
	L"15",
	L"16",
};

STR16 pMapDepthIndex[] =
{
	L"",
	L"-1",
	L"-2",
	L"-3",
};

// text that appears on the contract button

STR16 pContractButtonString[] =
{
	L"��������",
};

// text that appears on the update panel buttons

STR16 pUpdatePanelButtons[] =
{
	L"����������",
	L"����",
};

// Text which appears when everyone on your team is incapacitated and incapable of battle

UINT16 LargeTacticalStr[][ LARGE_STRING_LENGTH ] =
{
	L"� ���� ������� ��� ������� ���������!",
	L"����, �� ��������� ��������� �������, ������ ���� �� �������!",
	L"���� ����� ������� �������� (�� ��� ��������)!",
	L"���� ����� ������� �������� � ���� ������.",
};


//Insurance Contract.c
//The text on the buttons at the bottom of the screen.

STR16			InsContractText[] =
{
	L"����.",
	L"����",
	L"��",
	L"��������",
};



//Insurance Info
// Text on the buttons on the bottom of the screen

STR16		InsInfoText[] =
{
	L"����.",
	L"����."
};



//For use at the M.E.R.C. web site. Text relating to the player's account with MERC

STR16			MercAccountText[] =
{
	// Text on the buttons on the bottom of the screen
	L"�����������",
	L"�� ��.��������",
	L"���� #:",
	L"����.",
	L"���",
	L"������",	//5
	L"���������",
	L"�����:",
	L"�� �������, ��� ������ ����������� ������� %S?",		//the %S is a string that contains the dollar amount ( ex. "$150" )
};



//For use at the M.E.R.C. web site. Text relating a MERC mercenary


STR16			MercInfo[] =
{
	L"��������",
	L"�����������",
	L"�����������",
	L"����",
	L"���������",
	L"��������",
	L"���������",
	L"��������",
	L"��������",
	L"������.���.",
	L"��������",

	L"����.",
	L"������",
	L"�����",
	L"������.������.",
	L"�� ��.��������",
	L"�����",
	L"��������:",
	L"� ����   .",
	L"�������",

	L"������,�� ��������� ������� ���������.��� ������-18 ���.",
	L"����������",
};



// For use at the M.E.R.C. web site. Text relating to opening an account with MERC

STR16			MercNoAccountText[] =
{
	//Text on the buttons at the bottom of the screen
	L"������� ����",
	L"������",
	L"� ��� ��� �����. ������ �������?"
};



// For use at the M.E.R.C. web site. MERC Homepage

STR16			MercHomePageText[] =
{
	//Description of various parts on the MERC page
	L"���� �.�����,����������",
	L"�������� �����",
	L"�������� �����",
	L"�������� ������",
	// The version number on the video conferencing system that pops up when Speck is talking
	L"���� ���.v3.2"
};

// For use at MiGillicutty's Web Page.

STR16			sFuneralString[] =
{
	L"���� ������������:������� ������ � ������� ������� � 1983.",
	L"�������� �� ��������� � ������ ������� �.I.� ������ \"����\" ������������-���������� �� ����� �������.",
	L"��� ����� ����� ������������ ������ � ������,������� �� ��� ����� ������ �� �������.",
	L"���� ��� ��������� ���������� ������� ������ ���������� �����,�� �������,� ������� ����� ���������,�� �������������� ������ ������������ ��������.",
	L"���������� ����� ��� ���������, � ���� ������������ ������ � ����.",

	// Text for the various links available at the bottom of the page
	L"������� �����",
	L"��������� ��� � ������",
	L"������ �� ��������",
	L"���������� �������",
	L"���������� ������",

	// The text that comes up when you click on any of the links ( except for send flowers ).
	L"����� ������� ������� ������.� ���������, �� ��� ������ ��� ���������.��������� ����� ��������� ��������� � ������ ������ ��������.",
	L"��������,��� �� �������� ���� ���������� � ��� �������� �����."
};

// Text for the florist Home page

STR16			sFloristText[] =
{
	//Text on the button on the bottom of the page

	L"�������",

	//Address of United Florist

	L"\"�� ���������� ����� �����\"",
	L"1-555-SCENT-ME",
	L"333 ��.����-���,���� ����,�� ��� 90210",
	L"http://www.scent-me.com",

	// detail of the florist page

	L"�� �������� ������ � ����������!",
	L"��������������� �������� � ������� ������ ��� � ����� ����� ������� ����.���� �����������.",
	L"����� ������ � ���� ����!",
	L"�������� ��� ������� �������� �����,������� ����� ������� � �������� 10��� ���������.",
	L"�������� �����,�����&����� � 1981.",
	L"���� ����������-������ ������� �������-������� ��� ����� � ������� 10���� �� ������� ��� �����.� ����� �����!������!",
	L"��������� ��� ��������� ���� ��������� �������� � �����.",
	L"����� ����,��������� �� ���� ���� �������,��������������� ������� ��� ����� ��������� ������ �� ����� ���������.",
	L"� �������-��,���� � ��� ���,�� ����� ���������-������!"
};



//Florist OrderForm

STR16			sOrderFormText[] =
{
	//Text on the buttons

	L"�����",
	L"�������",
	L"��������",
	L"�������",

	L"����.������:",
	L"����:",			//5
	L"����� ������:",
	L"���� ��������",
	L"����.����",
	L"������ ����� ������",
	L"����� ��������",			//10
	L"��������.������",
	L"��������� �����($10)",
	L"������ ����($20)",
	L"������� �����($10)",
	L"��������� �����(���� ����)($10)",		//15
	L"������ �����������:",
	L"����� ���������� ������� ��������, 75 �������� - ��������.",
	L"...��� ���������� �� ����",

	L"����������� �����",
	L"���������� � �����",//20

	//The text that goes beside the area where the user can enter their name

	L"���:",
};




//Florist Gallery.c

STR16		sFloristGalleryText[] =
{
	//text on the buttons

	L"����",	//abbreviation for previous
	L"����",	//abbreviation for next

	L"�������� �� ����,��� ������ ��������.",
	L"����������:�� ������ ������� ��� ��������� ����� ������.����� $10.",

	//text on the button

	L"�� ��.���.",
};

//Florist Cards

STR16			sFloristCards[] =
{
	L"�������� �� ����������",
	L"�����"
};



// Text for Bobby Ray's Mail Order Site

STR16			BobbyROrderFormText[] =
{
	L"����� ������",				//Title of the page
	L"��.",					// The number of items ordered
	L"��� (%S)",			// The weight of the item
	L"��������",				// The name of the item
	L"����",				// the item's weight
	L"�����",				//5	// The total price of all of items of the same type
	L"���������",				// The sub total of all the item totals added
	L"��� (��. ����� ��������)",		// S&H is an acronym for Shipping and Handling
	L"����� �����.",			// The grand total of all item totals + the shipping and handling
	L"����� ��������",
	L"����.��������",			//10	// See below
	L"�����.(�� %S.)",			// The cost to ship the items
	L"��������-1����",			// Gets deliverd the next day
	L"2 ������� ���",			// Gets delivered in 2 days
	L"����������� ����",			// Gets delivered in 3 days
	L"��������",//15			// Clears the order page
	L"������� �����",			// Accept the order
	L"�����",				// text on the button that returns to the previous page
	L"�� ��.���.",				// Text on the button that returns to the home page
	L"* ��������� ������",		// Disclaimer stating that the item is used
	L"� ��� ��� �� ��� �������.",		//20	// A popup message that to warn of not enough money
	L"<���>",				// Gets displayed when there is no valid city selected
	L"�� �������,��� ���� ������� ���� ����� %S?",		// A popup that asks if the city selected is the correct one
	L"��� ��������**",			// Displays the weight of the package
	L"** ���.���",				// Disclaimer states that there is a minimum weight for the package
	L"������",
};


// This text is used when on the various Bobby Ray Web site pages that sell items

STR16			BobbyRText[] =
{
	L"��������",				// Title
	// instructions on how to order
	L"�������� �� ����.���� ��� ����� ������ �����,�������� ���. ������ ����-�����. ���-�� �����.����� �������� ���,��� ������,���������� ����� ������.",

	//Text on the buttons to go the various links

	L"����.����",		//
	L"����.", 			//3
	L"��������",			//4
	L"�����",			//5
	L"����.",			//6	//misc is an abbreviation for miscellaneous
	L"�.�.",			//7
	L"���",
	L"�����",
	L"�� ��.���.",			//10

	//The following 2 lines are used on the Ammunition page.
	//They are used for help text to display how many items the player's merc has
	//that can use this type of ammo

	L"� ����� ������� ����",//11
	L"����.,��� ���.���� ��� �����������", //12

	//The following lines provide information on the items

	L"���:",		// Weight of all the items of the same type
	L"���:",			// the caliber of the gun
	L"���:",			// number of rounds of ammo the Magazine can hold
	L"���:",			// The range of the gun
	L"���:",			// Damage of the weapon
	L"��:",			// Weapon's Rate Of Fire, acronym ROF
	L"����:",			// Cost of the item
	L"�� ������:",			// The number of items still in the store's inventory
	L"�����:���-��:",		// The number of items on order
	L"�����������",			// If the item is damaged
	L"���:",			// the Weight of the item
	L"�����:",			// The total cost of all items on order
	L"* %% ���������",		// if the item is damaged, displays the percent function of the item

	//Popup that tells the player that they can only order 10 items at a time

	L"������� �������!����� � ������ on-line ��������� �������� �� ����� 10 �����. ���� �� ������ �������� ������,(� �� �������,��� ��� � ����),��������� ��� ���� ����� � ������� ���� ���������.",

	// A popup that tells the user that they are trying to order more items then the store has in stock

	L"��������.���� ����� ����������.���������� �������� ��� �����.",

	//A popup that tells the user that the store is temporarily sold out

	L"��������,�� ��� ������ ����� ���� �����������.",

};


// Text for Bobby Ray's Home Page

STR16			BobbyRaysFrontText[] =
{
	//Details on the web site

	L"����� �� ������ ���������� ��������� ������� ������������ ������ � ������������� �������",
	L"�� ����� ���������� ��� ���,��� ����� ��� �������� �����",
	L"�.�.",

	//Text for the various links to the sub pages

	L"������",
	L"���������",
	L"��������",		//5
	L"�����",

	//Details on the web site

	L"���� �� ����� �� �������, ��� ��� ����� ��������!",
	L"� ����������",
};



// Text for the AIM page.
// This is the text used when the user selects the way to sort the aim mercanaries on the AIM mug shot page

STR16			AimSortText[] =
{
	L"����� �.I.M.",				// Title
	// Title for the way to sort
	L"����������:",

	// sort by...

	L"����",
	L"���������",
	L"��������",
	L"��������",
	L"������.���.",
	L"��������",

	//Text of the links to other AIM pages

	L"����������� ���� ���������",
	L"����������� ���������� ���������",
	L"����������� ������� �.I.M.",

	// text to display how the entries will be sorted

	L"�� �������.",
	L"�� ����."
};


//Aim Policies.c
//The page in which the AIM policies and regulations are displayed

STR16		AimPolicyText[] =
{
	// The text on the buttons at the bottom of the page

	L"����.���.",
	L"��. ���.AIM",
	L"�������",
	L"����.���.",
	L"����������",
	L"����."
};



//Aim Member.c
//The page in which the players hires AIM mercenaries

// Instructions to the user to either start video conferencing with the merc, or to go the mug shot index

STR16			AimMemberText[] =
{
	L"����� ������",
	L"������� � �����.",
	L"������ ������",
	L"������ ����.",
};

//Aim Member.c
//The page in which the players hires AIM mercenaries

STR16			CharacterInfo[] =
{
	// The various attributes of the merc

	L"��������",
	L"�����������",
	L"�����������",
	L"����",
	L"���������",
	L"��������",
	L"���������",
	L"��������",
	L"��������",
	L"������.���.",
	L"��������",				//10

	// the contract expenses' area

	L"�����",
	L"����",
	L"1 ����",
	L"7 ����",
	L"14 ����",

	// text for the buttons that either go to the previous merc,
	// start talking to the merc, or go to the next merc

	L"����.",
	L"�������",
	L"����.",

	L"��������.���.",				// Title for the additional info for the merc's bio
	L"������.�����",		//20		// Title of the page
	L"�����.������������:",				// Displays the optional gear cost
	L"����������� ���.�������",			// If the merc required a medical deposit, this is displayed
};


//Aim Member.c
//The page in which the player's hires AIM mercenaries

//The following text is used with the video conference popup

STR16			VideoConfercingText[] =
{
	L"���� ���������:",				//Title beside the cost of hiring the merc

	//Text on the buttons to select the length of time the merc can be hired

	L"1 ����",
	L"7 ����",
	L"14 ����",

	//Text on the buttons to determine if you want the merc to come with the equipment

	L"��� ����������",
	L"������ ������.",

	// Text on the Buttons

	L"��������� �����",			// to actually hire the merc
	L"������",				// go back to the previous menu
	L"������",				// go to menu in which you can hire the merc
	L"�����.��������",				// stops talking with the merc
	L"OK",
	L"�������� �����.",			// if the merc is not there, you can leave a message

	//Text on the top of the video conference popup

	L"���������������� �",
	L"����������. . .",

	L"� ���.�����."			// Displays if you are hiring the merc with the medical deposit
};



//Aim Member.c
//The page in which the player hires AIM mercenaries

// The text that pops up when you select the TRANSFER FUNDS button

STR16			AimPopUpText[] =
{
	L"������� ������ �������� �������",	// You hired the merc
	L"������ ��������� �����",		// Player doesn't have enough money, message 1
	L"������������ �������",				// Player doesn't have enough money, message 2

	// if the merc is not available, one of the following is displayed over the merc's face

	L"�� �������",
	L"�������� ���������",
	L"���������",

	//If you try to hire more mercs than game can support

	L"� ��� ��� ������� 18 ���������-������ �������.",

	L"���������",
	L"�����. ���������",
};


//AIM Link.c

STR16			AimLinkText[] =
{
	L"����� A.I.M.",	//The title of the AIM links page
};



//Aim History

// This page displays the history of AIM

STR16			AimHistoryText[] =
{
	L"������ A.I.M.",					//Title

	// Text on the buttons at the bottom of the page

	L"����.���.",
	L"��.���.",
	L"������� A.I.M.", //$$
	L"����.���."
};


//Aim Mug Shot Index

//The page in which all the AIM members' portraits are displayed in the order selected by the AIM sort page.

STR16			AimFiText[] =
{
	// displays the way in which the mercs were sorted

	L"����",
	L"���������",
	L"��������",
	L"��������",
	L"������.���.",
	L"��������",

	// The title of the page, the above text gets added at the end of this text

	L"����� A.I.M.:���������� �� �������. %S",
	L"����� A.I.M.:���������� �� ����. %S",

	// Instructions to the players on what to do

	L"����� ������",
	L"������� �����",			//10
	L"������ ������",
	L"����������� �����",

	// Gets displayed on top of the merc's portrait if they are...

	L"�����������",
	L"���������",						//14
	L"�� �������",
};



//AimArchives.
// The page that displays information about the older AIM alumni merc... mercs who are no longer with AIM

STR16			AimAlumniText[] =
{
	// Text of the buttons

	L"��� 1",
	L"��� 2",
	L"��� 3",

	L"������� A.I.M.",	// Title of the page //$$

	L"��"			// Stops displaying information on selected merc
};






//AIM Home Page

STR16			AimScreenText[] =
{
	// AIM disclaimers

	L"A.I.M. � ������� A.I.M.-������������������ �� ������ ������� �������� �����.",
	L"������� ���� � �� ������� ��� ����������.",
	L"�������� 1998-1999 A.I.M.,Ltd.��� ����� ��������.",

	//Text for an advertisement that gets displayed on the AIM page

	L"������������ ��������� ������",
	L"\"�� ���������� � ����� �����\"",				//10
	L"������ ��� ����",
	L"... ������ ���",
	L"���� � ��� ��� ������ ���������,�� ��� � �� �����.",
};


//Aim Home Page

STR16			AimBottomMenuText[] =
{
	//Text for the links at the bottom of all AIM pages
	L"��.���.",
	L"�����",
	L"�������", //$$
	L"��������",
	L"������",
	L"������"
};



//ShopKeeper Interface
// The shopkeeper interface is displayed when the merc wants to interact with
// the various store clerks scattered through out the game.

STR16 SKI_Text[ ] =
{
	L"��������� ������",		//Header for the merchandise available
	L"���",				//The current store inventory page being displayed
	L"����� �����",				//The total cost of the the items in the Dealer inventory area
	L"����� ����",			//The total value of items player wishes to sell
	L"������",				//Button text for dealer to evaluate items the player wants to sell
	L"��������",			//Button text which completes the deal. Makes the transaction.
	L"������",				//Text for the button which will leave the shopkeeper interface.
	L"�����.�������",			//The amount the dealer will charge to repair the merc's goods
	L"1 ���",			// SINGULAR! The text underneath the inventory slot when an item is given to the dealer to be repaired
	L"%d �����",		// PLURAL!   The text underneath the inventory slot when an item is given to the dealer to be repaired
	L"���������������",		// Text appearing over an item that has just been repaired by a NPC repairman dealer
	L"��� ��� ������ ������ ����.",	//Message box that tells the user there is no more room to put there stuff
	L"%d �����",		// The text underneath the inventory slot when an item is given to the dealer to be repaired
	L"������� ���� �� �����.",
};

//ShopKeeper Interface
//for the bank machine panels. Referenced here is the acronym ATM, which means Automatic Teller Machine

STR16	SkiAtmText[] =
{
	//Text on buttons on the banking machine, displayed at the bottom of the page
	L"0",
	L"1",
	L"2",
	L"3",
	L"4",
	L"5",
	L"6",
	L"7",
	L"8",
	L"9",
	L"OK",						// Transfer the money
	L"�����",					// Take money from the player
	L"����",					// Give money to the player
	L"������",					// Cancel the transfer
	L"��������",					// Clear the money display
};


//Shopkeeper Interface
STR16	gzSkiAtmText[] =
{

	// Text on the bank machine panel that....
	L"Select Type",			// tells the user to select either to give or take from the merc
	L"������� �����",			// Enter the amount to transfer
	L"��������� ������ �����",		// Giving money to the merc
	L"������� ������ � �����",		// Taking money from the merc
	L"������������ �������",			// Not enough money to transfer
	L"������",				// Display the amount of money the player currently has
};


STR16	SkiMessageBoxText[] =
{
	L"�� ������ ����� %S �� ������ ��������� �����,����� ������� �������?",
	L"������������ �����.�� ������� %S",
	L"�� ������ ����� %S �� ������ ��������� �����,����� ������� ���������?",
	L"��������� �������� ������ �������",
	L"��������� �������� �������� ����.����",
	L"��������� ��������",
	L"������� ������",
};


//OptionScreen.c

STR16	zOptionsText[] =
{
	//button Text
	L"���������",
	L"���������",
	L"�����",
	L"������",

	//Text above the slider bars
	L"�������",
	L"����",
	L"������",

	//Confirmation pop when the user selects..
	L"����� �� ���� � ��������� � ������� ����?",

	L"����� ������� ���� ����, ���� ��������.",
};


//SaveLoadScreen
STR16			zSaveLoadText[] =
{
	L"���������",
	L"���������",
	L"������",
	L"����.����.",
	L"����.����.",

	L"���� ���������",
	L"������ ��� ����������!",
	L"���� ���������",
	L"������ ��� ��������!",

	L"����������� ������ ���� ���������� �� �������.�������� ����� ����������.����������?",
	L"����� ����������� ���� ����� ���� � �������.���������� �� ���?",

	//Translators, the next two strings are for the same thing.  The first one is for beta version releases and the second one
	//is used for the final version.  Please don't modify the "#ifdef JA2BETAVERSION" or the "#else" or the "#endif" as they are
	//used by the compiler and will cause program errors if modified/removed.  It's okay to translate the strings though.
#ifdef JA2BETAVERSION
	L"����.������ ���� ��������.�������� � ���������.����������?",
#else
	L"������� �������� ������ ������. �������� ������������� � ���������?",
#endif

	//Translators, the next two strings are for the same thing.  The first one is for beta version releases and the second one
	//is used for the final version.  Please don't modify the "#ifdef JA2BETAVERSION" or the "#else" or the "#endif" as they are
	//used by the compiler and will cause program errors if modified/removed.  It's okay to translate the strings though.
#ifdef JA2BETAVERSION
	L"����.������ � ������ ���� ���� ��������. �������� � ���������. ����������?",
#else
	L"������� �������� ������ ������. �������� ������������� � ���������?",
#endif

	L"�� �������,��� ������ �������� ���.���� ������ #%d?",
	L"�� ������ ��������� ���� �� ������ #",


	//The first %d is a number that contains the amount of free space on the users hard drive,
	//the second is the recommended amount of free space.
	L"� ��� ��������� �������� ������������. �������� ����� %d M����. ����� ��� ������� %d ��������� M����.",

	L"����������...",			//When saving a game, a message box with this string appears on the screen

	L"������� ����.",
	L"����� ����.",
	L"�������.�����",
	L"�������. �����",

	L"�����.",
};



//MapScreen
STR16		zMarksMapScreenText[] =
{
	L"������� �����",
	L"� ��� ��� ���������.���� ����������� �������,� � ��� ����� ��������� ���������.",
	L"����� � ����",
	L"� �����.���� ���������",
	L"%S �� �����.",
	L"%S �������� � ����� �� �����",
	L"%S ������� �����,���������� �����.",
	L"%S �� �����.",
	L"����� �� ����� ���������,����� ���� �����.����.",

	// stuff for contracts
	L"�� ������ ������� �� ���������,�� � ��� ��� ����� �� ��������� ������ ����� �����.",
	L"%S:��������� ������ �������� %S �� %d ������.����.������ �������?",
	L"��������� �������",
	L"� �����.���� ���.�������.",

	// other items
	L"������", // people acting a field medics and bandaging wounded mercs
	L"��������", // people who are being bandaged by a medic
	L"������", // Continue on with the game after autobandage is complete
	L"����", // Stop autobandaging of patients by medics now
	L"��������.��� ����� ����������,�.�.��� ����-������.", // informs player this option/button has been disabled in the demo
	L"%S:��� ��������� �������.",
	L"%S:��� ����������� �������.",
	L"������������ �����,�������� ������ ����������.",
	L"%S:����� ����������.",
	L"� �����.������.��������.",
  L"�������� �����.�� �����������",
};


STR16 pLandMarkInSectorString[] =
{
	L"����� %d ������� ����-�� � ������� %S",
};

// confirm the player wants to pay X dollars to build a militia force in town
STR16 pMilitiaConfirmStrings[] =
{
	L"���������� ������ �����.�����.����� ������ $", // telling player how much it will cost
	L"�������� ������?", // asking player if they wish to pay the amount requested
	L"� ��� ��� ����� �� ���.", // telling the player they can't afford to train this town
	L"���������� ���������� ��������� � %S (%S %d)?", // continue training this town?
	L"����� $", // the cost in dollars to train militia
	L"( �/� )",   // abbreviated yes/no
	L"",	// unused
	L"���������� ������ �����.�����.� ������� %d ����� ������ $ %d. %S", // cost to train sveral sectors at once
	L"� ��� ��� $%d �� ���������� �����.��������� �����.",
	L"%S:����� %d ��������� �������� ����,����� ���������� ���������� ���������.",
	L"� %S ������ ������ ����������� ���������.",
};

#ifdef JA2DEMOADS
STR16 gpDemoString[] =
{
	//0-9
	L"������ ���������",
	L"������ ���.",
	L"����������� ����",
	L"��������� ����� �������",
	L"����������������� ���",
	L"������������ ���������",
	L"�������������� � NPC",
	L"������ � ������ ���",
	L"������ ������������ ��������",
	L"������������� � ��������� �����",
	L"����������� ������",
	//10-19
	L"������ 150 ����������",
	L"����� ������ �����",
	L"������ 250 ����.����",
	L"������������ ������",
	L"����� ������.�������",
	L"����� ��������",
	L"����� 9000 ����� ��������",
	L"����������� ������ ������� ������",
	L"���������� ���������",
	L"��������� ����������� ������� ������",
	//20-29
	L"���������� ������.���������",
	L"������ ��������",
	L"������������ �������������� � �������",
	L"��������� ������������ �������",
	L"�������,�������,��������",
	L"������������ ����� �����������",
	L"�������� ������� ���",
	L"�������� ����������",
	L"24-������� �����",
	L"����� 60 ����� ����",
	//30-34
	L"��������� ������� ���������",
	L"������������ ������������� ����������",
	L"���������� ���������� ������",
	L"������ ������������� �����������",
	L"� ����� ����...",
	//35 on are BOLD
	L"",
	L"",
	L"\"���� �� �����....��� 1999 ����\"",
	L"PC Gamer",
	L"�������� ���� � ���� �������! buka@dol.ru",
};

STR16 gpDemoIntroString[] =
{
	L"������ �� �������,��� ����� ��������� ���������,���� � �����:",
	//Point 1 (uses one string)
	L"�� ����� ������������ ������� ��������� � ����������(������� ����� � ���������).",
	//Point 2 (uses one string)
	L"����������, ����� ������� ������ ��������,�� ���� �� ��������,������,����������,��� ������ � ����� � ���...",
	//Point 3 (uses one string)
	L"���� � �����!������ �������,������ ��������,������.",
	//Additional comment
	L"(and this is just the beginning)",
	//Introduction/instructions
	L"����� ���������� � ���������(�������,����� ����������)",
};
#endif

//Strings used in the popup box when withdrawing, or depositing money from the $ sign at the bottom of the single merc panel
STR16	gzMoneyWithdrawMessageText[] =
{
	L"�� ���� ��� ����� ����� �� ����� $20,000.",
	L"�� �������, ��� ������ �������� %S �� ���� ����?",
};

STR16	gzCopyrightText[] =
{
	L"��������� �����(C) 1999 Sir-Tech Canada Ltd. ��� ����� ��������. ��������������� �� ���������� ����� ��� �������� ����",
};

//option Text
STR16		zOptionsToggleText[] =
{
	L"����",
	L"����� �������������", //$$
	L"��������",
	L"������� � �������",
	L"������������� ���",
	L"�������",
	L"�� ����� ��� ����!",
	L"������ ������ ������",
	L"���������� ��������",
	L"���������� �������",
	L"���� � �������� �������",
	L"�������� ��������� �����",
	L"������������ ������.�������",
	L"�������� �����.�� ����� ��������",
	L"��������� ������ �� �����.",
	L"��������� ������ �� �����",
	L"�������� �����",
	L"�������� �������� ��������",
	L"���������� �������",
	L"�������� ���������� ������",
};

//This is the help text associated with the above toggles.
STR16	zOptionsScreenHelpText[] =
{
	//speech
	L"���� �� ������ �������� ������ ����������,�������� ��� �����.",

	//Mute Confirmation
	L"���/���� ���������� �������������.",

		//Subtitles
	L" whether on-screen text is displayed for dialogue.", //$$

	//Key to advance speech
	L"���� ������� ��������,�������� ����� ���,����� ������ NPC ������.",

	//Toggle smoke animation
	L"��������� ��� �����,���� ������������� ��� ��������� ����.",

	//Blood n Gore
	L"��������� ��� �����,���� ������� �����.",

	//Never move my mouse
	L"��������� ��� �����,����� ������ ������������� ����������� �� ����������� ���� �������",

	//Old selection method
	L"�������� ��� �����,����� ��������� ��������,��� � ���������� ������ Jagged Alliance (����� ��� ����� �������� ��-�������).",

	//Show movement path
	L"�������� ��� �����,����� �������� ����������� � �������� �������(��� ����������� SHIFT ��� ����������� �����).",

	//show misses
	L"�������� ��� �����,����� ������,���� �������� ���� ��� \"�������\".",

	//Real Time Confirmation
	L"����� ����� ���,������.\"������������\" ������ ����� ��� �������� � �������� �����.",

	//Display the enemy indicator
	L"����� ����� ���,���������� ������,������ ��������,������������� ��� ��� ���������.",

	//Use the metric system
	L"����� ����� ���,���.������.�������,�����-����������.",

	//Merc Lighted movement
	L"����� ����� ���,���� �������� �����.���������� ������.��������� ��� ��������.",

	//Smart cursor
	L"����� ����� ���,������������ ������� �� �������� ����� ��� ��������.",

	//snap cursor to the door
	L"����� ����� ���,������������ ������� �� ����� ����� ������������� ����������� ��� ������ �����.",

	//glow items
	L"����� ����� ���,���� ��������( |I)",

	//toggle tree tops
	L"����� ����� ���,������������ �������� ��������.( |�)",

	//toggle wireframe
	L"����� ����� ���,����� ������� �����. ( |W)", //$$

	L"����� ����� ���,���������� ������-����������.( |Home )",

};


STR16	gzGIOScreenText[] =
{
	L"��������� ������ ����",
	L"����� ����",
	L"������������",
	L"������������",
	L"����� ����������",
	L"����� ����������",
	L"����������",
	L"������� ���������",
	L"������",
	L"����������",
	L"�������",
	L"Ok",
	L"������",
	L"�������������� ���������",
	L"��� ����������� �������",
	L"����� ���� ����������",
	L"��������� � ����-������",
};

STR16 pDeliveryLocationStrings[] =
{
	L"�����",			//Austin, Texas, USA
	L"������",			//Baghdad, Iraq (Suddam Hussein's home)
	L"�������",			//The main place in JA2 that you can receive items.  The other towns are dummy names...
	L"��� ����",		//Hong Kong, Hong Kong
	L"������",			//Beirut, Lebanon	(Middle East)
	L"������",			//London, England
	L"��� ��������",	//Los Angeles, California, USA (SW corner of USA)
	L"������",			//Meduna -- the other airport in JA2 that you can receive items.
	L"��������",		//The island of Metavira was the fictional location used by JA1
	L"������",				//Miami, Florida, USA (SE corner of USA)
	L"������",			//Moscow, USSR
	L"��� ����",		//New York, New York, USA
	L"������",			//Ottawa, Ontario, Canada -- where JA2 was made!
	L"�����",				//Paris, France
	L"�������",			//Tripoli, Libya (eastern Mediterranean)
	L"�����",				//Tokyo, Japan
	L"��������",		//Vancouver, British Columbia, Canada (west coast near US border)
};

STR16 pSkillAtZeroWarning[] =
{ //This string is used in the IMP character generation.  It is possible to select 0 ability
	//in a skill meaning you can't use it.  This text is confirmation to the player.
	L"������? ���� �������� ���������� �������."
};

STR16 pIMPBeginScreenStrings[] =
{
	L"( 8 ����. �������� )",
};

STR16 pIMPFinishButtonText[ 1 ]=
{
	L"������",
};

STR16 pIMPFinishStrings[ ]=
{
	L"�������,%S", //%S is the name of the merc
};

// the strings for imp voices screen
STR16 pIMPVoicesStrings[] =
{
	L"�����",
};

STR16 pDepartedMercPortraitStrings[ ]=
{
	L"���� � ���",
	L"������",
	L"������",
};

// title for program
STR16 pPersTitleText[] =
{
	L"�����",
};

// paused game strings
STR16 pPausedGameText[] =
{
	L"�����",
	L"����������� (|P|a|u|s|e)",
	L"��������� �� ����� (|P|a|u|s|e)",
};


STR16 pMessageStrings[] =
{
	L"����� �� ����?",
	L"OK",
	L"��",
	L"���",
	L"������",
	L"������",
	L"����",
	L"No description", //Save slots that don't have a description.
	L"���� ���������",
	L"���� ���������",
	L"QuickSave", //The name of the quicksave file (filename, text reference)
	L"SaveGame",	//The name of the normal savegame file, such as SaveGame01, SaveGame02, etc.
	L"sav",				//The 3 character dos extension (represents sav)
	L"..\\SavedGames", //The name of the directory where games are saved.
	L"����",
	L"�����",
	L"������ ������", //An empty save game slot
	L"����",				//Demo of JA2
	L"����� �����",				//State of development of a project (JA2) that is a debug build
	L"Release",			//Release build for JA2
	L"���",					//Abbreviation for Rounds per minute -- the potential # of bullets fired in a minute.
	L"���",					//Abbreviation for minute.
	L"�",						//One character abbreviation for meter (metric distance measurement unit).
	L"����",				//Abbreviation for rounds (# of bullets)
	L"��",					//Abbreviation for kilogram (metric weight measurement unit)
	L"�",					//Abbreviation for pounds (Imperial weight measurement unit)
	L"��.���",				//Home as in homepage on the internet.
	L"USD",					//Abbreviation to US dollars
	L"�/�",					//Lowercase acronym for not applicable.
	L"� ��� �����",		//Meanwhile
	L"%S ������(�) � ������ %S%S", //Name/Squad has arrived in sector A9.  Order must not change without notifying
																		//SirTech
	L"������",
	L"������ ������ �������� ����",
	L"��� ������-��� �������� ���������� ������� ���� (ALT+S).",
	L"�������",
	L"�������",
#ifdef JA2DEMO
	L"��� ���� ����������. ����� ����� ���������� � �����?",
	L"����� �����,��� �� ���� ���.",
#endif
	L"� ��� ��������� �������� ������������. � ��� �������� %S�� ���������,� ��� ������� 2 ��������� %SM�.",
	L"����� %S �� AIM",
	L"%S ������ %S.",		//'Merc name' has caught 'item' -- let SirTech know if name comes after item.
	L"%S ������ ���������.", //'Merc name' has taken the drug
	L"%S �� ����� ����������",//'Merc name' has no medical skill.

	//CDRom errors (such as ejecting CD while attempting to read the CD)
	L"�������� ����������� ���������.",
	L"������: ������ CD-ROM",

	//When firing heavier weapons in close quarters, you may not have enough room to do so.
	L"���� ����� ��� ��������.",

	//Can't change stance due to objects in the way...
	L"������ �������� ��������� ������.",

	//Simple text indications that appear in the game, when the merc can do one of these things.
	L"�������",
	L"�������",
	L"��������",

	L"%S �������� %S.", //"Item" passed to "merc".  Please try to keep the item %S before the merc %S, otherwise,
											 //must notify SirTech.
	L"������ �������� %S %S.", //pass "item" to "merc".  Same instructions as above.

	//A list of attachments appear after the items.  Ex:  Kevlar vest ( Ceramic Plate 'Attached )'
	L" ������������ )",

	//Cheat modes
	L"��������� ���-������� ����",
	L"��������� ���-������� ���",

	//Toggling various stealth modes
	L"����� �����.",
	L"����� �����.",
	L"%S �����.",
	L"%S ������.",

	//Wireframes are shown through buildings to reveal doors and windows that can't otherwise be seen in
	//an isometric engine.  You can toggle this mode freely in the game.
	L"�������������� ������� ���",//$$
	L"�������������� ������� ����",//$$

	//These are used in the cheat modes for changing levels in the game.  Going from a basement level to
	//an upper level, etc.
	L"������ ��������� � ����� ������...",
	L"���� ������� ���...",
	L"������ � ���������� ������� %d...",
	L"������ �� �������...",

	#ifdef JA2DEMO

	//For the demo, the sector exit interface, you'll be able to split your teams up, but the demo
	//has this feature disabled.  This string is fast help text that appears over "single" button.
	L"� ������ ���� �� ������� ���������\n�������, �� �� � ���� ������.",

	//The overhead map is a map of the entire sector, which you can go into anytime, except in the demo.
	L"����� ����� � ����-������ �����������.",

	#endif

	L".",		// used in the shop keeper inteface to mark the ownership of the item eg Red's gun
	L"���������.",
	L"��������.",
	L"3D-������ ����.",
	L"3D-������ ���.",
	L"����� %d ���������.",
	L"� ��� ��� �����,����� ��������� ����������� %S %S",	//first %S is the mercs name, the seconds is a string containing the salary
	L"�������",
	L"%S �� ����� ���� ����.",
	L"���� ���� ��������� ��� ������ SaveGame99.sav. ��� ������������� ������������� �� ��� ������ SaveGame01-SaveGame10 � ����� �� ������ �������� ������ � ��� � ������ ��������.",
	L"%S ����� ������� %S",
	L"����� ������ � �������.",
	L"%S ������ ������� � ��������� ����� ������� (������ %S) � ���� %d,�������� � %S.",		//first %S is mercs name, next is the sector location and name where they will be arriving in, lastely is the day an the time of arrival
	L"������� ���������.",
#ifdef JA2BETAVERSION
	L"���� ��������� � ������ ����-����������.",
#endif
};


UINT16 ItemPickupHelpPopup[][40] =
{
	L"OK",
	L"������� �����",
	L"�������� ���",
	L"������� ����",
	L"������"
};

STR16 pDoctorWarningString[] =
{
	L"%S ������� ������,����� ��� ����� ���� ������.",
	L"������ �� ����� ���������� ����.",
};

STR16 pMilitiaButtonsHelpText[] =
{
	L"����� ��������(������ ������)/����������(����� ������)", // button help text informing player they can pick up or drop militia with this button
	L"����� ���������� ������(������ ������)/����������(����� ������)",
	L"����� ������ ���������(������ ������)/����������(����� ������)",
	L"���������� ������������ ��������� ��������� �� ���� ��������",
};

STR16 pMapScreenJustStartedHelpText[] =
{
	L"��� � �IM � ������ ��������� (*���������* � �������)", // to inform the player to hired some mercs to get things going
	L"���� �� ������ ����������� � �������,�������� �� ������ ���������� ������� � ������ ������ ���� ������.", // to inform the player to hit time compression to get the game underway
};

STR16 pAntiHackerString[] =
{
	L"������.����������� ��� ������������� �����.�� �������� �� ����.",
};


STR16 gzLaptopHelpText[] =
{
	//Buttons:
	L"����������� �����",
	L"���������� web ��������",
	L"����������� ����� � ����������.",
	L"��������� ��������� �������",
	L"���������� � �������",
	L"����������� ���������� ���������� � ������",
	L"������� ������",

	//Bottom task bar icons (if they exist):
	L"����� ���������",
	L"����� �����",

	//Bookmarks:
	L"������������� ���������� ���������",
	L"����� ���-����� ������ � ����",
	L"�������� ���������� ���������",
	L"������������� �����",
	L"���� ��� ���������",
	L"������������ ��������� ������",
	L"��������� ������ �� ���������� A.I.M.",
};


STR16	gzHelpScreenText[] =
{
	L"����� �� ������ ������",
};

STR16 gzNonPersistantPBIText[] =
{
	L"���� ���. �� ������ ������ �������� ����� �����.",
	L"����� � ������, ����� ���������� ���( |E).",
	L"������������� ���������� ������� ����� ( |A).",
	L"������ ������������� ���������� �����, ����� �� ���������.",
	L"������ ������������� ���������� �����,����� �� ���� ������.",
	L"������ ������������� ���������� �����,����� �������� � ���������� � ������.",
	L"������ ������������� ���������� �����,���� ���������� ���������� ������.",
	L"������ ������������� ���������� �����,���� ���������� �����-������.",
	L"���� ���",
	L"������ ��������� ������.",
};

STR16 gzMiscString[] =
{
	L"���� ��������� ������� ��� ������ ���������...",
	L"������ ���� �� ����� ������������.",
	L"�������� ����� �� %d%%.",
	L"����� ��������� ��������� ������������ ���������� %S.",
	L"�� �������� ��������.",
};

STR16	gzIntroScreen[] =
{
	L"���������� ����� ������������� �����",
};

// These strings are combined with a merc name, a volume string (from pNoiseVolStr),
// and a direction (either "above", "below", or a string from pDirectionStr) to
// report a noise.
// e.g. "Sidney hears a loud sound of MOVEMENT coming from the SOUTH."
STR16 pNewNoiseStr[] =
{
	L"%S ������ %S ����, ������ � %S�.",
	L"%S ������ %S ���� ��������, ������ � %S�.",
	L"%S ������ %S �����, ������ � %S�.",
	L"%S ������ %S �����, ������ � %S�.",
	L"%S ������ %S ����, ������ � %S�.", //$$
	L"%S ������ %S ����� �� %S�.",
	L"%S ������ %S ���� � %S�.",
	L"%S ������ %S ���� � %S�.",
	L"%S ������ %S ���� � %S�.",
	L"%S ������ %S ����, ������ � %S�.",
	L"%S ������ %S ������, ������  %S�.",
};

STR16 wMapScreenSortButtonHelpText[] =
{
	L"���������� �� ����� (|F|1)",
	L"���������� �� ����. (|F|2)",
	L"���������� �� ��� (|F|3)",
	L"���������� �� ����� (|F|4)",
	L"���������� �� ����� ����.(|F|5)",
	L"���������� �� ������� ��������� (|F|6)",
};



STR16		BrokenLinkText[] =
{
	L"������ 404",
	L"URL �� ������.",
};


STR16 gzBobbyRShipmentText[] =
{
	L"����.�����������",
	L"����� #",
	L"����������",
	L"��������",
};


STR16	gzCreditNames[]=
{
	L"Chris Camfield",
	L"Shaun Lyng",
	L"Kris Mornes",
	L"Ian Currie",
	L"Linda Currie",
	L"Eric \"WTF\" Cheng",
	L"Lynn Holowka",
	L"Norman \"NRG\" Olsen",
	L"George Brooks",
	L"Andrew Stacey",
	L"Scot Loving",
	L"Andrew \"Big Cheese\" Emmons",
	L"Dave \"The Feral\" French",
	L"Alex Meduna",
	L"Joey \"Joeker\" Whelan",
};


STR16	gzCreditNameTitle[]=
{
	L"������� ����������� ����", 			// Chris Camfield !!!
	L"������/��������",							// Shaun Lyng
	L"����������� �������������� ����� � ���������",					//Kris Marnes
	L"��������/������",						// Ian Currie
	L"������/������ ����",				// Linda Currie
	L"��������",													// Eric \"WTF\" Cheng
	L"����-�����������, ���.���������",				// Lynn Holowka
	L"������� ��������",						// Norman \"NRG\" Olsen
	L"������ �� �����",											// George Brooks
	L"������ ������/��������",					// Andrew Stacey
	L"������� ��������/��������",						// Scot Loving
	L"������� �����������",									// Andrew \"Big Cheese Doddle\" Emmons
	L"�����������",											// Dave French
	L"����������� �������������� ����� � ������� ����",					// Alex Meduna
	L"�������� �� ���������",								// Joey \"Joeker\" Whelan",
};

STR16	gzCreditNameFunny[]=
{
	L"", 																			// Chris Camfield
	L"(��� ��� ������ ������� ����������)",					// Shaun Lyng
	L"(\"������. � ������ ����\")",	//Kris \"The Cow Rape Man\" Marnes
	L"(�� ��� ������� ���� ��� �����)",				// Ian Currie
	L"(�������� ��� Wizardry 8)",						// Linda Currie
	L"(���������� �� ����� ���������)",			// Eric \"WTF\" Cheng
	L"(���� �� ��� � CFSA - ��������� �������...)",	// Lynn Holowka
	L"",																			// Norman \"NRG\" Olsen
	L"",																			// George Brooks
	L"(������� ������ � �������� �����)",						// Andrew Stacey
	L"(��� ��������� ��� ������)",							// Scot Loving
	L"(������������ ������������� ����)",					// Andrew \"Big Cheese Doddle\" Emmons
	L"(����� ����� �������� �����������)",	// Dave French
	L"(������� � ������ ��� Wizardry 8)",							// Alex Meduna
	L"(������ �������� � ����������� ������!)",	// Joey \"Joeker\" Whelan",
};

STR16 sRepairsDoneString[] =
{
	L"%S �������� ������ ����� �����",
	L"%S �������� ������������� ��� ������ � �����",
	L"%S �������� ������������� ��� ����������",
	L"%S �������� ������������� ��� ���������������� ����",
};

STR16 zGioDifConfirmText[]=
{
	L"�� ������� ������ �����. ��� �������� ��� �������� � Jagged Alliance '������ ������', ��� �������� � ����� ���������, ��� ��� ���, ��� ������ ��������� ����� � ����. ��� ����� �������� �� ���� � �����, ��� ��� ��������� � ����. �� �������, ��� ������ ������ � ������ ������?",
	L"�� ������� ���������� �����. ��� �������� ��� ���� ���, ��� ��� ������ � Jagged Alliance '������ ������' ��� � ��������� ������. ��� ����� �������� �� ���� � �����, ��� ��� ��������� � ����. �� �������, ��� ������ ������ � ���������� ������?",
	L"�� ������� ������� �����. �� ��� �������������. ������ �� ��� ������, ���� ��� �������� ����� � �������� �����. ��� ����� �������� �� ���� � �����, ��� ��� ��������� � ����. �� �������, ��� ������ ������ � ������� ������?",
};

STR16 gzLateLocalizedString[] =
{
	L"%S ���� ��� �������� ������ �� ������...",

	//1-5
	L"����� �� ����� �������� ������,�.�.������ ��������� ��.",

	//This message comes up if you have pending bombs waiting to explode in tactical.
	L"������ ������� ����� ������.��������� ����������!",

	//'Name' refuses to move.
	L"%S ������������ ���������.",

	//%S a merc name
	L"%S:������������ �������,����� �������� ���������.",

	//A message that pops up when a vehicle runs out of gas.
	L"%S:��������� ������� � �� �������� � %c%d.",

	//6-10

	// the following two strings are combined with the pNewNoise[] strings above to report noises
	// heard above or below the merc
	L"���",
	L"���",

	//The following strings are used in autoresolve for autobandaging related feedback.
	L"�� � ���� �� ����� ��������� ��� ����������.",
	L"��� ��������� ��� ���������.",
	L"�� ������� ����������,����� ���������� ����.",
	L"������������ ����� ��������� �� �����.",
	L"������������ ��������� �������������.",
	L"��� �������� ����������.",

	//14
	L"�������",

  L"(roof)",

	L"��������: %d/%d",

	//In autoresolve if there were 5 mercs fighting 8 enemies the text would be "5 vs. 8"
	//"vs." is the abbreviation of versus.
	L"%d ������ %d",

	L"%S �����!",  //(ex "The ice cream truck is full")

  L"%S ��������� �� � ��������� � ������ ������, � � ��������� ����������� ������������ �/��� ������.",

	//20
	//Happens when you get shot in the legs, and you fall down.
	L"%S ����� � ���� � ��� ��������!",
	//Name can't speak right now.
	L"%S ������ �������� �� �����.",

	//22-24 plural versions @@@2 elite to veteran
	L"%d ������� ����� ����������.",
	L"%d ������� ����� ��������� ����������.",
	L"%d ���������� ��������� ����� ����������.",

	//25
	L"������.",

	//26
	//Name has gone psycho -- when the game forces the player into burstmode (certain unstable characters)
	L"%S �������� ����!",

	//27-28
	//Messages why a player can't time compress.
	L"������ ������ ������� �����, ��������� � ��� ���� �������� � ������� %S.", //
	L"������ ������� �����, ����� �������� ��������� � ������ � ����������.", //

	//29-31 singular versions @@@2 elite to veteran
	L"1 ������� ���� ����������.",
	L"1 ������� ���� ���������� ����������.",
	L"1 ���������� ��������� ����� �����������.",

	//32-34
	L"%S ������ �� �������.",
	L"���������� �� �����������?",
	L"(����� %d)",

	//35
	//Ex: "Red has repaired Scope's MP5K".  Careful to maintain the proper order (Red before Scope, Scope before MP5K)
	L"%S ������� %S %S",

	//36
	L"�����-������",

	//37-38 "Name trips and falls"
	L"%S ������",
	L"��� ���� ������ ����� ������.",

	//39
	L"����� �� ���������� �����.�� ����� �������.��������� �������� � ���������� ����.",

	//40-43
	//%S is the name of merc.
	L"%S:����������� ���������!",
	L"%S �� �������� ��������,����� ������ ����-����!",
	L"%S:��������� �����������!",
	L"%S �� �������� ��������,����� ������������� ���-����!",

	//44-45
	L"����� �������",
	L"%S �� ����� ������� ����� ��������.",

	//46-48
	L"%S'. ������� ��� ��������� ������!",
	L"�� ����������� ������ %d �������� ��������� �� ������.",
  L"������?",

	//49-50
	L"���������� �������",
	L"��� ������ ���������.",

	//51-52 Fast help text in mapscreen.
	L"���������� ���������� ������� (|S|p|a|c|e)",
	L"���������� ���������� ������� (|E|s|c)",

	//53-54 "Magic has unjammed the Glock 18" or "Magic has unjammed Raven's H&K G11"
	L"%S ��������(�) %S",
	L"%S ��������(�) %S (%S)",

	//55
	L"���������� ������� ����� ��� ��������� ����������� �������.",

	L"CD ������ ������ �� ������. ��������� ������� � ��.",

	L"�������� ������� ���������.",

	//58
	//Displayed with the version information when cheats are enabled.
	L"�������/������������: %d%%/%d%%",

	//59
	L"����������� ����� � ����?",

  L"����������� �����.",
};




#endif //RUSSIAN
