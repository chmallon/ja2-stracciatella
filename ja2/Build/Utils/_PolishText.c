#include "Language_Defines.h"


#ifdef POLISH

#include "Text.h"

/*

******************************************************************************************************
**                                  IMPORTANT TRANSLATION NOTES                                     **
******************************************************************************************************

GENERAL INSTRUCTIONS
- Always be aware that foreign strings should be of equal or shorter length than the English equivalent.
	I know that this is difficult to do on many occasions due to the nature of foreign languages when
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
	used in conjunction with other characters.  For example, %ls means string, and is commonly used for names,
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
	symbols.  SirTech will search for !!! to look for your problems and questions.  This is a more
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

	Other examples are used multiple times, like the Esc key  or "|E|s|c" or Space -> (|S|p|a|c|j|a)

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

	As described above, the "!!!" notation should be used by you to ask questions and address problems as
	SirTech uses the "@@@" notation.

*/

wchar_t ItemNames[MAXITEMS][80] =
{
	L"",
};


wchar_t ShortItemNames[MAXITEMS][80] =
{
	L"",
};

// Different weapon calibres
// CAWS is Close Assault Weapon System and should probably be left as it is
// NATO is the North Atlantic Treaty Organization
// WP is Warsaw Pact
// cal is an abbreviation for calibre
const wchar_t AmmoCaliber[][20] =
{
	L"0",
	L".38 cal",
	L"9mm",
	L".45 cal",
	L".357 cal",
	L"12 gauge",
	L"CAWS",
	L"5.45mm",
	L"5.56mm",
	L"7.62mm NATO",
	L"7.62mm WP",
	L"4.7mm",
	L"5.7mm",
	L"Monstrum",
	L"Rakiety",
	L"", // dart
	L"", // flame
};

// This BobbyRayAmmoCaliber is virtually the same as AmmoCaliber however the bobby version doesnt have as much room for the words.
//
// Different weapon calibres
// CAWS is Close Assault Weapon System and should probably be left as it is
// NATO is the North Atlantic Treaty Organization
// WP is Warsaw Pact
// cal is an abbreviation for calibre
const wchar_t BobbyRayAmmoCaliber[][20] =
{
	L"0",
	L".38 cal",
	L"9mm",
	L".45 cal",
	L".357 cal",
	L"12 gauge",
	L"CAWS",
	L"5.45mm",
	L"5.56mm",
	L"7.62mm N.",
	L"7.62mm WP",
	L"4.7mm",
	L"5.7mm",
	L"Monstrum",
	L"Rakiety",
	L"", // dart
};


const wchar_t WeaponType[][30] =
{
	L"Inny",
	L"Pistolet",
	L"Pistolet maszynowy",
	L"Karabin maszynowy",
	L"Karabin",
	L"Karabin snajperski",
	L"Karabin bojowy",
	L"Lekki karabin maszynowy",
	L"Strzelba"
};

const wchar_t TeamTurnString[][STRING_LENGTH] =
{
	L"Tura gracza", // player's turn
	L"Tura przeciwnika",
	L"Tura stworze�",
	L"Tura samoobrony",
	L"Tura cywili"
	// planning turn
};

const wchar_t Message[][STRING_LENGTH] =
{
	L"",

	// In the following 8 strings, the %ls is the merc's name, and the %d (if any) is a number.

	L"%ls dosta�(a) w g�ow� i traci 1 punkt inteligencji!",
	L"%ls dosta�(a) w rami� i traci 1 punkt zr�czno�ci!",
	L"%ls dosta�(a) w klatk� piersiow� i traci 1 punkt si�y!",
	L"%ls dosta�(a) w nogi i traci 1 punkt zwinno�ci!",
	L"%ls dosta�(a) w g�ow� i traci %d pkt. inteligencji!",
	L"%ls dosta�(a) w rami� i traci %d pkt. zr�czno�ci!",
	L"%ls dosta�(a) w klatk� piersiow� i traci %d pkt. si�y!",
	L"%ls dosta�(a) w nogi i traci %d pkt. zwinno�ci!",
	L"Przerwanie!",

	// The first %ls is a merc's name, the second is a string from pNoiseVolStr,
	// the third is a string from pNoiseTypeStr, and the last is a string from pDirectionStr

	L"", //OBSOLETE
	L"Dotar�y twoje posi�ki!",

	// In the following four lines, all %ls's are merc names

	L"%ls prze�adowuje.",
	L"%ls posiada za ma�o Punkt�w Akcji!",
	L"%ls udziela pierwszej pomocy. (Naci�nij dowolny klawisz aby przerwa�.)",
	L"%ls i %ls udzielaj� pierwszej pomocy. (Naci�nij dowolny klawisz aby przerwa�.)",
	// the following 17 strings are used to create lists of gun advantages and disadvantages
	// (separated by commas)
	L"niezawodna",
	L"zawodna",
	L"�atwa w naprawie",
	L"trudna do naprawy",
	L"solidna",
	L"niesolidna",
	L"szybkostrzelna",
	L"wolno strzelaj�ca",
	L"daleki zasi�g",
	L"kr�tki zasi�g",
	L"ma�a waga",
	L"du�a waga",
	L"niewielkie rozmiary",
	L"szybki ci�g�y ogie�",
	L"brak ci�g�ego ognia",
	L"du�y magazynek",
	L"ma�y magazynek",

	// In the following two lines, all %ls's are merc names

	L"%ls: kamufla� si� star�.",
	L"%ls: kamufla� si� zmy�.",

	// The first %ls is a merc name and the second %ls is an item name

	L"Brak amunicji w dodatkowej broni!",
	L"%ls ukrad�(a): %ls.",

	// The %ls is a merc name

	L"%ls ma bro� bez funkcji ci�g�ego ognia.",

	L"Ju� masz co� takiego do��czone.",
	L"Po��czy� przedmioty?",

	// Both %ls's are item names

	L"%ls i %ls nie pasuj� do siebie.",

	L"Brak",
	L"Wyjmij amunicj�",
	L"Dodatki",

	//You cannot use "item(s)" and your "other item" at the same time.
	//Ex:  You cannot use sun goggles and you gas mask at the same time.
	L" %ls i %ls nie mog� by� u�ywane jednocze�nie.",

	L"Element, kt�ry masz na kursorze myszy mo�e by� do��czony do pewnych przedmiot�w, poprzez umieszczenie go w jednym z czterech slot�w.",
	L"Element, kt�ry masz na kursorze myszy mo�e by� do��czony do pewnych przedmiot�w, poprzez umieszczenie go w jednym z czterech slot�w. (Jednak w tym przypadku, przedmioty do siebie nie pasuj�.)",
	L"Ten sektor nie zosta� oczyszczony z wrog�w!",
	L"Wci�� musisz da� %ls %ls",
	L"%ls dosta�(a) w g�ow�!",
	L"Przerwa� walk�?",
	L"Ta zmiana b�dzie trwa�a. Kontynuowa�?",
	L"%ls ma wi�cej energii!",
	L"%ls po�lizgn��(n�a) si� na kulkach!",
	L"%ls nie chwyci�(a) - %ls!",
	L"%ls naprawi�(a) %ls",
	L"Przerwanie dla: ",
	L"Podda� si�?",
	L"Ta osoba nie chce twojej pomocy.",
	L"NIE S�DZ�!",
  	L"Aby podr�owa� helikopterem Skyridera, musisz najpierw zmieni� przydzia� najemnik�w na POJAZD/HELIKOPTER.",
	L"%ls mia�(a) czas by prze�adowa� tylko jedn� bro�",
	L"Tura dzikich kot�w",
};


// the names of the towns in the game

const wchar_t* pTownNames[] =
{
	L"",
	L"Omerta",
	L"Drassen",
	L"Alma",
	L"Grumm",
	L"Tixa",
	L"Cambria",
	L"San Mona",
	L"Estoni",
	L"Orta",
	L"Balime",
	L"Meduna",
	L"Chitzena",
};

// the types of time compression. For example: is the timer paused? at normal speed, 5 minutes per second, etc.
// min is an abbreviation for minutes

const wchar_t* sTimeStrings[] =
{
	L"Pauza",
	L"Normalna",
	L"5 min.",
	L"30 min.",
	L"60 min.",
	L"6 godz.", //NEW
};


// Assignment Strings: what assignment does the merc  have right now? For example, are they on a squad, training,
// administering medical aid (doctor) or training a town. All are abbreviated. 8 letters is the longest it can be.

const wchar_t* pAssignmentStrings[] =
{
	L"Oddz. 1",
	L"Oddz. 2",
	L"Oddz. 3",
	L"Oddz. 4",
	L"Oddz. 5",
	L"Oddz. 6",
	L"Oddz. 7",
	L"Oddz. 8",
	L"Oddz. 9",
	L"Oddz. 10",
	L"Oddz. 11",
	L"Oddz. 12",
	L"Oddz. 13",
	L"Oddz. 14",
	L"Oddz. 15",
	L"Oddz. 16",
	L"Oddz. 17",
	L"Oddz. 18",
	L"Oddz. 19",
	L"Oddz. 20",
	L"S�u�ba", // on active duty
	L"Lekarz", // administering medical aid
	L"Pacjent", // getting medical aid
	L"Pojazd", // sitting around resting
	L"Podr�", // in transit - abbreviated form
	L"Naprawa", // repairing
	L"Praktyka", // training themselves  // ***************NEW******************** as of June 24. 1998
	L"Samoobr.", // training a town to revolt // *************NEW******************** as of June 24, 1998
	L"Instruk.", // training a teammate
	L"Ucze�", // being trained by someone else // *******************NEW************** as of June 24, 1998
	L"Nie �yje", // dead
	L"Obezw�.", // abbreviation for incapacitated
	L"Jeniec", // Prisoner of war - captured
	L"Szpital", // patient in a hospital
	L"Pusty",	// Vehicle is empty
};


const wchar_t* pMilitiaString[] =
{
	L"Samoobrona", // the title of the militia box
	L"Bez przydzia�u", //the number of unassigned militia troops
	L"Nie mo�esz przemieszcza� oddzia��w samoobrony gdy nieprzyjaciel jest w sektorze!",
};


const wchar_t* pMilitiaButtonString[] =
{
	L"Auto", // auto place the militia troops for the player
	L"OK", // done placing militia troops
};

const wchar_t* pConditionStrings[] =
{
	L"Doskona�y", //the state of a soldier .. excellent health
	L"Dobry", // good health
	L"Do�� dobry", // fair health
	L"Ranny", // wounded health
	L"Zm�czony",//L"Wyczerpany", // tired
	L"Krwawi", // bleeding to death
	L"Nieprzyt.", // knocked out
	L"Umieraj�cy", // near death
	L"Nie �yje", // dead
};

const wchar_t* pEpcMenuStrings[] =
{
	L"S�u�ba", // set merc on active duty
	L"Pacjent", // set as a patient to receive medical aid
	L"Pojazd", // tell merc to enter vehicle
	L"Wypu��", // let the escorted character go off on their own
	L"Anuluj", // close this menu
};


// look at pAssignmentString above for comments

const wchar_t* pPersonnelAssignmentStrings[] =
{
	L"Oddz. 1",
	L"Oddz. 2",
	L"Oddz. 3",
	L"Oddz. 4",
	L"Oddz. 5",
	L"Oddz. 6",
	L"Oddz. 7",
	L"Oddz. 8",
	L"Oddz. 9",
	L"Oddz. 10",
	L"Oddz. 11",
	L"Oddz. 12",
	L"Oddz. 13",
	L"Oddz. 14",
	L"Oddz. 15",
	L"Oddz. 16",
	L"Oddz. 17",
	L"Oddz. 18",
	L"Oddz. 19",
	L"Oddz. 20",
	L"S�u�ba",
	L"Lekarz",
	L"Pacjent",
	L"Pojazd",
	L"Podr�",
	L"Naprawa",
	L"Praktyka",
	L"Trenuje samoobron�",
	L"Instruktor",
	L"Ucze�",
	L"Nie �yje",
	L"Obezw�adniony",
	L"Jeniec",
	L"Szpital",
	L"Pusty",	// Vehicle is empty
};


// refer to above for comments

const wchar_t* pLongAssignmentStrings[] =
{
	L"Oddzia� 1",
	L"Oddzia� 2",
	L"Oddzia� 3",
	L"Oddzia� 4",
	L"Oddzia� 5",
	L"Oddzia� 6",
	L"Oddzia� 7",
	L"Oddzia� 8",
	L"Oddzia� 9",
	L"Oddzia� 10",
	L"Oddzia� 11",
	L"Oddzia� 12",
	L"Oddzia� 13",
	L"Oddzia� 14",
	L"Oddzia� 15",
	L"Oddzia� 16",
	L"Oddzia� 17",
	L"Oddzia� 18",
	L"Oddzia� 19",
	L"Oddzia� 20",
	L"S�u�ba",
	L"Lekarz",
	L"Pacjent",
	L"Pojazd",
	L"W podr�y",
	L"Naprawa",
	L"Praktyka",
	L"Trenuj samoobron�",
	L"Trenuj oddzia�",
	L"Ucze�",
	L"Nie �yje",
	L"Obezw�adniony",
	L"Jeniec",
	L"W szpitalu",
	L"Pusty",	// Vehicle is empty
};


// the contract options

const wchar_t* pContractStrings[] =
{
	L"Opcje kontraktu:",
	L"", // a blank line, required
	L"Zaproponuj 1 dzie�", // offer merc a one day contract extension
	L"Zaproponuj 1 tydzie�", // 1 week
	L"Zaproponuj 2 tygodnie", // 2 week
	L"Zwolnij", // end merc's contract
	L"Anuluj", // stop showing this menu
};

const wchar_t* pPOWStrings[] =
{
	L"Jeniec",  //an acronym for Prisoner of War
	L"??",
};

const wchar_t* pLongAttributeStrings[] =
{
	L"SI�A", //The merc's strength attribute. Others below represent the other attributes.
	L"ZR�CZNO��",
	L"ZWINNO��",
	L"INTELIGENCJA",
	L"UMIEJ�TNO�CI STRZELECKIE",
	L"WIEDZA MEDYCZNA",
	L"ZNAJOMO�� MECHANIKI",
	L"UMIEJ�TNO�� DOWODZENIA",
	L"ZNAJOMO�� MATERIA��W WYBUCHOWYCH",
	L"POZIOM DO�WIADCZENIA",
};

const wchar_t* pInvPanelTitleStrings[] =
{
	L"Os�ona", // the armor rating of the merc
	L"Ekwip.", // the weight the merc is carrying
	L"Kamuf.", // the merc's camouflage rating
};

const wchar_t* pShortAttributeStrings[] =
{
	L"Zwn", // the abbreviated version of : agility
	L"Zrc", // dexterity
	L"Si�", // strength
	L"Dow", // leadership
	L"Int", // wisdom
	L"Do�", // experience level
	L"Str", // marksmanship skill
	L"Wyb", // explosive skill
	L"Mec", // mechanical skill
	L"Med", // medical skill
};


const wchar_t* pUpperLeftMapScreenStrings[] =
{
	L"Przydzia�", // the mercs current assignment // *********************NEW****************** as of June 24, 1998
	L"Kontrakt", // the contract info about the merc
	L"Zdrowie", // the health level of the current merc
	L"Morale", // the morale of the current merc
	L"Stan",	// the condition of the current vehicle
	L"Paliwo",	// the fuel level of the current vehicle
};

const wchar_t* pTrainingStrings[] =
{
	L"Praktyka", // tell merc to train self // ****************************NEW******************* as of June 24, 1998
	L"Samoobrona", // tell merc to train town // *****************************NEW ****************** as of June 24, 1998
	L"Instruktor", // tell merc to act as trainer
	L"Ucze�", // tell merc to be train by other // **********************NEW******************* as of June 24, 1998
};

const wchar_t* pGuardMenuStrings[] =
{
	L"Limit ognia:", // the allowable rate of fire for a merc who is guarding
	L" Agresywny ogie�", // the merc can be aggressive in their choice of fire rates
	L" Oszcz�dzaj amunicj�", // conserve ammo
	L" Strzelaj w ostateczno�ci", // fire only when the merc needs to
	L"Inne opcje:", // other options available to merc
	L" Mo�e si� wycofa�", // merc can retreat
	L" Mo�e szuka� schronienia",  // merc is allowed to seek cover
	L" Mo�e pomaga� partnerom", // merc can assist teammates
	L"OK", // done with this menu
	L"Anuluj", // cancel this menu
};

// This string has the same comments as above, however the * denotes the option has been selected by the player

const wchar_t* pOtherGuardMenuStrings[] =
{
	L"Limit ognia:",
	L" *Agresywny ogie�*",
	L" *Oszcz�dzaj amunicj�*",
	L" *Strzelaj w ostateczno�ci*",
	L"Inne opcje:",
	L" *Mo�e si� wycofa�*",
	L" *Mo�e szuka� schronienia*",
	L" *Mo�e pomaga� partnerom*",
	L"OK",
	L"Anuluj",
};

const wchar_t* pAssignMenuStrings[] =
{
	L"S�u�ba", // merc is on active duty
	L"Lekarz", // the merc is acting as a doctor
	L"Pacjent", // the merc is receiving medical attention
	L"Pojazd", // the merc is in a vehicle
	L"Naprawa", // the merc is repairing items
	L"Szkolenie", // the merc is training
	L"Anuluj", // cancel this menu
};

const wchar_t* pRemoveMercStrings[] =
{
	L"Usu� najemnika", // remove dead merc from current team
	L"Anuluj",
};

const wchar_t* pAttributeMenuStrings[] =
{
	L"Si�a",
	L"Zr�czno��",
	L"Zwinno��",
	L"Zdrowie",
	L"Um. strzeleckie",
	L"Wiedza med.",
	L"Zn. mechaniki",
	L"Um. dowodzenia",
	L"Zn. mat. wyb.",
	L"Anuluj",
};

const wchar_t* pTrainingMenuStrings[] =
{
 L"Praktyka", // train yourself //****************************NEW************************** as of June 24, 1998
 L"Samoobrona", // train the town // ****************************NEW ************************* as of June 24, 1998
 L"Instruktor", // train your teammates // *******************NEW************************** as of June 24, 1998
 L"Ucze�",  // be trained by an instructor //***************NEW************************** as of June 24, 1998
 L"Anuluj", // cancel this menu
};


const wchar_t* pSquadMenuStrings[] =
{
	L"Oddzia�  1",
	L"Oddzia�  2",
	L"Oddzia�  3",
	L"Oddzia�  4",
	L"Oddzia�  5",
	L"Oddzia�  6",
	L"Oddzia�  7",
	L"Oddzia�  8",
	L"Oddzia�  9",
	L"Oddzia� 10",
	L"Oddzia� 11",
	L"Oddzia� 12",
	L"Oddzia� 13",
	L"Oddzia� 14",
	L"Oddzia� 15",
	L"Oddzia� 16",
	L"Oddzia� 17",
	L"Oddzia� 18",
	L"Oddzia� 19",
	L"Oddzia� 20",
	L"Anuluj",
};


const wchar_t* pPersonnelScreenStrings[] =
{
	L"Zdrowie: ", // health of merc
	L"Zwinno��: ",
	L"Zr�czno��: ",
 	L"Si�a: ",
 	L"Um. dowodzenia: ",
 	L"Inteligencja: ",
 	L"Poziom do�w.: ", // experience level
 	L"Um. strzeleckie: ",
 	L"Zn. mechaniki: ",
 	L"Zn. mat. wybuchowych: ",
 	L"Wiedza medyczna: ",
 	L"Zastaw na �ycie: ", // amount of medical deposit put down on the merc
 	L"Bie��cy kontrakt: ", // cost of current contract
 	L"Liczba zab�jstw: ", // number of kills by merc
 	L"Liczba asyst: ", // number of assists on kills by merc
 	L"Dzienny koszt:", // daily cost of merc
 	L"Og�lny koszt:", // total cost of merc
 	L"Warto�� kontraktu:", // cost of current contract
 	L"Us�ugi og�em", // total service rendered by merc
 	L"Zaleg�a kwota", // amount left on MERC merc to be paid
 	L"Celno��:", // percentage of shots that hit target
 	L"Ilo�� walk:", // number of battles fought
 	L"Ranny(a):", // number of times merc has been wounded
 	L"Umiej�tno�ci:",
 	L"Brak umi�j�tno�ci",
};


//These string correspond to enums used in by the SkillTrait enums in SoldierProfileType.h
const wchar_t* gzMercSkillText[] =
{
	L"Brak umiej�tno�ci",
	L"Otwieranie zamk�w",
	L"Walka wr�cz",
	L"Elektronika",
	L"Nocne operacje",
	L"Rzucanie",
	L"Szkolenie",
	L"Ci�ka bro�",
	L"Bro� automatyczna",
	L"Skradanie si�",
	L"Obur�czno��",
	L"Kradzie�e",
	L"Sztuki walki",
	L"Bro� bia�a",
	L"Snajper",
	L"Kamufla�",
	L"(Eksp.)",
};


// This is pop up help text for the options that are available to the merc

const wchar_t* pTacticalPopupButtonStrings[] =
{
	L"W|sta�/Id�",
	L"S|chyl si�/Id�",
	L"Wsta�/Biegnij (|R)",
	L"|Padnij/Czo�gaj si�",
	L"Patrz (|L)",
	L"Akcja",
	L"Rozmawiaj",
	L"Zbadaj (|C|t|r|l)",

	// Pop up door menu
	L"Otw�rz",
	L"Poszukaj pu�apek",
	L"U�yj wytrych�w",
	L"Wywa�",
	L"Usu� pu�apki",
	L"Zamknij na klucz",
	L"Otw�rz kluczem",
	L"U�yj �adunku wybuchowego",
	L"U�yj �omu",
	L"Anuluj (|E|s|c)",
	L"Zamknij"
};

// Door Traps. When we examine a door, it could have a particular trap on it. These are the traps.

const wchar_t* pDoorTrapStrings[] =
{
	L"nie posiada �adnych pu�apek",
	L"ma za�o�ony �adunek wybuchowy",
	L"jest pod napi�ciem",
	L"posiada syren� alarmow�",
	L"posiada dyskretny alarm"
};

// On the map screen, there are four columns. This text is popup help text that identifies the individual columns.

const wchar_t* pMapScreenMouseRegionHelpText[] =
{
	L"Wyb�r postaci",
	L"Przydzia� najemnika",
	L"Nanie� tras� podr�y",
	L"Kontrakt najemnika",
	L"Usu� najemnika",
	L"�pij", // *****************************NEW********************* as of June 29, 1998
};

// volumes of noises

const wchar_t* pNoiseVolStr[] =
{
	L"CICHY",
	L"WYRA�NY",
	L"G�O�NY",
	L"BARDZO G�O�NY"
};

// types of noises

const wchar_t* pNoiseTypeStr[] = // OBSOLETE
{
	L"NIEOKRE�LONY D�WI�K",
	L"ODG�OS RUCHU",
	L"ODG�OS SKRZYPNI�CIA",
	L"PLUSK",
	L"ODG�OS UDERZENIA",
	L"STRZA�",
	L"WYBUCH",
	L"KRZYK",
	L"ODG�OS UDERZENIA",
	L"ODG�OS UDERZENIA",
	L"�OMOT",
	L"TRZASK"
};

// Directions that are used to report noises

const wchar_t* pDirectionStr[] =
{
	L"P�N-WSCH",
	L"WSCH",
	L"P�D-WSCH",
	L"P�D",
	L"P�D-ZACH",
	L"ZACH",
	L"P�N-ZACH",
	L"P�N"
};

// These are the different terrain types.

const wchar_t* pLandTypeStrings[] =
{
	L"Miasto",
	L"Droga",
	L"Otwarty teren",
	L"Pustynia",
	L"Las",
	L"Las",
	L"Bagno",
	L"Woda",
	L"Wzg�rza",
	L"Teren nieprzejezdny",
	L"Rzeka",	//river from north to south
	L"Rzeka",	//river from east to west
	L"Terytorium innego kraju",
	//NONE of the following are used for directional travel, just for the sector description.
	L"Tropiki",
	L"Pola uprawne",
	L"Otwarty teren, droga",
	L"Las, droga",
	L"Las, droga",
	L"Tropiki, droga",
	L"Las, droga",
	L"Wybrze�e",
	L"G�ry, droga",
	L"Wybrze�e, droga",
	L"Pustynia, droga",
	L"Bagno, droga",
	L"Las, Rakiety Z-P",
	L"Pustynia, Rakiety Z-P",
	L"Tropiki, Rakiety Z-P",
	L"Meduna, Rakiety Z-P",

	//These are descriptions for special sectors
	L"Szpital w Cambrii",
	L"Lotnisko w Drassen",
	L"Lotnisko w Medunie",
	L"Rakiety Z-P",
	L"Kryj�wka rebeliant�w", //The rebel base underground in sector A10
	L"Tixa - Lochy",	//The basement of the Tixa Prison (J9)
	L"Gniazdo stworze�",	//Any mine sector with creatures in it
	L"Orta - Piwnica",	//The basement of Orta (K4)
	L"Tunel",				//The tunnel access from the maze garden in Meduna
						//leading to the secret shelter underneath the palace
	L"Schron",				//The shelter underneath the queen's palace
	L"",							//Unused
};

const wchar_t* gpStrategicString[] =
{
	L"",	//Unused
	L"%ls wykryto w sektorze %c%d, a inny oddzia� jest w drodze.",	//STR_DETECTED_SINGULAR
	L"%ls wykryto w sektorze %c%d, a inne oddzia�y s� w drodze.",	//STR_DETECTED_PLURAL
	L"Chcesz skoordynowa� jednoczesne przybycie?",			//STR_COORDINATE

	//Dialog strings for enemies.

	L"Wr�g daje ci szans� si� podda�.",			//STR_ENEMY_SURRENDER_OFFER
	L"Wr�g schwyta� reszt� twoich nieprzytomnych najemnik�w.",	//STR_ENEMY_CAPTURED

	//The text that goes on the autoresolve buttons

	L"Odwr�t", 		//The retreat button				//STR_AR_RETREAT_BUTTON
	L"OK",		//The done button				//STR_AR_DONE_BUTTON

	//The headers are for the autoresolve type (MUST BE UPPERCASE)

	L"OBRONA",								//STR_AR_DEFEND_HEADER
	L"ATAK",								//STR_AR_ATTACK_HEADER
	L"STARCIE",								//STR_AR_ENCOUNTER_HEADER
	L"Sektor",		//The Sector A9 part of the header		//STR_AR_SECTOR_HEADER

	//The battle ending conditions

	L"ZWYCI�STWO!",								//STR_AR_OVER_VICTORY
	L"PORA�KA!",								//STR_AR_OVER_DEFEAT
	L"KAPITULACJA!",							//STR_AR_OVER_SURRENDERED
	L"NIEWOLA!",								//STR_AR_OVER_CAPTURED
	L"ODWR�T!",								//STR_AR_OVER_RETREATED

	//These are the labels for the different types of enemies we fight in autoresolve.

	L"Samoobrona",							//STR_AR_MILITIA_NAME,
	L"Elity",								//STR_AR_ELITE_NAME,
	L"�o�nierze",								//STR_AR_TROOP_NAME,
	L"Administrator",							//STR_AR_ADMINISTRATOR_NAME,
	L"Stworzenie",								//STR_AR_CREATURE_NAME,

	//Label for the length of time the battle took

	L"Czas trwania",							//STR_AR_TIME_ELAPSED,

	//Labels for status of merc if retreating.  (UPPERCASE)

	L"WYCOFA�(A) SI�",								//STR_AR_MERC_RETREATED,
	L"WYCOFUJE SI�",								//STR_AR_MERC_RETREATING,
	L"WYCOFAJ SI�",								//STR_AR_MERC_RETREAT,

	//PRE BATTLE INTERFACE STRINGS
	//Goes on the three buttons in the prebattle interface.  The Auto resolve button represents
	//a system that automatically resolves the combat for the player without having to do anything.
	//These strings must be short (two lines -- 6-8 chars per line)

	L"Rozst. autom.",							//STR_PB_AUTORESOLVE_BTN,
	L"Id� do sektora",							//STR_PB_GOTOSECTOR_BTN,
	L"Wycof. ludzi",							//STR_PB_RETREATMERCS_BTN,

	//The different headers(titles) for the prebattle interface.
	L"STARCIE Z WROGIEM",							//STR_PB_ENEMYENCOUNTER_HEADER,
	L"INWAZJA WROGA",							//STR_PB_ENEMYINVASION_HEADER, // 30
	L"ZASADZKA WROGA",
	L"WEJ�CIE DO WROGIEGO SEKTORA",
	L"ATAK STWOR�W",
	L"ATAK DZIKICH KOT�W",							//STR_PB_BLOODCATAMBUSH_HEADER
	L"WEJ�CIE DO LEGOWISKA DZIKICH KOT�W",			//STR_PB_ENTERINGBLOODCATLAIR_HEADER

	//Various single words for direct translation.  The Civilians represent the civilian
	//militia occupying the sector being attacked.  Limited to 9-10 chars

	L"Po�o�enie",
	L"Wrogowie",
	L"Najemnicy",
	L"Samoobrona",
	L"Stwory",
	L"Dzikie koty",
	L"Sektor",
	L"Brak",		//If there are no uninvolved mercs in this fight.
	L"N/D",			//Acronym of Not Applicable N/A
	L"d",			//One letter abbreviation of day
	L"g",			//One letter abbreviation of hour

	//TACTICAL PLACEMENT USER INTERFACE STRINGS
	//The four buttons

	L"Wyczy��",
	L"Rozprosz",
	L"Zgrupuj",
	L"OK",

	//The help text for the four buttons.  Use \n to denote new line (just like enter).

	L"Kasuje wszystkie pozy|cje najemnik�w, \ni pozwala ponownie je wprowadzi�.",
	L"Po ka�dym naci�ni�ciu rozmie|szcza\nlosowo twoich najemnik�w.",
	L"|Grupuje najemnik�w w wybranym miejscu.",
	L"Kliknij ten klawisz gdy ju� rozmie�cisz \nswoich najemnik�w. (|E|n|t|e|r)",
	L"Musisz rozmie�ci� wszystkich najemnik�w \nzanim rozpoczniesz walk�.",

	//Various strings (translate word for word)

	L"Sektor",
	L"Wybierz pocz�tkowe pozycje",

	//Strings used for various popup message boxes.  Can be as long as desired.

	L"To miejsce nie jest zbyt dobre. Jest niedost�pne. Spr�buj gdzie indziej.",
	L"Rozmie�� swoich najemnik�w na pod�wietlonej cz�ci mapy.",

	//This message is for mercs arriving in sectors.  Ex:  Red has arrived in sector A9.
	//Don't uppercase first character, or add spaces on either end.

	L"przyby�(a) do sektora",

	//These entries are for button popup help text for the prebattle interface.  All popup help
	//text supports the use of \n to denote new line.  Do not use spaces before or after the \n.
	L"|Automatycznie prowadzi walk� za ciebie,\nnie �aduj�c planszy.",
	L"Atakuj�c sektor wroga nie mo�na automatycznie rozstrzygn�� walki.",
	L"Wej�cie do s|ektora by nawi�za� walk� z wrogiem.",
	L"Wycofuje oddzia� do s�siedniego sekto|ra.",				//singular version
	L"Wycofuje wszystkie oddzia�y do s�siedniego sekto|ra.", //multiple groups with same previous sector

	//various popup messages for battle conditions.

	//%c%d is the sector -- ex:  A9
	L"Nieprzyjaciel zatakowa� oddzia�y samoobrony w sektorze %c%d.",
	//%c%d is the sector -- ex:  A9
	L"Stworzenia zaatakowa�y oddzia�y samoobrony w sektorze %c%d.",
	//1st %d refers to the number of civilians eaten by monsters,  %c%d is the sector -- ex:  A9
	//Note:  the minimum number of civilians eaten will be two.
	L"Stworzenia zatakowa�y i zabi�y %d cywili w sektorze %ls.",
	//%c%d is the sector -- ex:  A9
	L"Nieprzyjaciel zatakowa� twoich najemnik�w w sektorze %ls.  �aden z twoich najemnik�w nie mo�e walczy�!",
	//%c%d is the sector -- ex:  A9
	L"Stworzenia zatakowa�y twoich najemnik�w w sektorze %ls.  �aden z twoich najemnik�w nie mo�e walczy�!",

};

const wchar_t* gpGameClockString[] =
{
	//This is the day represented in the game clock.  Must be very short, 4 characters max.
	L"Dzie�",
};

//When the merc finds a key, they can get a description of it which
//tells them where and when they found it.
const wchar_t* sKeyDescriptionStrings[2] =
{
	L"Zn. w sektorze:",
	L"Zn. w dniu:",
};

//The headers used to describe various weapon statistics.

const wchar_t gWeaponStatsDesc[][ 14 ] =
{
	L"Waga (%ls):", // change kg to another weight unit if your standard is not kilograms, and TELL SIR-TECH!
	L"Stan:",
	L"Ilo��:", 		// Number of bullets left in a magazine
	L"Zas.:",		// Range
	L"Si�a:",		// Damage
    L"PA:",                 // abbreviation for Action Points
	L"",
	L"=",
	L"=",
};

//The headers used for the merc's money.

const wchar_t gMoneyStatsDesc[][ 13 ] =
{
	L"Kwota",
	L"Pozosta�o:", //this is the overall balance
	L"Kwota",
	L"Wydzieli�:", // the amount he wants to separate from the overall balance to get two piles of money

	L"Bie��ce",
	L"Saldo",
	L"Kwota do",
	L"podj�cia",
};

//The health of various creatures, enemies, characters in the game. The numbers following each are for comment
//only, but represent the precentage of points remaining.

const wchar_t zHealthStr[][13] =
{
	L"UMIERAJ�CY",		//	>= 0
	L"KRYTYCZNY", 		//	>= 15
	L"KIEPSKI",		//	>= 30
	L"RANNY",    	//	>= 45
	L"ZDROWY",    	//	>= 60
	L"SILNY",     	// 	>= 75
	L"DOSKONA�Y",		// 	>= 90
};

const wchar_t* gzMoneyAmounts[6] =
{
	L"$1000",
	L"$100",
	L"$10",
	L"OK",
	L"Wydziel",
	L"Podejmij",
};

// short words meaning "Advantages" for "Pros" and "Disadvantages" for "Cons."
const wchar_t gzProsLabel[10] =
{
	L"Zalety:",
};

const wchar_t gzConsLabel[10] =
{
	L"Wady:",
};

//Conversation options a player has when encountering an NPC
const wchar_t zTalkMenuStrings[6][ SMALL_STRING_LENGTH ] =
{
	L"Powt�rz", 	//meaning "Repeat yourself"
	L"Przyja�nie",		//approach in a friendly
	L"Bezpo�rednio",		//approach directly - let's get down to business
	L"Gro�nie",		//approach threateningly - talk now, or I'll blow your face off
	L"Daj",
	L"Rekrutuj",
};

//Some NPCs buy, sell or repair items. These different options are available for those NPCs as well.
const wchar_t zDealerStrings[4][ SMALL_STRING_LENGTH ]=
{
	L"Kup/Sprzedaj",
	L"Kup",
	L"Sprzedaj",
	L"Napraw",
};

const wchar_t zDialogActions[1][ SMALL_STRING_LENGTH ] =
{
	L"OK",
};


//These are vehicles in the game.

const wchar_t* pVehicleStrings[] =
{
 L"Eldorado",
 L"Hummer", // a hummer jeep/truck -- military vehicle
 L"Furgonetka z lodami",
 L"Jeep",
 L"Czo�g",
 L"Helikopter",
};

const wchar_t* pShortVehicleStrings[] =
{
	L"Eldor.",
	L"Hummer",			// the HMVV
	L"Furg.",
	L"Jeep",
	L"Czo�g",
	L"Heli.", 				// the helicopter
};

const wchar_t* zVehicleName[] =
{
	L"Eldorado",
	L"Hummer",		//a military jeep. This is a brand name.
	L"Furg.",			// Ice cream truck
	L"Jeep",
	L"Czo�g",
	L"Heli.", 		//an abbreviation for Helicopter
};


//These are messages Used in the Tactical Screen

const wchar_t TacticalStr[][ MED_STRING_LENGTH ] =
{
	L"Nalot",
	L"Udzieli� automatycznie pierwszej pomocy?",

	// CAMFIELD NUKE THIS and add quote #66.

	L"%ls zauwa�y�(a) �e dostawa jest niekompletna.",

	// The %ls is a string from pDoorTrapStrings

	L"Zamek %ls.",
	L"Brak zamka.",
	L"Sukces!",
	L"Niepowodzenie.",
	L"Sukces!",
	L"Niepowodzenie.",
	L"Zamek nie ma pu�apek.",
	L"Sukces!",
	// The %ls is a merc name
	L"%ls nie posiada odpowiedniego klucza.",
	L"Zamek zosta� rozbrojony.",
	L"Zamek nie ma pu�apek.",
	L"Zamkni�te.",
	L"DRZWI",
	L"ZABEZP.",
	L"ZAMKNI�TE",
	L"OTWARTE",
	L"ROZWALONE",
	L"Tu jest prze��cznik. W��czy� go?",
	L"Rozbroi� pu�apk�?",
	L"Poprz...",
	L"Nast...",
	L"Wi�cej...",

	// In the next 2 strings, %ls is an item name

	L"%ls - po�o�ono na ziemi.",
	L"%ls - przekazano do - %ls.",

	// In the next 2 strings, %ls is a name

	L"%ls otrzyma�(a) ca�� zap�at�.",
	L"%ls - nale�no�� wobec niej/niego wynosi jeszcze %d.",
	L"Wybierz cz�stotliwo�� sygna�u detonuj�cego:",  	//in this case, frequency refers to a radio signal
	L"Ile tur do eksplozji:",	//how much time, in turns, until the bomb blows
	L"Ustaw cz�stotliwo�� zdalnego detonatora:", 	//in this case, frequency refers to a radio signal
	L"Rozbroi� pu�apk�?",
	L"Usun�� niebiesk� flag�?",
	L"Umie�ci� tutaj niebiesk� flag�?",
	L"Ko�cz�ca tura",

	// In the next string, %ls is a name. Stance refers to way they are standing.

	L"Na pewno chcesz zaatakowa� - %ls?",
	L"Pojazdy nie mog� zmienia� pozycji.",
	L"Robot nie mo�e zmienia� pozycji.",

	// In the next 3 strings, %ls is a name

	L"%ls nie mo�e zmieni� pozycji w tym miejscu.",
	L"%ls nie mo�e tu otrzyma� pierwszej pomocy.",
	L"%ls nie potrzebuje pierwszej pomocy.",
	L"Nie mo�na ruszy� w to miejsce.",
	L"Oddzia� jest ju� kompletny. Nie ma miejsca dla nowych rekrut�w.",	//there's no room for a recruit on the player's team

	// In the next string, %ls is a name

	L"%ls pracuje ju� dla ciebie.",

	// Here %ls is a name and %d is a number

	L"%ls - nale�no�� wobec niej/niego wynosi %d$.",

	// In the next string, %ls is a name

	L"%ls - Eskortowa� t� osob�?",

	// In the next string, the first %ls is a name and the second %ls is an amount of money (including $ sign)

	L"%ls - Zatrudni� t� osob� za %ls dziennie?",

	// This line is used repeatedly to ask player if they wish to participate in a boxing match.

	L"Chcesz walczy�?",

	// In the next string, the first %ls is an item name and the
	// second %ls is an amount of money (including $ sign)

	L"%ls - Kupi� to za %ls?",

	// In the next string, %ls is a name

	L"%ls jest pod eskort� oddzia�u %d.",

	// These messages are displayed during play to alert the player to a particular situation

	L"ZACI�TA",					//weapon is jammed.
	L"Robot potrzebuje amunicji kaliber %ls.",		//Robot is out of ammo
	L"Rzuci� tam? To niemo�liwe.",		//Merc can't throw to the destination he selected

	// These are different buttons that the player can turn on and off.

	L"Skradanie si� (|Z)",
	L"Ekran |Mapy",
	L"Koniec tury (|D)",
	L"Rozmowa",
	L"Wycisz",
	L"Pozycja do g�ry (|P|g|U|p)",
	L"Poziom kursora (|T|a|b)",
	L"Wspinaj si� / Zeskocz",
	L"Pozycja w d� (|P|g|D|n)",
	L"Bada� (|C|t|r|l)",
	L"Poprzedni najemnik",
	L"Nast�pny najemnik (|S|p|a|c|j|a)",
	L"|Opcje",
	L"Ci�g�y ogie� (|B)",
	L"Sp�jrz/Obr�� si� (|L)",
	L"Zdrowie: %d/%d\nEnergia: %d/%d\nMorale: %ls",
	L"Co?",					//this means "what?"
	L"Kont",					//an abbrieviation for "Continued"
	L"%ls ma w��czone potwierdzenia g�osowe.",
	L"%ls ma wy��czone potwierdzenia g�osowe.",
	L"Stan: %d/%d\nPaliwo: %d/%d",
	L"Wysi�d� z pojazdu" ,
	L"Zmie� oddzia� ( |S|h|i|f|t |S|p|a|c|j|a )",
	L"Prowad�",
	L"N/D",						//this is an acronym for "Not Applicable."
	L"U�yj ( Walka wr�cz )",
	L"U�yj ( Broni palnej )",
	L"U�yj ( Broni bia�ej )",
	L"U�yj ( Mat. wybuchowych )",
	L"U�yj ( Apteczki )",
	L"(�ap)",
	L"(Prze�aduj)",
	L"(Daj)",
	L"%ls - pu�apka zosta�a uruchomiona.",
	L"%ls przyby�(a) na miejsce.",
	L"%ls straci�(a) wszystkie Punkty Akcji.",
	L"%ls jest nieosi�galny(na).",
	L"%ls ma ju� za�o�one opatrunki.",
	L"%ls nie ma banda�y.",
	L"Wr�g w sektorze!",
	L"Nie ma wroga w zasi�gu wzroku.",
	L"Zbyt ma�o Punkt�w Akcji.",
	L"Nikt nie u�ywa zdalnego sterowania.",
	L"Ci�g�y ogie� opr�ni� magazynek!",
	L"�O�NIERZ",
	L"STW�R",
	L"SAMOOBRONA",
	L"CYWIL",
	L"Wyj�cie z sektora",
	L"OK",
	L"Anuluj",
	L"Wybrany najemnik",
	L"Wszyscy najemnicy w oddziale",
	L"Id� do sektora",
	L"Otw�rz map�",
	L"Nie mo�na opu�ci� sektora z tej strony.",
	L"%ls jest zbyt daleko.",
	L"Usu� korony drzew",
	L"Poka� korony drzew",
	L"WRONA",				//Crow, as in the large black bird
	L"SZYJA",
	L"G�OWA",
	L"TU��W",
	L"NOGI",
	L"Powiedzie� kr�lowej to, co chce wiedzie�?",
	L"Wz�r odcisku palca pobrany",
	L"Niew�a�ciwy wz�r odcisku palca. Bro� bezu�yteczna.",
	L"Cel osi�gni�ty",
	L"Droga zablokowana",
	L"Wp�ata/Podj�cie pieni�dzy",		//Help text over the $ button on the Single Merc Panel
	L"Nikt nie potrzebuje pierwszej pomocy.",
	L"Zac.",						// Short form of JAMMED, for small inv slots
	L"Nie mo�na si� tam dosta�.",					// used ( now ) for when we click on a cliff
	L"Przej�cie zablokowane. Czy chcesz zamieni� si� miejscami z t� osob�?",
	L"Osoba nie chce si� przesun��.",
	// In the following message, '%ls' would be replaced with a quantity of money (e.g. $200)
	L"Zgadzasz si� zap�aci� %ls?",
	L"Zgadzasz si� na darmowe leczenie?",
	L"Zgadasz si� na ma��e�stwo z Darylem?",
	L"K�ko na klucze",
	L"Nie mo�esz tego zrobi� z eskortowan� osob�.",
	L"Oszcz�dzi� Krotta?",
	L"Poza zasi�giem broni",
	L"G�rnik",
	L"Pojazdem mo�na podr�owa� tylko pomi�dzy sektorami",
	L"Teraz nie mo�na automatycznie udzieli� pierwszej pomocy",
	L"Przej�cie zablokowane dla - %ls",
	L"Twoi najemnicy, schwytani przez �o�nierzy Deidranny, s� tutaj uwi�zieni!",
	L"Zamek zosta� trafiony",
	L"Zamek zosta� zniszczony",
	L"Kto� inny majstruje przy tych drzwiach.",
	L"Stan: %d/%d\nPaliwo: %d/%d",
  L"%ls nie widzi - %ls.",  // Cannot see person trying to talk to
};

//Varying helptext explains (for the "Go to Sector/Map" checkbox) what will happen given different circumstances in the "exiting sector" interface.
const wchar_t* pExitingSectorHelpText[] =
{
	//Helptext for the "Go to Sector" checkbox button, that explains what will happen when the box is checked.
	L"Je�li zaznaczysz t� opcj�, to s�siedni sektor zostanie natychmiast za�adowany.",
	L"Je�li zaznaczysz t� opcj�, to na czas podr�y pojawi si� automatycznie ekran mapy.",

	//If you attempt to leave a sector when you have multiple squads in a hostile sector.
	L"Ten sektor jest okupowany przez wroga i nie mo�esz tu zostawi� najemnik�w.\nMusisz upora� si� z t� sytuacj� zanim za�adujesz inny sektor.",

	//Because you only have one squad in the sector, and the "move all" option is checked, the "go to sector" option is locked to on.
	//The helptext explains why it is locked.
	L"Gdy wyprowadzisz swoich pozosta�ych najemnik�w z tego sektora,\ns�siedni sektor zostanie automatycznie za�adowany.",
	L"Gdy wyprowadzisz swoich pozosta�ych najemnik�w z tego sektora,\nto na czas podr�y pojawi si� automatycznie ekran mapy.",

	//If an EPC is the selected merc, it won't allow the merc to leave alone as the merc is being escorted.  The "single" button is disabled.
	L"%ls jest pod eskort� twoich najemnik�w i nie mo�e bez nich opu�ci� tego sektora.",

	//If only one conscious merc is left and is selected, and there are EPCs in the squad, the merc will be prohibited from leaving alone.
	//There are several strings depending on the gender of the merc and how many EPCs are in the squad.
	//DO NOT USE THE NEWLINE HERE AS IT IS USED FOR BOTH HELPTEXT AND SCREEN MESSAGES!
	L"%ls nie mo�e sam opu�ci� tego sektora, gdy� %ls jest pod jego eskort�.", //male singular
	L"%ls nie mo�e sama opu�ci� tego sektora, gdy� %ls jest pod jej eskort�.", //female singular
	L"%ls nie mo�e sam opu�ci� tego sektora, gdy� eskortuje inne osoby.", //male plural
	L"%ls nie mo�e sama opu�ci� tego sektora, gdy� eskortuje inne osoby.", //female plural

	//If one or more of your mercs in the selected squad aren't in range of the traversal area, then the  "move all" option is disabled,
	//and this helptext explains why.
	L"Wszyscy twoi najemnicy musz� by� w pobli�u,\naby oddzia� m�g� si� przemieszcza�.",

	L"", //UNUSED

	//Standard helptext for single movement.  Explains what will happen (splitting the squad)
	L"Je�li zaznaczysz t� opcj�, %ls b�dzie podr�owa� w pojedynk�\ni automatycznie znajdzie si� w osobnym oddziale.",

	//Standard helptext for all movement.  Explains what will happen (moving the squad)
	L"Je�li zaznaczysz t� opcj�, aktualnie\nwybrany oddzia� opu�ci ten sektor.",

	//This strings is used BEFORE the "exiting sector" interface is created.  If you have an EPC selected and you attempt to tactically
	//traverse the EPC while the escorting mercs aren't near enough (or dead, dying, or unconscious), this message will appear and the
	//"exiting sector" interface will not appear.  This is just like the situation where
	//This string is special, as it is not used as helptext.  Do not use the special newline character (\n) for this string.
	L"%ls jest pod eskort� twoich najemnik�w i nie mo�e bez nich opu�ci� tego sektora. Aby opu�ci� sektor twoi najemnicy musz� by� w pobli�u.",
};



const wchar_t* pRepairStrings[] =
{
	L"Wyposa�enie", 		// tell merc to repair items in inventory
	L"Baza rakiet Z-P", // tell merc to repair SAM site - SAM is an acronym for Surface to Air Missile
	L"Anuluj", 		// cancel this menu
	L"Robot", 		// repair the robot
};


// NOTE: combine prestatbuildstring with statgain to get a line like the example below.
// "John has gained 3 points of marksmanship skill."

const wchar_t* sPreStatBuildString[] =
{
	L"traci", 		// the merc has lost a statistic
	L"zyskuje", 		// the merc has gained a statistic
	L"pkt.",	// singular
	L"pkt.",	// plural
	L"pkt.",	// singular
	L"pkt.",	// plural
};

const wchar_t* sStatGainStrings[] =
{
	L"zdrowia.",
	L"zwinno�ci.",
	L"zr�czno�ci.",
	L"inteligencji.",
	L"umiej�tno�ci medycznych.",
	L"umiej�tno�ci w dziedzinie materia��w wybuchowych.",
	L"umiej�tno�ci w dziedzinie mechaniki.",
	L"umiej�tno�ci strzeleckich.",
	L"do�wiadczenia.",
	L"si�y.",
	L"umiej�tno�ci dowodzenia.",
};


const wchar_t* pHelicopterEtaStrings[] =
{
	L"Ca�kowita trasa:  ",// total distance for helicopter to travel
	L" Bezp.:   ", 			// distance to travel to destination
	L" Niebezp.:", 			// distance to return from destination to airport
	L"Ca�kowity koszt: ", 		// total cost of trip by helicopter
	L"PCP:  ", 			// ETA is an acronym for "estimated time of arrival"
	L"Helikopter ma ma�o paliwa i musi wyl�dowa� na terenie wroga.",	// warning that the sector the helicopter is going to use for refueling is under enemy control ->
  L"Pasa�erowie: ",
  L"Wyb�r Skyridera lub pasa�er�w?",
  L"Skyrider",
  L"Pasa�erowie",
};

const wchar_t* sMapLevelString[] =
{
	L"Poziom:", 			// what level below the ground is the player viewing in mapscreen
};

const wchar_t* gsLoyalString[] =
{
	L"Lojalno�ci", 			// the loyalty rating of a town ie : Loyal 53%
};


// error message for when player is trying to give a merc a travel order while he's underground.

const wchar_t* gsUndergroundString[] =
{
	L"nie mo�na wydawa� rozkaz�w podr�y pod ziemi�.",
};

const wchar_t* gsTimeStrings[] =
{
	L"g",				// hours abbreviation
	L"m",				// minutes abbreviation
	L"s",				// seconds abbreviation
	L"d",				// days abbreviation
};

// text for the various facilities in the sector

const wchar_t* sFacilitiesStrings[] =
{
	L"Brak",
	L"Szpital",
	L"Przemys�",
	L"Wi�zienie",
	L"Baza wojskowa",
	L"Lotnisko",
	L"Strzelnica",		// a field for soldiers to practise their shooting skills
};

// text for inventory pop up button

const wchar_t* pMapPopUpInventoryText[] =
{
	L"Inwentarz",
	L"Zamknij",
};

// town strings

const wchar_t* pwTownInfoStrings[] =
{
	L"Rozmiar",					// 0 // size of the town in sectors
	L"", 						// blank line, required
	L"Pod kontrol�", 					// how much of town is controlled
	L"Brak", 					// none of this town
	L"Przynale�na kopalnia", 				// mine associated with this town
	L"Lojalno��",					// 5 // the loyalty level of this town
	L"Wyszkolonych", 					// the forces in the town trained by the player
	L"",
	L"G��wne obiekty", 				// main facilities in this town
	L"Poziom", 					// the training level of civilians in this town
	L"Szkolenie cywili",				// 10 // state of civilian training in town
	L"Samoobrona", 					// the state of the trained civilians in the town
};

// Mine strings

const wchar_t* pwMineStrings[] =
{
	L"Kopalnia",						// 0
	L"Srebro",
	L"Z�oto",
	L"Dzienna produkcja",
	L"Mo�liwa produkcja",
	L"Opuszczona",				// 5
	L"Zamkni�ta",
	L"Na wyczerpaniu",
	L"Produkuje",
	L"Stan",
	L"Tempo produkcji",
	L"Typ z�o�a",				// 10
	L"Kontrola miasta",
	L"Lojalno�� miasta",
//	L"G�rnicy",
};

// blank sector strings

const wchar_t* pwMiscSectorStrings[] =
{
	L"Si�y wroga",
	L"Sektor",
	L"Przedmiot�w",
	L"Nieznane",
	L"Pod kontrol�",
	L"Tak",
	L"Nie",
};

// error strings for inventory

const wchar_t* pMapInventoryErrorString[] =
{
	L"%ls jest zbyt daleko.",	//Merc is in sector with item but not close enough
	L"Nie mo�na wybra� tego najemnika.",  //MARK CARTER
	L"%ls nie mo�e st�d zabra� tego przedmiotu, gdy� nie jest w tym sektorze.",
	L"Podczas walki nie mo�na korzysta� z tego panelu.",
	L"Podczas walki nie mo�na korzysta� z tego panelu.",
	L"%ls nie mo�e tu zostawi� tego przedmiotu, gdy� nie jest w tym sektorze.",
};

const wchar_t* pMapInventoryStrings[] =
{
	L"Po�o�enie", 			// sector these items are in
	L"Razem przedmiot�w", 		// total number of items in sector
};


// help text for the user

const wchar_t* pMapScreenFastHelpTextList[] =
{
	L"Kliknij w kolumnie 'Przydz.', aby przydzieli� najemnika do innego oddzia�u lub wybranego zadania.",
	L"Aby wyznaczy� najemnikowi cel w innym sektorze, kliknij pole w kolumnie 'Cel'.",
	L"Gdy najemnicy otrzymaj� ju� rozkaz przemieszczenia si�, kompresja czasu pozwala im szybciej dotrze� na miejsce.",
	L"Kliknij lewym klawiszem aby wybra� sektor. Kliknij ponownie aby wyda� najemnikom rozkazy przemieszczenia, lub kliknij prawym klawiszem by uzyska� informacje o sektorze.",
	L"Naci�nij w dowolnym momencie klawisz 'H' by wy�wietli� okienko pomocy.",
	L"Pr�bny tekst",
	L"Pr�bny tekst",
	L"Pr�bny tekst",
	L"Pr�bny tekst",
	L"Niewiele mo�esz tu zrobi�, dop�ki najemnicy nie przylec� do Arulco. Gdy ju� zbierzesz sw�j oddzia�, kliknij przycisk Kompresji Czasu, w prawym dolnym rogu. W ten spos�b twoi najemnicy szybciej dotr� na miejsce.",
};

// movement menu text

const wchar_t* pMovementMenuStrings[] =
{
	L"Przemie�� najemnik�w", 	// title for movement box
	L"Nanie� tras� podr�y", 		// done with movement menu, start plotting movement
	L"Anuluj", 		// cancel this menu
	L"Inni",		// title for group of mercs not on squads nor in vehicles
};


const wchar_t* pUpdateMercStrings[] =
{
	L"Oj:", 			// an error has occured
	L"Wygas� kontrakt najemnik�w:", 	// this pop up came up due to a merc contract ending
	L"Najemnicy wype�nili zadanie:", // this pop up....due to more than one merc finishing assignments
	L"Najemnicy wr�cili do pracy:", // this pop up ....due to more than one merc waking up and returing to work
	L"Odpoczywaj�cy najemnicy:", // this pop up ....due to more than one merc being tired and going to sleep
	L"Wkr�tce wygasn� kontrakty:", 	// this pop up came up due to a merc contract ending
};

// map screen map border buttons help text

const wchar_t* pMapScreenBorderButtonHelpText[] =
{
	L"Poka� miasta (|W)",
	L"Poka� kopalnie (|M)",
	L"Poka� oddzia�y i wrog�w (|T)",
	L"Poka� przestrze� powietrzn� (|A)",
	L"Poka� przedmioty (|I)",
	L"Poka� samoobron� i wrog�w (|Z)",
};


const wchar_t* pMapScreenBottomFastHelp[] =
{
	L"|Laptop",
	L"Ekran taktyczny (|E|s|c)",
	L"|Opcje",
	L"Kompresja czasu (|+)", 	// time compress more
	L"Kompresja czasu (|-)", 	// time compress less
	L"Poprzedni komunikat (|S|t|r|z|a|�|k|a |w |g|�|r|�)\nPoprzednia strona (|P|g|U|p)", 	// previous message in scrollable list
	L"Nast�pny komunikat (|S|t|r|z|a|�|k|a |w |d|�|�)\nNast�pna strona (|P|g|D|n)", 	// next message in the scrollable list
	L"W��cz/Wy��cz kompresj� czasu (|S|p|a|c|j|a)",	// start/stop time compression
};

const wchar_t* pMapScreenBottomText[] =
{
	L"Saldo dost�pne", 	// current balance in player bank account
};

const wchar_t* pMercDeadString[] =
{
	L"%ls nie �yje.",
};


const wchar_t* pDayStrings[] =
{
	L"Dzie�",
};

// the list of email sender names

const wchar_t* pSenderNameList[] =
{
	L"Enrico",
	L"Psych Pro Inc",
	L"Pomoc",
	L"Psych Pro Inc",
	L"Speck",
	L"R.I.S.",
	L"Barry",
	L"Blood",
	L"Lynx",
	L"Grizzly",
	L"Vicki",
	L"Trevor",
	L"Grunty",
	L"Ivan",
	L"Steroid",
	L"Igor",
	L"Shadow",
	L"Red",
	L"Reaper",
	L"Fidel",
	L"Fox",
	L"Sidney",
	L"Gus",
	L"Buns",
	L"Ice",
	L"Spider",
	L"Cliff",
	L"Bull",
	L"Hitman",
	L"Buzz",
	L"Raider",
	L"Raven",
	L"Static",
	L"Len",
	L"Danny",
	L"Magic",
	L"Stephan",
	L"Scully",
	L"Malice",
	L"Dr.Q",
	L"Nails",
	L"Thor",
	L"Scope",
	L"Wolf",
	L"MD",
	L"Meltdown",
	//----------
	L"M.I.S. Ubezpieczenia",
	L"Bobby Ray",
	L"Kingpin",
	L"John Kulba",
	L"A.I.M.",
};


// new mail notify string

const wchar_t* pNewMailStrings[] =
{
 L"Masz now� poczt�...",
};


// confirm player's intent to delete messages

const wchar_t* pDeleteMailStrings[] =
{
 L"Usun�� wiadomo��?",
 L"Usun�� wiadomo��?",
};


// the sort header strings

const wchar_t* pEmailHeaders[] =
{
 	L"Od:",
 	L"Temat:",
 	L"Dzie�:",
};

// email titlebar text

const wchar_t* pEmailTitleText[] =
{
 	L"Skrzynka odbiorcza",
};


// the financial screen strings
const wchar_t* pFinanceTitle[] =
{
	L"Ksi�gowy Plus",		//the name we made up for the financial program in the game
};

const wchar_t* pFinanceSummary[] =
{
	L"Wyp�ata:", 				// credit (subtract from) to player's account
	L"Wp�ata:", 				// debit (add to) to player's account
	L"Wczorajsze wp�ywy:",
	L"Wczorajsze dodatkowe wp�ywy:",
	L"Wczorajsze wydatki:",
	L"Saldo na koniec dnia:",
	L"Dzisiejsze wp�ywy:",
	L"Dzisiejsze dodatkowe wp�ywy:",
	L"Dzisiejsze wydatki:",
	L"Saldo dost�pne:",
	L"Przewidywane wp�ywy:",
	L"Przewidywane saldo:", 		// projected balance for player for tommorow
};


// headers to each list in financial screen

const wchar_t* pFinanceHeaders[] =
{
	L"Dzie�", 					// the day column
	L"Ma", 				// the credits column
	L"Winien",				// the debits column
	L"Transakcja", 			// transaction type - see TransactionText below
	L"Saldo", 				// balance at this point in time
	L"Strona", 				// page number
	L"Dzie� (dni)", 		// the day(s) of transactions this page displays
};


const wchar_t* pTransactionText[] =
{
	L"Naros�e odsetki",			// interest the player has accumulated so far
	L"Anonimowa wp�ata",
	L"Koszt transakcji",
	L"Wynaj�to -", 				// Merc was hired
	L"Zakupy u Bobby'ego Ray'a", 		// Bobby Ray is the name of an arms dealer
	L"Uregulowanie rachunk�w w M.E.R.C.",
	L"Zastaw na �ycie dla - %ls", 		// medical deposit for merc
	L"Analiza profilu w IMP", 		// IMP is the acronym for International Mercenary Profiling
	L"Ubezpieczneie dla - %ls",
	L"Redukcja ubezp. dla - %ls",
	L"Przed�. ubezp. dla - %ls", 				// johnny contract extended
	L"Anulowano ubezp. dla - %ls",
	L"Odszkodowanie za - %ls", 		// insurance claim for merc
	L"1 dzie�", 				// merc's contract extended for a day
	L"1 tydzie�", 				// merc's contract extended for a week
	L"2 tygodnie", 				// ... for 2 weeks
	L"Przych�d z kopalni",
	L"", //String nuked
	L"Zakup kwiat�w",
	L"Pe�ny zwrot zastawu za - %ls",
	L"Cz�ciowy zwrot zastawu za - %ls",
	L"Brak zwrotu zastawu za - %ls",
	L"Zap�ata dla - %ls",		// %ls is the name of the npc being paid
	L"Transfer funduszy do - %ls", 			// transfer funds to a merc
	L"Transfer funduszy od - %ls", 		// transfer funds from a merc
	L"Samoobrona w - %ls", // initial cost to equip a town's militia
	L"Zakupy u - %ls.",	//is used for the Shop keeper interface.  The dealers name will be appended to the end of the string.
	L"%ls wp�aci�(a) pieni�dze.",
};

const wchar_t* pTransactionAlternateText[] =
{
	L"Ubezpieczenie dla -", 				// insurance for a merc
	L"Przed�. kontrakt z - %ls o 1 dzie�.", 				// entend mercs contract by a day
	L"Przed�. kontrakt z - %ls o 1 tydzie�.",
	L"Przed�. kontrakt z - %ls o 2 tygodnie.",
};

// helicopter pilot payment

const wchar_t* pSkyriderText[] =
{
	L"Skyriderowi zap�acono %d$", 			// skyrider was paid an amount of money
	L"Skyriderowi trzeba jeszcze zap�aci� %d$", 		// skyrider is still owed an amount of money
	L"Skyrider zatankowa�",	// skyrider has finished refueling
	L"",//unused
	L"",//unused
	L"Skyrider jest got�w do kolejnego lotu.", // Skyrider was grounded but has been freed
	L"Skyrider nie ma pasa�er�w. Je�li chcesz przetransportowa� najemnik�w, zmie� ich przydzia� na POJAZD/HELIKOPTER.",
};


// strings for different levels of merc morale

const wchar_t* pMoralStrings[] =
{
 L"�wietne",
 L"Dobre",
 L"Stabilne",
 L"S�abe",
 L"Panika",
 L"Z�e",
};

// Mercs equipment has now arrived and is now available in Omerta or Drassen.

const wchar_t* pLeftEquipmentString[] =
{
	L"%ls - jego/jej sprz�t jest ju� w Omercie( A9 ).",
	L"%ls - jego/jej sprz�t jest ju� w Drassen( B13 ).",
};

// Status that appears on the Map Screen

const wchar_t* pMapScreenStatusStrings[] =
{
	L"Zdrowie",
	L"Energia",
	L"Morale",
	L"Stan",	// the condition of the current vehicle (its "health")
	L"Paliwo",	// the fuel level of the current vehicle (its "energy")
};


const wchar_t* pMapScreenPrevNextCharButtonHelpText[] =
{
	L"Poprzedni najemnik (|S|t|r|z|a|�|k|a |w |l|e|w|o)", 			// previous merc in the list
	L"Nast�pny najemnik (|S|t|r|z|a|�|k|a |w |p|r|a|w|o)", 				// next merc in the list
};


const wchar_t* pEtaString[] =
{
	L"PCP:", 				// eta is an acronym for Estimated Time of Arrival
};

const wchar_t* pTrashItemText[] =
{
	L"Wi�cej tego nie zobaczysz. Czy na pewno chcesz to zrobi�?", 	// do you want to continue and lose the item forever
	L"To wygl�da na co� NAPRAWD� wa�nego. Czy NA PEWNO chcesz to zniszczy�?", // does the user REALLY want to trash this item
};


const wchar_t* pMapErrorString[] =
{
	L"Oddzia� nie mo�e si� przemieszcza�, je�li kt�ry� z najemnik�w �pi.",

//1-5
	L"Najpierw wyprowad� oddzia� na powierzchni�.",
	L"Rozkazy przemieszczenia? To jest sektor wroga!",
	L"Aby podr�owa� najemnicy musz� by� przydzieleni do oddzia�u lub pojazdu.",
	L"Nie masz jeszcze ludzi.", 		// you have no members, can't do anything
	L"Najemnik nie mo�e wype�ni� tego rozkazu.",			 		// merc can't comply with your order
//6-10
	L"musi mie� eskort�, aby si� przemieszcza�. Umie�� go w oddziale z eskort�.", // merc can't move unescorted .. for a male
	L"musi mie� eskort�, aby si� przemieszcza�. Umie�� j� w oddziale z eskort�.", // for a female
	L"Najemnik nie przyby� jeszcze do Arulco!",
	L"Wygl�da na to, �e trzeba wpierw uregulowa� sprawy kontraktu.",
	L"",
//11-15
	L"Rozkazy przemieszczenia? Trwa walka!",
	L"Zaatakowa�y ci� dzikie koty, w sektorze %ls!",
	L"W sektorze I16 znajduje si� co�, co wygl�da na legowisko dzikich kot�w!",
	L"",
	L"Baza rakiet Ziemia-Powietrze zosta�a przej�ta.",
//16-20
	L"%ls - kopalnia zosta�a przej�ta. Tw�j dzienny przych�d zosta� zredukowany do %ls.",
	L"Nieprzyjaciel bezkonfliktowo przej�� sektor %ls.",
	L"Przynajmniej jeden z twoich najemnik�w nie zosta� do tego przydzielony.",
	L"%ls nie mo�e si� przy��czy�, poniewa� %ls jest pe�ny",
	L"%ls nie mo�e si� przy��czy�, poniewa� %ls jest zbyt daleko.",
//21-25
	L"%ls - kopalnia zosta�a przej�ta przez si�y Deidranny!",
	L"Si�y Deidranny w�a�nie zaatakowa�y baz� rakiet Ziemia-Powietrze w - %ls.",
	L"Si�y Deidranny w�a�nie zaatakowa�y - %ls.",
	L"W�a�nie zauwa�ono si�y Deidranny w - %ls.",
	L"Si�y Deidranny w�a�nie przej�y - %ls.",
//26-30
	L"Przynajmniej jeden z twoich najemnik�w nie m�g� si� po�o�y� spa�.",
	L"Przynajmniej jeden z twoich najemnik�w nie m�g� wsta�.",
	L"Oddzia�y samoobrony nie pojawi� si� dop�ki nie zostan� wyszkolone.",
	L"%ls nie mo�e si� w tej chwili przemieszcza�.",
	L"�o�nierze samoobrony, kt�rzy znajduj� si� poza granicami miasta, nie mog� by� przeniesieni do innego sektora.",
//31-35
	L"Nie mo�esz trenowa� samoobrony w - %ls.",
	L"Pusty pojazd nie mo�e si� porusza�!",
	L"%ls ma zbyt wiele ran by podr�owa�!",
	L"Musisz wpierw opu�ci� muzeum!",
	L"%ls nie �yje!",
//36-40
	L"%ls nie mo�e si� zamieni� z - %ls, poniewa� si� porusza",
	L"%ls nie mo�e w ten spos�b wej�c do pojazdu",
	L"%ls nie mo�e si� do��czy� do - %ls",
	L"Nie mo�esz kompresowa� czasu dop�ki nie zatrudnisz sobie kilku nowych najemnik�w!",
	L"Ten pojazd mo�e si� porusza� tylko po drodze!",
//41-45
	L"Nie mo�na zmienia� przydzia�u najemnik�w, kt�rzy s� w drodze",
	L"Pojazd nie ma paliwa!",
	L"%ls jest zbyt zm�czony(na) by podr�owa�.",
	L"�aden z pasa�er�w nie jest w stanie kierowa� tym pojazdem.",
	L"Jeden lub wi�cej cz�onk�w tego oddzia�u nie mo�e si� w tej chwili przemieszcza�.",
//46-50
	L"Jeden lub wi�cej INNYCH cz�onk�w tego oddzia�u nie mo�e si� w tej chwili przemieszcza�.",
	L"Pojazd jest uszkodzony!",
	L"Pami�taj, �e w jednym sektorze tylko dw�ch najemnik�w mo�e trenowa� �o�nierzy samoobrony.",
	L"Robot nie mo�e si� porusza� bez operatora. Umie�� ich razem w jednym oddziale.",
};


// help text used during strategic route plotting
const wchar_t* pMapPlotStrings[] =
{
	L"Kliknij ponownie sektor docelowy, aby zatwierdzi� tras� podr�y, lub kliknij inny sektor, aby j� wyd�u�y�.",
	L"Trasa podr�y zatwierdzona.",
	L"Cel podr�y nie zosta� zmieniony.",
	L"Trasa podr�y zosta�a anulowana.",
	L"Trasa podr�y zosta�a skr�cona.",
};


// help text used when moving the merc arrival sector
const wchar_t* pBullseyeStrings[] =
{
	L"Kliknij sektor, do kt�rego maj� przylatywa� najemnicy.",
	L"Dobrze. Przylatuj�cy najemnicy b�d� zrzucani w %ls",
	L"Najemnicy nie mog� tu przylatywa�. Przestrze� powietrzna nie jest zabezpieczona!",
	L"Anulowano. Sektor zrzutu nie zosta� zmieniony.",
	L"Przestrze� powietrzna nad %ls nie jest ju� bezpieczna! Sektor zrzutu zosta� przesuni�ty do %ls.",
};


// help text for mouse regions

const wchar_t* pMiscMapScreenMouseRegionHelpText[] =
{
	L"Otw�rz wyposa�enie (|E|n|t|e|r)",
	L"Zniszcz przedmiot",
	L"Zamknij wyposa�enie (|E|n|t|e|r)",
};



// male version of where equipment is left
const wchar_t* pMercHeLeaveString[] =
{
	L"Czy %ls ma zostawi� sw�j sprz�t w sektorze, w kt�rym si� obecnie znajduje (%ls), czy w Dressen (B13), sk�d odlatuje? ",
	L"Czy %ls ma zostawi� sw�j sprz�t w sektorze, w kt�rym si� obecnie znajduje (%ls), czy w Omercie (A9), sk�d odlatuje?",
	L"wkr�tce odchodzi i zostawi sw�j sprz�t w Omercie (A9).",
	L"wkr�tce odchodzi i zostawi sw�j sprz�t w Drassen (B13).",
	L"%ls wkr�tce odchodzi i zostawi sw�j sprz�t w %ls.",
};


// female version
const wchar_t* pMercSheLeaveString[] =
{
	L"Czy %ls ma zostawi� sw�j sprz�t w sektorze, w kt�rym si� obecnie znajduje (%ls), czy w Dressen (B13), sk�d odlatuje? ",
	L"Czy %ls ma zostawi� sw�j sprz�t w sektorze, w kt�rym si� obecnie znajduje (%ls), czy w Omercie (A9), sk�d odlatuje?",
	L"wkr�tce odchodzi i zostawi sw�j sprz�t w Omercie (A9).",
	L"wkr�tce odchodzi i zostawi sw�j sprz�t w Drassen (B13).",
	L"%ls wkr�tce odchodzi i zostawi sw�j sprz�t w %ls.",
};


const wchar_t* pMercContractOverStrings[] =
{
	L" zako�czy� kontrakt wi�c wyjecha�.", 		// merc's contract is over and has departed
	L" zako�czy�a kontrakt wi�c wyjecha�a.", 		// merc's contract is over and has departed
	L" - jego kontrakt zosta� zerwany wi�c odszed�.", 		// merc's contract has been terminated
	L" - jej kontrakt zosta� zerwany wi�c odesz�a.",		// merc's contract has been terminated
	L"Masz za du�y d�ug wobec M.E.R.C. wi�c %ls odchodzi.", // Your M.E.R.C. account is invalid so merc left
};

// Text used on IMP Web Pages

const wchar_t* pImpPopUpStrings[] =
{
	L"Nieprawid�owy kod dost�pu",
	L"Czy na pewno chcesz wznowi� proces okre�lenia profilu?",
	L"Wprowad� nazwisko oraz p�e�",
	L"Wst�pna kontrola stanu twoich finans�w wykaza�a, �e nie sta� ci� na analiz� profilu.",
	L"Opcja tym razem nieaktywna.",
	L"Aby wykona� profil, musisz mie� miejsce dla przynajmniej jednego cz�onka za�ogi.",
	L"Profil zosta� ju� wykonany.",
};


// button labels used on the IMP site

const wchar_t* pImpButtonText[] =
{
	L"O Nas", 			// about the IMP site
	L"ZACZNIJ", 			// begin profiling
	L"Osobowo��", 		// personality section
	L"Atrybuty", 		// personal stats/attributes section
	L"Portret", 			// the personal portrait selection
	L"G�os %d", 			// the voice selection
	L"Gotowe", 			// done profiling
	L"Zacznij od pocz�tku", 		// start over profiling
	L"Tak, wybieram t� odpowied�.",
	L"Tak",
	L"Nie",
	L"Sko�czone", 			// finished answering questions
	L"Poprz.", 			// previous question..abbreviated form
	L"Nast.", 			// next question
	L"TAK, JESTEM.", 		// yes, I am certain
	L"NIE, CHC� ZACZ�� OD NOWA.", // no, I want to start over the profiling process
	L"TAK",
	L"NIE",
	L"Wstecz", 			// back one page
	L"Anuluj", 			// cancel selection
	L"Tak.",
	L"Nie, Chc� spojrze� jeszcze raz.",
	L"Rejestr", 			// the IMP site registry..when name and gender is selected
	L"Analizuj�...", 			// analyzing your profile results
	L"OK",
	L"G�os",
};

const wchar_t* pExtraIMPStrings[] =
{
	L"Aby zacz�� analiz� profilu, wybierz osobowo��.",
	L"Teraz okre�l swoje atrybuty.",
	L"Teraz mo�esz przyst�pi� do wyboru portretu.",
	L"Aby zako�czy� proces, wybierz pr�bk� g�osu, kt�ra ci najbardziej odpowiada."
};

const wchar_t* pFilesTitle[] =
{
	L"Przegl�darka plik�w",
};

const wchar_t* pFilesSenderList[] =
{
	L"Raport Rozp.", 		// the recon report sent to the player. Recon is an abbreviation for reconissance
	L"Intercept #1", 		// first intercept file .. Intercept is the title of the organization sending the file...similar in function to INTERPOL/CIA/KGB..refer to fist record in files.txt for the translated title
	L"Intercept #2",	   // second intercept file
	L"Intercept #3",			 // third intercept file
	L"Intercept #4", // fourth intercept file
	L"Intercept #5", // fifth intercept file
	L"Intercept #6", // sixth intercept file
};

// Text having to do with the History Log

const wchar_t* pHistoryTitle[] =
{
	L"Historia",
};

const wchar_t* pHistoryHeaders[] =
{
	L"Dzie�", 			// the day the history event occurred
	L"Strona", 			// the current page in the history report we are in
	L"Dzie�", 			// the days the history report occurs over
	L"Po�o�enie", 			// location (in sector) the event occurred
	L"Zdarzenie", 			// the event label
};

// various history events
// THESE STRINGS ARE "HISTORY LOG" STRINGS AND THEIR LENGTH IS VERY LIMITED.
// PLEASE BE MINDFUL OF THE LENGTH OF THESE STRINGS. ONE WAY TO "TEST" THIS
// IS TO TURN "CHEAT MODE" ON AND USE CONTROL-R IN THE TACTICAL SCREEN, THEN
// GO INTO THE LAPTOP/HISTORY LOG AND CHECK OUT THE STRINGS. CONTROL-R INSERTS
// MANY (NOT ALL) OF THE STRINGS IN THE FOLLOWING LIST INTO THE GAME.
const wchar_t* pHistoryStrings[] =
{
	L"",																						// leave this line blank
	//1-5
	L"%ls naj�ty(ta) w A.I.M.", 										// merc was hired from the aim site
	L"%ls naj�ty(ta) w M.E.R.C.", 									// merc was hired from the aim site
	L"%ls ginie.", 															// merc was killed
	L"Uregulowano rachunki w M.E.R.C.",								// paid outstanding bills at MERC
	L"Przyj�to zlecenie od Enrico Chivaldori",
	//6-10
	L"Profil IMP wygenerowany",
	L"Podpisano umow� ubezpieczeniow� dla %ls.", 				// insurance contract purchased
	L"Anulowano umow� ubezpieczeniow� dla %ls.", 				// insurance contract canceled
	L"Wyp�ata ubezpieczenia za %ls.", 							// insurance claim payout for merc
	L"Przed�u�ono kontrakt z: %ls o 1 dzie�.", 						// Extented "mercs name"'s for a day
	//11-15
	L"Przed�u�ono kontrakt z: %ls o 1 tydzie�.", 					// Extented "mercs name"'s for a week
	L"Przed�u�ono kontrakt z: %ls o 2 tygodnie.", 					// Extented "mercs name"'s 2 weeks
	L"%ls zwolniony(na).", 													// "merc's name" was dismissed.
	L"%ls odchodzi.", 																		// "merc's name" quit.
	L"przyj�to zadanie.", 															// a particular quest started
	//16-20
	L"zadanie wykonane.",
	L"Rozmawiano szefem kopalni %ls",									// talked to head miner of town
	L"Wyzwolono - %ls",
	L"U�yto kodu Cheat",
	L"�ywno�� powinna by� jutro w Omercie",
	//21-25
	L"%ls odchodzi, aby wzi�� �lub z Darylem Hickiem",
	L"Wygas� kontrakt z - %ls.",
	L"%ls zrekrutowany(na).",
	L"Enrico narzeka na brak post�p�w",
	L"Walka wygrana",
	//26-30
	L"%ls - w kopalni ko�czy si� ruda",
	L"%ls - w kopalni sko�czy�a si� ruda",
	L"%ls - kopalnia zosta�a zamkni�ta",
	L"%ls - kopalnia zosta�a otwarta",
	L"Informacja o wi�zieniu zwanym Tixa.",
	//31-35
	L"Informacja o tajnej fabryce broni zwanej Orta.",
	L"Naukowiec w Orcie ofiarowa� kilka karabin�w rakietowych.",
	L"Kr�lowa Deidranna robi u�ytek ze zw�ok.",
	L"Frank opowiedzia� o walkach w San Monie.",
	L"Pewien pacjent twierdzi, �e widzia� co� w kopalni.",
	//36-40
	L"Go�� o imieniu Devin sprzedaje materia�y wybuchowe.",
	L"Spotkanie ze s�awynm eks-najemnikiem A.I.M. - Mike'iem!",
	L"Tony handluje broni�.",
	L"Otrzymano karabin rakietowy od sier�anta Krotta.",
	L"Dano Kyle'owi akt w�asno�ci sklepu Angela.",
	//41-45
	L"Madlab zaoferowa� si� zbudowa� robota.",
	L"Gabby potrafi zrobi� mikstur� chroni�c� przed robakami.",
	L"Keith wypad� z interesu.",
	L"Howard dostarcza� cyjanek kr�lowej Deidrannie.",
	L"Spotkanie z handlarzem Keithem w Cambrii.",
	//46-50
	L"Spotkanie z aptekarzem Howardem w Balime",
	L"Spotkanie z Perko, prowadz�cym ma�y warsztat.",
	L"Spotkanie z Samem z Balime - prowadzi sklep z narz�dziami.",
	L"Franz handluje sprz�tem elektronicznym.",
	L"Arnold prowadzi warsztat w Grumm.",
	//51-55
	L"Fredo naprawia sprz�t elektroniczny w Grumm.",
	L"Otrzymano darowizn� od bogatego go�cia w Balime.",
	L"Spotkano Jake'a, kt�ry prowadzi z�omowisko.",
	L"Jaki� w��cz�ga da� nam elektroniczn� kart� dost�pu.",
	L"Przekupiono Waltera, aby otworzy� drzwi do piwnicy.",
	//56-60
	L"Dave oferuje darmowe tankowania, je�li b�dzie mia� paliwo.",
	L"Greased Pablo's palms.",
	L"Kingpin trzyma pieni�dze w kopalni w San Mona.",
	L"%ls wygra�(a) walk�",
	L"%ls przegra�(a) walk�",
	//61-65
	L"%ls zdyskwalifikowany(na) podczas walki",
	L"Znaleziono du�o pieni�dzy w opuszczonej kopalni.",
	L"Spotkano zab�jc� nas�anego przez Kingpina.",
	L"Utrata kontroli nad sektorem",				//ENEMY_INVASION_CODE
	L"Sektor obroniony",
	//66-70
	L"Przegrana bitwa",							//ENEMY_ENCOUNTER_CODE
	L"Fatalna zasadzka",						//ENEMY_AMBUSH_CODE
	L"Usunieto zasadzk� wroga",
	L"Nieudany atak",			//ENTERING_ENEMY_SECTOR_CODE
	L"Udany atak!",
	//71-75
	L"Stworzenia zaatakowa�y",			//CREATURE_ATTACK_CODE
	L"Zabity(ta) przez dzikie koty",			//BLOODCAT_AMBUSH_CODE
	L"Wyr�ni�to dzikie koty",
	L"%ls zabity(ta)",
	L"Przekazano Carmenowi g�ow� terrorysty",
	L"Slay odszed�",
	L"Zabito: %ls",
};

const wchar_t* pHistoryLocations[] =
{
	L"N/D",						// N/A is an acronym for Not Applicable
};

// icon text strings that appear on the laptop

const wchar_t* pLaptopIcons[] =
{
	L"E-mail",
	L"Sie�",
	L"Finanse",
	L"Personel",
	L"Historia",
	L"Pliki",
	L"Zamknij",
	L"sir-FER 4.0",			// our play on the company name (Sirtech) and web surFER
};

// bookmarks for different websites
// IMPORTANT make sure you move down the Cancel string as bookmarks are being added

const wchar_t* pBookMarkStrings[] =
{
	L"A.I.M.",
	L"Bobby Ray's",
	L"I.M.P",
	L"M.E.R.C.",
	L"Pogrzeby",
	L"Kwiaty",
	L"Ubezpieczenia",
	L"Anuluj",
};

// When loading or download a web page

const wchar_t* pDownloadString[] =
{
	L"�adowanie strony...",
	L"Otwieranie strony...",
};

//This is the text used on the bank machines, here called ATMs for Automatic Teller Machine

const wchar_t* gsAtmStartButtonText[] =
{
	L"Atrybuty", 			// view stats of the merc
	L"Wyposa�enie", 			// view the inventory of the merc
	L"Zatrudnienie",
};

// Web error messages. Please use foreign language equivilant for these messages.
// DNS is the acronym for Domain Name Server
// URL is the acronym for Uniform Resource Locator

const wchar_t* pErrorStrings[] =
{
	L"Niestabilne po��czenie z Hostem. Transfer mo�e trwa� d�u�ej.",
};


const wchar_t* pPersonnelString[] =
{
	L"Najemnicy:", 			// mercs we have
};


const wchar_t* pWebTitle[ ]=
{
	L"sir-FER 4.0",		// our name for thL"sir-FER 4.0",		// our name for the version of the browser, play on company name
};


// The titles for the web program title bar, for each page loaded

const wchar_t* pWebPagesTitles[] =
{
	L"A.I.M.",
	L"A.I.M. Cz�onkowie",
	L"A.I.M. Portrety",		// a mug shot is another name for a portrait
	L"A.I.M. Lista",
	L"A.I.M.",
	L"A.I.M. Weterani",
	L"A.I.M. Polisy",
	L"A.I.M. Historia",
	L"A.I.M. Linki",
	L"M.E.R.C.",
	L"M.E.R.C. Konta",
	L"M.E.R.C. Rejestracja",
	L"M.E.R.C. Indeks",
	L"Bobby Ray's",
	L"Bobby Ray's - Bro�",
	L"Bobby Ray's - Amunicja",
	L"Bobby Ray's - Pancerz",
	L"Bobby Ray's - R�ne",							//misc is an abbreviation for miscellaneous
	L"Bobby Ray's - U�ywane",
	L"Bobby Ray's - Zam�wienie pocztowe",
	L"I.M.P.",
	L"I.M.P.",
	L"United Floral Service",
	L"United Floral Service - Galeria",
	L"United Floral Service - Zam�wienie",
	L"United Floral Service - Galeria kartek",
	L"Malleus, Incus & Stapes - Brokerzy ubezpieczeniowi",
	L"Informacja",
	L"Kontrakt",
	L"Uwagi",
	L"McGillicutty - Zak�ad pogrzebowy",
	L"",
	L"Nie odnaleziono URL.",
	L"Bobby Ray's - Ostatnie dostawy",
	L"",
	L"",
};

const wchar_t* pShowBookmarkString[] =
{
	L"Sir-Pomoc",
	L"Kliknij ponownie Sie� by otworzy� menu Ulubione.",
};

const wchar_t* pLaptopTitles[] =
{
	L"Poczta",
	L"Przegl�darka plik�w",
	L"Personel",
	L"Ksi�gowy Plus",
	L"Historia",
};

const wchar_t* pPersonnelDepartedStateStrings[] =
{
	//reasons why a merc has left.
	L"�mier� w akcji",
	L"Zwolnienie",
	L"Inny",
	L"Ma��e�stwo",
	L"Koniec kontraktu",
	L"Rezygnacja",
};
// personnel strings appearing in the Personnel Manager on the laptop

const wchar_t* pPersonelTeamStrings[] =
{
	L"Bie��cy oddzia�",
	L"Wyjazdy",
	L"Koszt dzienny:",
	L"Najwy�szy koszt:",
	L"Najni�szy koszt:",
	L"�mier� w akcji:",
	L"Zwolnienie:",
	L"Inny:",
};


const wchar_t* pPersonnelCurrentTeamStatsStrings[] =
{
	L"Najni�szy",
	L"�redni",
	L"Najwy�szy",
};


const wchar_t* pPersonnelTeamStatsStrings[] =
{
	L"ZDR",
	L"ZWN",
	L"ZRCZ",
	L"SI�A",
	L"DOW",
	L"INT",
	L"DO�W",
	L"STRZ",
	L"MECH",
	L"WYB",
	L"MED",
};


// horizontal and vertical indices on the map screen

const wchar_t* pMapVertIndex[] =
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

const wchar_t* pMapHortIndex[] =
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

const wchar_t* pMapDepthIndex[] =
{
	L"",
	L"-1",
	L"-2",
	L"-3",
};

// text that appears on the contract button

const wchar_t* pContractButtonString[] =
{
	L"Kontrakt",
};

// text that appears on the update panel buttons

const wchar_t* pUpdatePanelButtons[] =
{
	L"Dalej",
	L"Stop",
};

// Text which appears when everyone on your team is incapacitated and incapable of battle

const wchar_t LargeTacticalStr[][ LARGE_STRING_LENGTH ] =
{
	L"Pokonano ci� w tym sektorze!",
	L"Wr�g nie zna lito�ci i po�era was wszystkich!",
	L"Nieprzytomni cz�onkowie twojego oddzia�u zostali pojmani!",
	L"Cz�onkowie twojego oddzia�u zostali uwi�zieni.",
};


//Insurance Contract.c
//The text on the buttons at the bottom of the screen.

const wchar_t* InsContractText[] =
{
	L"Wstecz",
	L"Dalej",
	//L"Akceptuj�",
	L"OK",
	L"Skasuj",
};



//Insurance Info
// Text on the buttons on the bottom of the screen

const wchar_t* InsInfoText[] =
{
	L"Wstecz",
	L"Dalej"
};



//For use at the M.E.R.C. web site. Text relating to the player's account with MERC

const wchar_t* MercAccountText[] =
{
	// Text on the buttons on the bottom of the screen
	L"Autoryzacja",
	L"Strona g��wna",
	L"Konto #:",
	L"Najemnik",
	L"Dni",
	L"Stawka",	//5
	L"Op�ata",
	L"Razem:",
	L"Czy na pewno chcesz zatwierdzi� p�atno��: %ls?",		//the %ls is a string that contains the dollar amount ( ex. "$150" )
};



//For use at the M.E.R.C. web site. Text relating a MERC mercenary


const wchar_t* MercInfo[] =
{
	L"Zdrowie",
	L"Zwinno��",
	L"Sprawno��",
	L"Si�a",
	L"Um. dowodz.",
	L"Inteligencja",
	L"Poz. do�wiadczenia",
	L"Um. strzeleckie",
	L"Zn. mechaniki",
	L"Mat. wybuchowe",
	L"Wiedza medyczna",

	L"Poprzedni",
	L"Najmij",
	L"Nast�pny",
	L"Dodatkowe informacje",
	L"Strona g��wna",
	L"Naj�ty",
	L"Koszt:",
	L"Dziennie",
	L"Nie �yje",

	L"Wygl�da na to, �e chcesz wynaj�� zbyt wielu najemnik�w. Limit wynosi 18.",
	L"Niedost�pny",
};



// For use at the M.E.R.C. web site. Text relating to opening an account with MERC

const wchar_t* MercNoAccountText[] =
{
	//Text on the buttons at the bottom of the screen
	L"Otw�rz konto",
	L"Anuluj",
	L"Nie posiadasz konta. Czy chcesz sobie za�o�y�?"
};



// For use at the M.E.R.C. web site. MERC Homepage

const wchar_t* MercHomePageText[] =
{
	//Description of various parts on the MERC page
	L"Speck T. Kline, za�o�yciel i w�a�ciciel",
	L"Aby otworzy� konto naci�nij tu",
	L"Aby zobaczy� konto naci�nij tu",
	L"Aby obejrze� akta naci�nij tu",
	// The version number on the video conferencing system that pops up when Speck is talking
	L"Speck Com v3.2",
};

// For use at MiGillicutty's Web Page.

const wchar_t* sFuneralString[] =
{
	L"Zak�ad pogrzebowy McGillicutty, pomaga rodzinom pogr��onym w smutku od 1983.",
	L"Kierownik, by�y najemnik A.I.M. Murray \'Pops\' McGillicutty jest do�wiadczonym pracownikiem zak�adu pogrzebowego.",
	L"Przez ca�e �ycie obcowa� ze �mierci�, 'Pops' wie jak trudne s� te chwile.",
	L"Zak�ad pogrzebowy McGillicutty oferuje szeroki zakres us�ug, od duchowego wsparcia po rekonstrukcj� silnie zniekszta�conych zw�ok.",
	L"Pozw�l by McGillicutty ci pom�g� a tw�j ukochany b�dzie spoczywa� w pokoju.",

	// Text for the various links available at the bottom of the page
	L"WY�LIJ KWIATY",
	L"KOLEKCJA TRUMIEN I URN",
	L"US�UGI KREMA- CYJNE",
	L"US�UGI PLANOWANIA POGRZEBU",
	L"KARTKI POGRZE- BOWE",

	// The text that comes up when you click on any of the links ( except for send flowers ).
	L"Niestety, z powodu �mierci w rodzinie, nie dzia�aj� jeszcze wszystkie elementy tej strony.",
	L"Przepraszamy za powy�sze uniedogodnienie."
};

// Text for the florist Home page

const wchar_t* sFloristText[] =
{
	//Text on the button on the bottom of the page

	L"Galeria",

	//Address of United Florist

	L"\"Zrzucamy z samolotu w dowolnym miejscu\"",
	L"1-555-POCZUJ-MNIE",
	L"Ul. Nosowska 333, Zapuszczone miasto, CA USA 90210",
	L"http://www.poczuj-mnie.com",

	// detail of the florist page

	L"Dzia�amy szybko i sprawnie!",
	L"Gwarantujemy dostaw� w dowolny punkt na Ziemi, nast�pnego dnia po z�o�eniu zam�wienia!",
	L"Oferujemy najni�sze ceny na �wiecie!",
	L"Poka� nam ofert� z ni�sz� cen�, a dostaniesz w nagrod� tuzin r�, za darmo!",
	L"Lataj�ca flora, fauna i kwiaty od 1981.",
	L"Nasz ozdobiony bombowiec zrzuci tw�j bukiet w promieniu co najwy�ej dziesi�ciu mil od ��danego miejsca. Kiedy tylko zechcesz!",
	L"Pozw�l nam zaspokoi� twoje kwieciste fantazje.",
	L"Bruce, nasz �wiatowej renomy projektant bukiet�w, zerwie dla ciebie naj�wie�sze i najwspanialsze kwiaty z naszej szklarni.",
	L"I pami�taj, je�li czego� nie mamy, mo�emy to szybko zasadzi�!"
};



//Florist OrderForm

const wchar_t* sOrderFormText[] =
{
	//Text on the buttons

	L"Powr�t",
	L"Wy�lij",
	L"Skasuj",
	L"Galeria",

	L"Nazwa bukietu:",
	L"Cena:",			//5
	L"Zam�wienie numer:",
	L"Czas dostawy",
	L"nast. dnia",
	L"dostawa gdy to b�dzie mo�liwe",
	L"Miejsce dostawy",			//10
	L"Dodatkowe us�ugi",
	L"Zgnieciony bukiet($10)",
	L"Czarne R�e($20)",
	L"Zwi�dni�ty bukiet($10)",
	L"Ciasto owocowe (je�eli b�dzie)($10)",		//15
	L"Osobiste kondolencje:",
	L"Ze wzgl�du na rozmiar karteczek, tekst nie mo�e zawiera� wi�cej ni� 75 znak�w.",
	L"...mo�esz te� przejrze� nasze",

	L"STANDARDOWE KARTKI",
	L"Informacja o rachunku",//20

	//The text that goes beside the area where the user can enter their name

	L"Nazwisko:",
};




//Florist Gallery.c

const wchar_t* sFloristGalleryText[] =
{
	//text on the buttons

	L"Poprz.",	//abbreviation for previous
	L"Nast.",	//abbreviation for next

	L"Kliknij wybran� pozycj� aby z�o�y� zam�wienie.",
	L"Uwaga: $10 dodatkowej op�aty za zwi�dni�ty lub zgnieciony bukiet.",

	//text on the button

	L"G��wna",
};

//Florist Cards

const wchar_t* sFloristCards[] =
{
	L"Kliknij sw�j wyb�r",
	L"Wstecz"
};



// Text for Bobby Ray's Mail Order Site

const wchar_t* BobbyROrderFormText[] =
{
	L"Formularz zam�wienia",				//Title of the page
	L"Ilo��",					// The number of items ordered
	L"Waga (%ls)",			// The weight of the item
	L"Nazwa",				// The name of the item
	L"Cena",				// the item's weight
	L"Warto��",				//5	// The total price of all of items of the same type
	L"W sumie",				// The sub total of all the item totals added
	L"Transport",		// S&H is an acronym for Shipping and Handling
	L"Razem",			// The grand total of all item totals + the shipping and handling
	L"Miejsce dostawy",
	L"Czas dostawy",			//10	// See below
	L"Koszt (za %ls.)",			// The cost to ship the items
	L"Ekspres - 24h",			// Gets deliverd the next day
	L"2 dni robocze",			// Gets delivered in 2 days
	L"Standardowa dostawa",			// Gets delivered in 3 days
	L" Wyczy��",//15			// Clears the order page
	L" Akceptuj�",			// Accept the order
	L"Wstecz",				// text on the button that returns to the previous page
	L"Strona g��wna",				// Text on the button that returns to the home page
	L"* oznacza u�ywane rzeczy",		// Disclaimer stating that the item is used
	L"Nie sta� ci� na to.",		//20	// A popup message that to warn of not enough money
	L"<BRAK>",				// Gets displayed when there is no valid city selected
	L"Miejsce docelowe przesy�ki: %ls. Potwierdzasz?",		// A popup that asks if the city selected is the correct one
	L"Waga przesy�ki*",			// Displays the weight of the package
	L"* Min. Waga",				// Disclaimer states that there is a minimum weight for the package
	L"Dostawy",
};


// This text is used when on the various Bobby Ray Web site pages that sell items

const wchar_t* BobbyRText[] =
{
	L"Zam�w",				// Title

	L"Kliknij wybrane towary. Lewym klawiszem zwi�kszasz ilo�� towaru, a prawym zmniejszasz. Gdy ju� skompletujesz swoje zakupy przejd� do formularza zam�wienia.",			// instructions on how to order

	//Text on the buttons to go the various links

	L"Poprzednia",		//
	L"Bro�", 			//3
	L"Amunicja",			//4
	L"Ochraniacze",			//5
	L"R�ne",			//6	//misc is an abbreviation for miscellaneous
	L"U�ywane",			//7
	L"Nast�pna",
	L"FORMULARZ",
	L"Strona g��wna",			//10

	//The following 2 lines are used on the Ammunition page.
	//They are used for help text to display how many items the player's merc has
	//that can use this type of ammo

	L"Tw�j zesp� posiada",//11
	L"szt. broni do kt�rej pasuje amunicja tego typu", //12

	//The following lines provide information on the items

	L"Waga:",			// Weight of all the items of the same type
	L"Kal:",			// the caliber of the gun
	L"Mag:",			// number of rounds of ammo the Magazine can hold
	L"Zas:",				// The range of the gun
	L"Si�a:",				// Damage of the weapon
	L"CS:",			// Weapon's Rate Of Fire, acroymn ROF
	L"Koszt:",			// Cost of the item
	L"Na stanie:",			// The number of items still in the store's inventory
	L"Ilo�� na zam�w.:",		// The number of items on order
	L"Uszkodz.",			// If the item is damaged
	L"Waga:",			// the Weight of the item
	L"Razem:",			// The total cost of all items on order
	L"* Stan: %%",		// if the item is damaged, displays the percent function of the item

	//Popup that tells the player that they can only order 10 items at a time

	L"Przepraszamy za to utrudnienie, ale na jednym zam�wieniu mo�e si� znajdowa� tylko 10 pozycji! Je�li potrzebujesz wi�cej, z�� kolejne zam�wienie.",

	// A popup that tells the user that they are trying to order more items then the store has in stock

	L"Przykro nam. Chwilowo nie mamy tego wi�cej na magazynie. Prosz� spr�bowa� p�niej.",

	//A popup that tells the user that the store is temporarily sold out

	L"Przykro nam, ale chwilowo nie mamy tego towaru na magazynie",

};


// Text for Bobby Ray's Home Page

const wchar_t* BobbyRaysFrontText[] =
{
	//Details on the web site

	L"Tu znajdziesz nowo�ci z dziedziny broni i osprz�tu wojskowego",
	L"Zaspokoimy wszystkie twoje potrzeby w dziedzinie materia��w wybuchowych",
	L"U�YWANE RZECZY",

	//Text for the various links to the sub pages

	L"RӯNE",
	L"BRO�",
	L"AMUNICJA",		//5
	L"OCHRANIACZE",

	//Details on the web site

	L"Je�li MY tego nie mamy, to znaczy, �e nigdzie tego nie dostaniesz!",
	L"W trakcie budowy",
};



// Text for the AIM page.
// This is the text used when the user selects the way to sort the aim mercanaries on the AIM mug shot page

const wchar_t* AimSortText[] =
{
	L"Cz�onkowie A.I.M.",				// Title

	L"Sortuj wg:",					// Title for the way to sort

	// sort by...

	L"Ceny",
	L"Do�wiadczenia",
	L"Um. strzeleckich",
	L"Um. med.",
	L"Zn. mat. wyb.",
	L"Zn. mechaniki",

	//Text of the links to other AIM pages

	L"Portrety najemnik�w",
	L"Akta najemnika",
	L"Poka� galeri� by�ych cz�onk�w A.I.M.",

	// text to display how the entries will be sorted

	L"Rosn�co",
	L"Malej�co",
};


//Aim Policies.c
//The page in which the AIM policies and regulations are displayed

const wchar_t* AimPolicyText[] =
{
	// The text on the buttons at the bottom of the page

	L"Poprzednia str.",
	L"Strona g��wna",
	L"Przepisy",
	L"Nast�pna str.",
	L"Rezygnuj�",
	L"Akceptuj�",
};



//Aim Member.c
//The page in which the players hires AIM mercenaries

// Instructions to the user to either start video conferencing with the merc, or to go the mug shot index

const wchar_t* AimMemberText[] =
{
	L"Lewy klawisz myszy",
	L"kontakt z najemnikiem",
	L"Prawy klawisz myszy",
	L"lista portret�w",
};

//Aim Member.c
//The page in which the players hires AIM mercenaries

const wchar_t* CharacterInfo[] =
{
	// The various attributes of the merc

	L"Zdrowie",
	L"Zwinno��",
	L"Sprawno��",
	L"Si�a",
	L"Um. dowodzenia",
	L"Inteligencja",
	L"Poziom do�w.",
	L"Um. strzeleckie",
	L"Zn. mechaniki",
	L"Zn. mat. wyb.",
	L"Wiedza med.",				//10

	// the contract expenses' area

	L"Zap�ata",
	L"Czas",
	L"1 dzie�",
	L"1 tydzie�",
	L"2 tygodnie",

	// text for the buttons that either go to the previous merc,
	// start talking to the merc, or go to the next merc

	L"Poprzedni",
	L"Kontakt",
	L"Nast�pny",

	L"Dodatkowe informacje",				// Title for the additional info for the merc's bio
	L"Aktywni cz�onkowie",		//20		// Title of the page
	L"Opcjonalne wyposa�enie:",				// Displays the optional gear cost
	L"Wymagany jest zastaw na �ycie",			// If the merc required a medical deposit, this is displayed
};


//Aim Member.c
//The page in which the player's hires AIM mercenaries

//The following text is used with the video conference popup

const wchar_t* VideoConfercingText[] =
{
	L"Warto�� kontraktu:",				//Title beside the cost of hiring the merc

	//Text on the buttons to select the length of time the merc can be hired

	L"Jeden dzie�",
	L"Jeden tydzie�",
	L"Dwa tygodnie",

	//Text on the buttons to determine if you want the merc to come with the equipment

	L"Bez sprz�tu",
	L"We� sprz�t",

	// Text on the Buttons

	L"TRANSFER",			// to actually hire the merc
	L"ANULUJ",				// go back to the previous menu
	L"WYNAJMIJ",				// go to menu in which you can hire the merc
	L"ROZ��CZ",				// stops talking with the merc
	L"OK",
	L"NAGRAJ SI�",			// if the merc is not there, you can leave a message

	//Text on the top of the video conference popup

	L"Wideo konferencja z - ",
	L"��cz�. . .",

	L"z zastawem"			// Displays if you are hiring the merc with the medical deposit
};



//Aim Member.c
//The page in which the player hires AIM mercenaries

// The text that pops up when you select the TRANSFER FUNDS button

const wchar_t* AimPopUpText[] =
{
	L"TRANSFER ZAKO�CZONY POMY�LNIE",	// You hired the merc
	L"PRZEPROWADZENIE TRANSFERU NIE MO�LIWE",		// Player doesn't have enough money, message 1
	L"BRAK �RODK�W",				// Player doesn't have enough money, message 2

	// if the merc is not available, one of the following is displayed over the merc's face

	L"Wynaj�to",
	L"Prosz� zostaw wiadomo��",
	L"Nie �yje",

	//If you try to hire more mercs than game can support

	L"Masz ju� pe�ny zesp� 18 najemnik�w.",

	L"Nagrana wiadomo��",
	L"Wiadomo�� zapisana",
};


//AIM Link.c

const wchar_t* AimLinkText[] =
{
	L"A.I.M. Linki",	//The title of the AIM links page
};



//Aim History

// This page displays the history of AIM

const wchar_t* AimHistoryText[] =
{
	L"A.I.M. Historia",					//Title

	// Text on the buttons at the bottom of the page

	L"Poprzednia str.",
	L"Strona g��wna",
	L"Byli cz�onkowie",
	L"Nast�pna str."
};


//Aim Mug Shot Index

//The page in which all the AIM members' portraits are displayed in the order selected by the AIM sort page.

const wchar_t* AimFiText[] =
{
	// displays the way in which the mercs were sorted

	L"ceny",
	L"do�wiadczenia",
	L"um. strzeleckich",
	L"um. medycznych",
	L"zn. materia��w wyb.",
	L"zn. mechaniki",

	// The title of the page, the above text gets added at the end of this text

	L"Cz�onkowie A.I.M. posortowani rosn�co wg %ls",
	L"Cz�onkowie A.I.M. posortowani malej�co wg %ls",

	// Instructions to the players on what to do

	L"Lewy klawisz",
	L"Wyb�r najemnika",			//10
	L"Prawy klawisz",
	L"Opcje sortowania",

	// Gets displayed on top of the merc's portrait if they are...

	L"Wyjecha�(a)",
	L"Nie �yje",						//14
	L"Wynaj�to",
};



//AimArchives.
// The page that displays information about the older AIM alumni merc... mercs who are no longer with AIM

const wchar_t* AimAlumniText[] =
{

	L"STRONA 1",
	L"STRONA 2",
	L"STRONA 3",

	L"Byli cz�onkowie A.I.M.",	// Title of the page


	L"OK"			// Stops displaying information on selected merc
};






//AIM Home Page

const wchar_t* AimScreenText[] =
{
	// AIM disclaimers

	L"Znaki A.I.M. i logo A.I.M. s� prawnie chronione w wi�kszo�ci kraj�w.",
	L"Wi�c nawet nie my�l o pr�bie ich podrobienia.",
	L"Copyright 1998-1999 A.I.M., Ltd. All rights reserved.",

	//Text for an advertisement that gets displayed on the AIM page

	L"United Floral Service",
	L"\"Zrzucamy gdziekolwiek\"",				//10
	L"Zr�b to jak nale�y...",
	L"...za pierwszym razem",
	L"Bro� i akcesoria, je�li czego� nie mamy, to tego nie potrzebujesz.",
};


//Aim Home Page

const wchar_t* AimBottomMenuText[] =
{
	//Text for the links at the bottom of all AIM pages
	L"Strona g��wna",
	L"Cz�onkowie",
	L"Byli cz�onkowie",
	L"Przepisy",
	L"Historia",
	L"Linki",
};



//ShopKeeper Interface
// The shopkeeper interface is displayed when the merc wants to interact with
// the various store clerks scattered through out the game.

const wchar_t* SKI_Text[ ] =
{
	L"TOWARY NA STANIE",		//Header for the merchandise available
	L"STRONA",				//The current store inventory page being displayed
	L"KOSZT OGӣEM",				//The total cost of the the items in the Dealer inventory area
	L"WARTO�� OGӣEM",			//The total value of items player wishes to sell
	L"WYCENA",				//Button text for dealer to evaluate items the player wants to sell
	L"TRANSAKCJA",			//Button text which completes the deal. Makes the transaction.
	L"OK",				//Text for the button which will leave the shopkeeper interface.
	L"KOSZT NAPRAWY",			//The amount the dealer will charge to repair the merc's goods
	L"1 GODZINA",			// SINGULAR! The text underneath the inventory slot when an item is given to the dealer to be repaired
	L"%d GODZIN(Y)",		// PLURAL!   The text underneath the inventory slot when an item is given to the dealer to be repaired
	L"NAPRAWIONO",		// Text appearing over an item that has just been repaired by a NPC repairman dealer
	L"Brak miejsca by zaoferowa� wi�cej rzeczy.",	//Message box that tells the user there is no more room to put there stuff
	L"%d MINUT(Y)",		// The text underneath the inventory slot when an item is given to the dealer to be repaired
	L"Upu�� przedmiot na ziemi�.",
};


const wchar_t* SkiMessageBoxText[] =
{
	L"Czy chcesz do�o�y� %ls ze swojego konta, aby pokry� r�nic�?",
	L"Brak �rodk�w. Brakuje ci %ls",
	L"Czy chcesz przeznaczy� %ls ze swojego konta, aby pokry� koszty?",
	L"Popro� o rozpocz�cie transakscji",
	L"Popro� o napraw� wybranych przedmiot�w",
	L"Zako�cz rozmow�",
	L"Saldo dost�pne",
};


//OptionScreen.c

const wchar_t* zOptionsText[] =
{
	//button Text
	L"Zapisz gr�",
	L"Odczytaj gr�",
	L"Wyj�cie",
	L"OK",

	//Text above the slider bars
	L"Efekty",
	L"Dialogi",
	L"Muzyka",

	//Confirmation pop when the user selects..
	L"Zako�czy� gr� i wr�ci� do g��wnego menu?",

	L"Musisz w��czy� opcj� dialog�w lub napis�w.",
};


//SaveLoadScreen
const wchar_t* zSaveLoadText[] =
{
	L"Zapisz gr�",
	L"Odczytaj gr�",
	L"Anuluj",
	L"Zapisz wybran�",
	L"Odczytaj wybran�",

	L"Gra zosta�a pomy�lnie zapisana",
	L"B��D podczas zapisu gry!",
	L"Gra zosta�a pomy�lnie odczytana",
	L"B��D podczas odczytu gry!",

	L"Wersja gry w zapisanym pliku r�ni si� od bie��cej. Prawdopodobnie mo�na bezpiecznie kontynuowa�. Kontynuowa�?",
	L"Zapisane pliki gier mog� by� uszkodzone. Czy chcesz je usun��?",

	//Translators, the next two strings are for the same thing.  The first one is for beta version releases and the second one
	//is used for the final version.  Please don't modify the "#ifdef JA2BETAVERSION" or the "#else" or the "#endif" as they are
	//used by the compiler and will cause program errors if modified/removed.  It's okay to translate the strings though.
#ifdef JA2BETAVERSION
	L"Nieprawid�owa wersja zapisu gry.  W razie problem�w prosimy o raport.  Kontynuowa�?",
#else
	L"Pr�ba odczytu starszej wersji zapisu gry.  Zaktualizowa� ten zapis i odczyta� gr�?",
#endif

	//Translators, the next two strings are for the same thing.  The first one is for beta version releases and the second one
	//is used for the final version.  Please don't modify the "#ifdef JA2BETAVERSION" or the "#else" or the "#endif" as they are
	//used by the compiler and will cause program errors if modified/removed.  It's okay to translate the strings though.
#ifdef JA2BETAVERSION
	L"Nieprawid�owa wersja zapisu gry.  W razie problem�w prosimy o raport.  Kontynuowa�?",
#else
	L"Pr�ba odczytu starszej wersji zapisu gry.  Zaktualizowa� ten zapis i odczyta� gr�?",
#endif

	L"Czy na pewno chcesz nadpisa� gr� na pozycji %d?",
	L"Chcesz odczyta� gr� z pozycji",


	//The first %d is a number that contains the amount of free space on the users hard drive,
	//the second is the recommended amount of free space.
	L"Brak miejsca na dysku twardym.  Na dysku wolne jest %d MB, a wymagane jest przynajmniej %d MB.",

	L"Zapisuj�...",			//When saving a game, a message box with this string appears on the screen

	L"Standardowe uzbrojenie",
	L"Ca�e mn�stwo broni",
	L"Realistyczna gra",
	L"Elementy S-F",

	L"Stopie� trudno�ci",
};



//MapScreen
const wchar_t* zMarksMapScreenText[] =
{
	L"Poziom mapy",
	L"Nie masz jeszcze �o�nierzy samoobrony.  Musisz najpierw wytrenowa� mieszka�c�w miast.",
	L"Dzienny przych�d",
	L"Najmemnik ma polis� ubezpieczeniow�",
    L"%ls nie potrzebuje snu.",
	L"%ls jest w drodze i nie mo�e spa�",
	L"%ls jest zbyt zm�czony(na), spr�buj troch� p�niej.",
	L"%ls prowadzi.",
	L"Oddzia� nie mo�e si� porusza� je�eli jeden z najemnik�w �pi.",

	// stuff for contracts
	L"Mimo, �e mo�esz op�aci� kontrakt, to jednak nie masz got�wki by op�aci� sk�adk� ubezpieczeniow� za najemnika.",
	L"%ls - sk�adka ubezpieczeniowa najemnika b�dzie kosztowa� %ls za %d dzie�(dni). Czy chcesz j� op�aci�?",
	L"Inwentarz sektora",
	L"Najemnik posiada zastaw na �ycie.",

	// other items
	L"Lekarze", // people acting a field medics and bandaging wounded mercs // **************************************NEW******** as of July 09, 1998
	L"Pacjenci", // people who are being bandaged by a medic // ****************************************************NEW******** as of July 10, 1998
	L"Gotowe", // Continue on with the game after autobandage is complete
	L"Przerwij", // Stop autobandaging of patients by medics now
	L"Przykro nam, ale ta opcja jest wy��czona w wersji demo.", // informs player this option/button has been disabled in the demo
	L"%ls nie ma zestawu narz�dzi.",
	L"%ls nie ma apteczki.",
	L"Brak ch�tnych ludzi do szkolenia, w tej chwili.",
	L"%ls posiada ju� maksymaln� liczb� oddzia��w samoobrony.",
	L"Najemnik ma kontrakt na okre�lony czas.",
  L"Kontrakt najemnika nie jest ubezpieczony",
};


const wchar_t* pLandMarkInSectorString[] =
{
	L"Oddzia� %d zauwa�y� kogo� w sektorze %ls",
};

// confirm the player wants to pay X dollars to build a militia force in town
const wchar_t* pMilitiaConfirmStrings[] =
{
	L"Szkolenie oddzia�u samoobrony b�dzie kosztowa�o $", // telling player how much it will cost
	L"Zatwierdzasz wydatek?", // asking player if they wish to pay the amount requested
	L"Nie sta� ci� na to.", // telling the player they can't afford to train this town
	L"Kontynuowa� szkolenie samoobrony w - %ls (%ls %d)?", // continue training this town?
	L"Koszt $", // the cost in dollars to train militia
	L"( T/N )",   // abbreviated yes/no
	L"",	// unused
	L"Szkolenie samoobrony w %d sektorach b�dzie kosztowa�o $ %d. %ls", // cost to train sveral sectors at once
	L"Nie masz %d$, aby wyszkoli� samoobron� w tym mie�cie.",
	L"%ls musi mie� %d% lojalno�ci, aby mo�na by�o kontynuowa� szkolenie samoobrony.",
	L"Nie mo�esz ju� d�u�ej szkoli� samoobrony w mie�cie %ls.",
};

#ifdef JA2DEMOADS
const wchar_t* gpDemoString[] =
{
	//0-9
	L"NAJLEPSZA GRA STRATEGICZNA.",
	L"NAJLEPSZA GRA ROLE-PLAYING.",
	L"Nieliniowy scenariusz gry",
	L"Absolutnie nowy pomys�",
	L"Wspania�e strzelaniny",
	L"Dynamiczne o�wietlenie",
	L"Rozmowy z postaciami",
	L"Nocne akcje",
	L"Kreowanie w�asnego najemnika",
	L"Ambitne i wci�gaj�ce misje",
	L"Sterowanie pojazdami",
	//10-19
	L"Ponad 150 r�nych postaci",
	L"Setki ciekawych przedmiot�w",
	L"Ponad 250 r�nych plener�w",
	L"Efektowne eksplozje",
	L"Nowe taktyczne manewry",
	L"Tony animacji",
	L"Ponad 9000 kwestii dialogowych",
	L"Niesamowity podk�ad muzyczny Kevina Manthei",
	L"Globalna strategia",
	L"Naloty",
	//20-29
	L"Szkolenie oddzia��w samoobrony",
	L"Mo�liwo�� dostosowania scenariusza gry",
	L"Realistyczne tereny",
	L"Efektowna grafika high-color",
	L"Kupno, sprzeda� i handel",
	L"Bezwzgl�dny przeciwnik AI",
	L"�wietna walka taktyczna",
	L"Opcjonalny element S-F",
	L"24 godzinny zegar",
	L"Minimum 60 godzin gry",
	//30-34
	L"R�ne poziomy trudno�ci",
	L"Realistycznie opracowana balistyka",
	L"Wierne odwzorowanie zasad fizyki",
	L"W pe�ni interaktywny interfejs",
	L"I wiele, wiele wi�cej...",
	//35 on are BOLD
	L"",
	L"",
	L"Jedna z najbardziej pomys�owych gier 1998 roku",
	L"PC Gamer",
	L"Zam�w ju� dzisiaj na stronie WWW.TOPWARE.PL !",
};

const wchar_t* gpDemoIntroString[] =
{
	L"Za chwil� spr�bujesz najlepszej strategii, role-playing i najwspanialszej walki taktycznej:",
	//Point 1 (uses one string)
	L"Kontrola nad grup� najemnik�w o bogatej osobowo�ci   (�adnych nudnych i zb�dnych postaci).",
	//Point 2 (uses one string)
	L"Sprawd� wspania�e manewry taktyczne, pocz�wszy od biegu do czo�gania si�, wspinania si�, skakania, walki wr�cz i wielu innych.",
	//Point 3 (uses one string)
	L"Igraj z ogniem! Fajne zabawki, �mierciono�na bro�, efektowne eksplozje.",
	//Additional comment
	L"(a to dopiero pocz�tek...)",
	//Introduction/instructions
	L"Witamy w Demoville... (naci�nij dowolny klawisz)",
};
#endif

//Strings used in the popup box when withdrawing, or depositing money from the $ sign at the bottom of the single merc panel
const wchar_t* gzMoneyWithdrawMessageText[] =
{
	L"Jednorazowo mo�esz wyp�aci� do 20,000$.",
	L"Czy na pewno chcesz wp�aci� %ls na swoje konto?",
};

const wchar_t* gzCopyrightText[] =
{
	L"Copyright (C) 1999 Sir-tech Canada Ltd.  All rights reserved.",
};

//option Text
const wchar_t* zOptionsToggleText[] =
{
	L"Dialogi",
	L"Wycisz potwierdzenia",
	L"Napisy",
	L"Wstrzymuj napisy",
	L"Animowany dym",
	L"Drastyczne sceny",
	L"Nigdy nie ruszaj mojej myszki!",
	L"Stara metoda wyboru",
	L"Pokazuj tras� ruchu",
	L"Pokazuj chybione strza�y",
	L"Potwierdzenia w trybie Real-Time",
	L"Informacja, �e najemnik �pi/budzi si�",
	L"U�ywaj systemu metrycznego",
	L"�wiat�o wok� najemnik�w podczas ruchu",
	L"Przyci�gaj kursor do najemnik�w",
	L"Przyci�gaj kursor do drzwi",
	L"Pulsuj�ce przedmioty",
	L"Pokazuj korony drzew",
	L"Pokazuj siatk�",
	L"Pokazuj kursor 3D",
};

//This is the help text associated with the above toggles.
const wchar_t* zOptionsScreenHelpText[] =
{
	//speech
	L"W��cz t� opcj�, je�li chcesz s�ucha� dialog�w.",

	//Mute Confirmation
	L"W��cza lub wy��cza g�osowe potwierzenia postaci.",

		//Subtitles
	L"W��cza lub wy��cza napisy podczas dialog�w.",

	//Key to advance speech
	L"Je�li napisy s� w��czone, opcja ta pozwoli ci spokojnie je przeczyta� podczas dialogu.",

	//Toggle smoke animation
	L"Wy��cz t� opcj�, aby poprawi� p�ynno�� dzia�ania gry.",

	//Blood n Gore
	L"Wy��cz t� opcj�, je�li nie lubisz widoku krwi.",

	//Never move my mouse
	L"Wy��cz t� opcj�, aby kursor myszki automatycznie ustawia� si� nad pojawiaj�cymi si� okienkami dialogowymi.",

	//Old selection method
	L"W��cz t� opcj�, aby wyb�r postaci dzia�a� tak jak w poprzedniej wersji gry.",

	//Show movement path
	L"W��cz t� opcj� je�li chcesz widzie� tras� ruchu w trybie Real-Time.",

	//show misses
	L"W��cz t� opcj�, aby zobaczy� w co trafiaj� twoje kule gdy pud�ujesz.",

	//Real Time Confirmation
	L"Gdy opcja ta jest w��czona, ka�dy ruch najemnika w trybie Real-Time b�dzie wymaga� dodatkowego, potwierdzaj�cego klikni�cia.",

	//Sleep/Wake notification
  	L"Gdy opcja ta jest w��czona, wy�wietlana b�dzie informacja, �e najemnik po�o�y� si� spa� lub wsta� i wr�ci� do pracy.",

	//Use the metric system
	L"Gdy opcja ta jest w��czona, gra u�ywa systemu metrycznego.",

	//Merc Lighted movement
	L"Gdy opcja ta jest w��czona, teren wok� najemnika b�dzie o�wietlony podczas ruchu. Wy��cz t� opcj�, je�li obni�a p�ynno�� gry.",

	//Smart cursor
	L"Gdy opcja ta jest w��czona, kursor automatycznie ustawia si� na najemnikach gdy znajdzie si� w ich pobli�u.",

	//snap cursor to the door
	L"Gdy opcja ta jest w��czona, kursor automatycznie ustawi si� na drzwiach gdy znajdzie si� w ich pobli�u.",

	//glow items
	L"Gdy opcja ta jest w��czona, przedmioty pulsuj�. ( |I )",

	//toggle tree tops
	L"Gdy opcja ta jest w��czona, wy�wietlane s� korony drzew. ( |T )",

	//toggle wireframe
	L"Gdy opcja ta jest w��czona, wy�wietlane s� zarysy niewidocznych �cian. ( |W )",

	L"Gdy opcja ta jest w��czona, kursor ruchu wy�wietlany jest w 3D. ( |Home )",

};


const wchar_t* gzGIOScreenText[] =
{
	L"POCZ�TKOWE USTAWIENIA GRY",
	L"Styl gry",
	L"Realistyczny",
	L"S-F",
	L"Opcje broni",
	L"Mn�stwo broni",
	L"Standardowe uzbrojenie",
	L"Stopie� trudno�ci",
	L"Nowicjusz",
	L"Do�wiadczony",
	L"Ekspert",
	L"Ok",
	L"Anuluj",
	L"Dodatkowe opcje",
	L"Nielimitowany czas",
	L"Tury limitowane czasowo",
	L"Nie dzia�a w wersji demo",
};

const wchar_t* pDeliveryLocationStrings[] =
{
	L"Austin",			//Austin, Texas, USA
	L"Bagdad",			//Baghdad, Iraq (Suddam Hussein's home)
	L"Drassen",			//The main place in JA2 that you can receive items.  The other towns are dummy names...
	L"Hong Kong",		//Hong Kong, Hong Kong
	L"Bejrut",			//Beirut, Lebanon	(Middle East)
	L"Londyn",			//London, England
	L"Los Angeles",	//Los Angeles, California, USA (SW corner of USA)
	L"Meduna",			//Meduna -- the other airport in JA2 that you can receive items.
	L"Metavira",		//The island of Metavira was the fictional location used by JA1
	L"Miami",				//Miami, Florida, USA (SE corner of USA)
	L"Moskwa",			//Moscow, USSR
	L"Nowy Jork",		//New York, New York, USA
	L"Ottawa",			//Ottawa, Ontario, Canada -- where JA2 was made!
	L"Pary�",				//Paris, France
	L"Trypolis",			//Tripoli, Libya (eastern Mediterranean)
	L"Tokio",				//Tokyo, Japan
	L"Vancouver",		//Vancouver, British Columbia, Canada (west coast near US border)
};

const wchar_t* pSkillAtZeroWarning[] =
{ //This string is used in the IMP character generation.  It is possible to select 0 ability
	//in a skill meaning you can't use it.  This text is confirmation to the player.
	L"Na pewno? Warto�� zero oznacza brak jakichkolwiek umiej�tno�ci w tej dziedzinie.",
};

const wchar_t* pIMPBeginScreenStrings[] =
{
	L"( Maks. 8 znak�w )",
};

const wchar_t* pIMPFinishButtonText[ 1 ]=
{
	L"Analizuj�",
};

const wchar_t* pIMPFinishStrings[ ]=
{
	L"Dzi�kujemy, %ls", //%ls is the name of the merc
};

// the strings for imp voices screen
const wchar_t* pIMPVoicesStrings[] =
{
	L"G�os",
};

const wchar_t* pDepartedMercPortraitStrings[ ]=
{
	L"�mier� w akcji",
	L"Zwolnienie",
	L"Inny",
};

// title for program
const wchar_t* pPersTitleText[] =
{
	L"Personel",
};

// paused game strings
const wchar_t* pPausedGameText[] =
{
	L"Gra wstrzymana",
	L"Wzn�w gr� (|P|a|u|s|e)",
	L"Wstrzymaj gr� (|P|a|u|s|e)",
};


const wchar_t* pMessageStrings[] =
{
	L"Zako�czy� gr�?",
	L"OK",
	L"TAK",
	L"NIE",
	L"ANULUJ",
	L"NAJMIJ",
	L"LIE",
	L"Brak opisu", //Save slots that don't have a description.
	L"Gra zapisana.",
	L"Gra zapisana.",
	L"QuickSave", //The name of the quicksave file (filename, text reference)
	L"SaveGame",	//The name of the normal savegame file, such as SaveGame01, SaveGame02, etc.
	L"sav",				//The 3 character dos extension (represents sav)
	L"../SavedGames", //The name of the directory where games are saved.
	L"Dzie�",
	L"Najemn.",
	L"Wolna pozycja", //An empty save game slot
	L"Demo",				//Demo of JA2
	L"Debug",				//State of development of a project (JA2) that is a debug build
	L"",			//Release build for JA2
	L"strz/min",					//Abbreviation for Rounds per minute -- the potential # of bullets fired in a minute.
	L"min",					//Abbreviation for minute.
	L"m",						//One character abbreviation for meter (metric distance measurement unit).
	L"kul",				//Abbreviation for rounds (# of bullets)
	L"kg",					//Abbreviation for kilogram (metric weight measurement unit)
	L"lb",					//Abbreviation for pounds (Imperial weight measurement unit)
	L"Strona g��wna",				//Home as in homepage on the internet.
	L"USD",					//Abbreviation to US dollars
	L"N/D",					//Lowercase acronym for not applicable.
	L"Tymczasem",		//Meanwhile
	L"%ls przyby�(a) do sektora %ls%ls", //Name/Squad has arrived in sector A9.  Order must not change without notifying
																		//SirTech
	L"Wersja",
	L"Wolna pozycja na szybki zapis",
	L"Ta pozycja zarezerwowana jest na szybkie zapisy wykonywane podczas gry kombinacj� klawiszy ALT+S.",
	L"Otw.",
	L"Zamkn.",
#ifdef JA2DEMO
	L"Tu ju� wszystko za�atwione. Mo�e czas spotka� si� z Gabby'm.",
	L"Nie trzeba by�o go zabija�.",
#endif
	L"Brak miejsca na dysku twardym.  Na dysku wolne jest %ls MB, a wymagane jest przynajmniej %ls MB.",
	L"Naj�to - %ls z A.I.M.",
	L"%ls z�apa�(a) %ls",		//'Merc name' has caught 'item' -- let SirTech know if name comes after item.
	L"%ls zaaplikowa�(a) sobie lekarstwo", //'Merc name' has taken the drug
	L"%ls nie posiada wiedzy medycznej",//'Merc name' has no medical skill.

	//CDRom errors (such as ejecting CD while attempting to read the CD)
	L"Integralno�� gry zosta�a nara�ona na szwank.",
	L"B��D: Wyj�to p�yt� CD",

	//When firing heavier weapons in close quarters, you may not have enough room to do so.
	L"Nie ma miejsca, �eby st�d odda� strza�.",

	//Can't change stance due to objects in the way...
	L"Nie mo�na zmieni� pozycji w tej chwili.",

	//Simple text indications that appear in the game, when the merc can do one of these things.
	L"Upu��",
	L"Rzu�",
	L"Podaj",

	L"%ls przekazano do - %ls.", //"Item" passed to "merc".  Please try to keep the item %ls before the merc %ls, otherwise,
											 //must notify SirTech.
	L"Brak wolnego miejsca, by przekaza� %ls do - %ls.", //pass "item" to "merc".  Same instructions as above.

	//A list of attachments appear after the items.  Ex:  Kevlar vest ( Ceramic Plate 'Attached )'
	L" do��czono )",

	//Cheat modes
	L"Pierwszy poziom lamerskich zagrywek osi�gni�ty",
	L"Drugi poziom lamerskich zagrywek osi�gni�ty",

	//Toggling various stealth modes
	L"Oddzia� ma w��czony tryb skradania si�.",
	L"Oddzia� ma wy��czony tryb skradania si�.",
	L"%ls ma w��czony tryb skradania si�.",
	L"%ls ma wy��czony tryb skradania si�.",

	//Wireframes are shown through buildings to reveal doors and windows that can't otherwise be seen in
	//an isometric engine.  You can toggle this mode freely in the game.
	L"Dodatkowe siatki w��czone.",
	L"Dodatkowe siatki wy��czone.",

	//These are used in the cheat modes for changing levels in the game.  Going from a basement level to
	//an upper level, etc.
	L"Nie mo�na wyj�� do g�ry z tego poziomu...",
	L"Nie ma ju� ni�szych poziom�w...",
	L"Wej�cie na %d poziom pod ziemi�...",
	L"Wyj�cie z podziemii...",

	#ifdef JA2DEMO

	//For the demo, the sector exit interface, you'll be able to split your teams up, but the demo
	//has this feature disabled.  This string is fast help text that appears over "single" button.
	L"W pe�nej wersji gry b�dzie mo�na dzieli� oddzia�y,\nale w wersji demo jest to niemo�liwe.",

	//The overhead map is a map of the entire sector, which you can go into anytime, except in the demo.
	L"Og�lna mapa sektora jest niedost�pna w wersji demo.",

	#endif

	L" - ",		// used in the shop keeper inteface to mark the ownership of the item eg Red's gun
	L"Automatyczne centrowanie ekranu wy��czone.",
	L"Automatyczne centrowanie ekranu w��czone.",
	L"Kursor 3D wy��czony.",
	L"Kursor 3D w��czony.",
	L"Oddzia� %d aktywny.",
	L"%ls - Nie sta� ci� by wyp�aci� jej/jemu dzienn� pensj� w wysoko�ci %ls.",	//first %ls is the mercs name, the seconds is a string containing the salary
	L"Pomi�",
	L"%ls nie mo�e odej�� sam(a).",
	L"Utworzono zapis gry o nazwie SaveGame99.sav. W razie potrzeby zmie� jego nazw� na SaveGame01..10. Wtedy b�dzie mo�na go odczyta� ze standardowego okna odczytu gry.",
	L"%ls wypi�(a) troch� - %ls",
	L"Przesy�ka dotar�a do Drassen.",
 	L"%ls przyb�dzie do wyznaczonego punktu zrzutu (sektor %ls) w dniu %d, oko�o godziny %ls.",		//first %ls is mercs name, next is the sector location and name where they will be arriving in, lastely is the day an the time of arrival
	L"Lista historii zaktualizowana.",
#ifdef JA2BETAVERSION
	L"Automatyczny zapis zosta� pomy�lnie wykonany.",
#endif
};


const wchar_t ItemPickupHelpPopup[][40] =
{
	L"OK",
	L"W g�r�",
	L"Wybierz wszystko",
	L"W d�",
	L"Anuluj",
};

const wchar_t* pDoctorWarningString[] =
{
	L"%ls jest za daleko, aby podda� si� leczeniu.",
	L"Lekarze nie mogli opatrzy� wszystkich rannych.",
};

const wchar_t* pMilitiaButtonsHelpText[] =
{
	L"Podnie�(Prawy klawisz myszy)/upu��(Lewy klawisz myszy) Zielonych �o�nierzy", // button help text informing player they can pick up or drop militia with this button
	L"Podnie�(Prawy klawisz myszy)/upu��(Lewy klawisz myszy) Do�wiadczonych �o�nierzy",
	L"Podnie�(Prawy klawisz myszy)/upu��(Lewy klawisz myszy) Weteran�w",
	L"Umieszcza jednakow� ilo�� �o�nierzy samoobrony w ka�dym sektorze.",
};

const wchar_t* pMapScreenJustStartedHelpText[] =
{
	L"Zajrzyj do A.I.M. i zatrudnij kilku najemnik�w (*Wskaz�wka* musisz otworzy� laptopa)", // to inform the player to hired some mercs to get things going
	L"Je�li chcesz ju� uda� si� do Arulco, kliknij przycisk kompresji czasu, w prawym dolnym rogu ekranu.", // to inform the player to hit time compression to get the game underway
};

const wchar_t* pAntiHackerString[] =
{
	L"B��d. Brakuje pliku, lub jest on uszkodzony. Gra zostanie przerwana.",
};


const wchar_t* gzLaptopHelpText[] =
{
	//Buttons:
	L"Przegl�danie poczty",
	L"Przegl�danie stron internetowych",
	L"Przegl�danie plik�w i za��cznik�w pocztowych",
	L"Rejestr zdarze�",
	L"Informacje o cz�onkach oddzia�u",
	L"Finanse i rejestr transakcji",
	L"Koniec pracy z laptopem",

	//Bottom task bar icons (if they exist):
	L"Masz now� poczt�",
	L"Masz nowe pliki",

	//Bookmarks:
	L"Mi�dzynarodowe Stowarzyszenie Najemnik�w",
	L"Bobby Ray's - Internetowy sklep z broni�",
	L"Instytut Bada� Najemnik�w",
	L"Bardziej Ekonomiczne Centrum Rekrutacyjne",
	L"McGillicutty's - Zak�ad pogrzebowy",
	L"United Floral Service",
	L"Brokerzy ubezpieczeniowi",
};


const wchar_t* gzHelpScreenText[] =
{
	L"Zamknij okno pomocy",
};

const wchar_t* gzNonPersistantPBIText[] =
{
	L"Trwa walka. Najemnik�w mo�na wycofa� tylko na ekranie taktycznym.",
	L"W|ejd� do sektora, aby kontynuowa� walk�.",
	L"|Automatycznie rozstrzyga walk�.",
	L"Nie mo�na automatycznie rozstrzygn�� walki, gdy atakujesz.",
	L"Nie mo�na automatycznie rozstrzygn�� walki, gdy wpadasz w pu�apk�.",
	L"Nie mo�na automatycznie rozstrzygn�� walki, gdy walczysz ze stworzeniami w kopalni.",
	L"Nie mo�na automatycznie rozstrzygn�� walki, gdy w sektorze s� wrodzy cywile.",
	L"Nie mo�na automatycznie rozstrzygn�� walki, gdy w sektorze s� dzikie koty.",
	L"TRWA WALKA",
	L"W tym momencie nie mo�esz si� wycofa�.",
};

const wchar_t* gzMiscString[] =
{
	L"�o�nierze samoobrony kontynuuj� walk� bez pomocy twoich najemnik�w...",
	L"W tym momencie tankowanie nie jest konieczne.",
	L"W baku jest %d%% paliwa.",
	L"�o�nierze Deidranny przej�li ca�kowit� kontrol� nad - %ls.",
	L"Nie masz ju� gdzie zatankowa�.",
};

const wchar_t* gzIntroScreen[] =
{
	L"Nie odnaleziono filmu wprowadzaj�cego",
};

// These strings are combined with a merc name, a volume string (from pNoiseVolStr),
// and a direction (either "above", "below", or a string from pDirectionStr) to
// report a noise.
// e.g. "Sidney hears a loud sound of MOVEMENT coming from the SOUTH."
const wchar_t* pNewNoiseStr[] =
{
	L"%ls s�yszy %ls D�WI�K dochodz�cy z %ls.",
	L"%ls s�yszy %ls ODG�OS RUCHU dochodz�cy z %ls.",
	L"%ls s�yszy %ls ODG�OS SKRZYPNI�CIA dochodz�cy z %ls.",
	L"%ls s�yszy %ls PLUSK dochodz�cy z %ls.",
	L"%ls s�yszy %ls ODG�OS UDERZENIA dochodz�cy z %ls.",
	L"%ls s�yszy %ls WYBUCH dochodz�cy z %ls.",
	L"%ls s�yszy %ls KRZYK dochodz�cy z %ls.",
	L"%ls s�yszy %ls ODG�OS UDERZENIA dochodz�cy z %ls.",
	L"%ls s�yszy %ls ODG�OS UDERZENIA dochodz�cy z %ls.",
	L"%ls s�yszy %ls �OMOT dochodz�cy z %ls.",
	L"%ls s�yszy %ls TRZASK dochodz�cy z %ls.",
};

const wchar_t* wMapScreenSortButtonHelpText[] =
{
	L"Sortuj wed�ug kolumny Imi� (|F|1)",
	L"Sortuj wed�ug kolumny Przydzia� (|F|2)",
	L"Sortuj wed�ug kolumny Sen (|F|3)",
	L"Sortuj wed�ug kolumny Lokalizacja (|F|4)",
	L"Sortuj wed�ug kolumny Cel podr�y (|F|5)",
	L"Sortuj wed�ug kolumny Wyjazd (|F|6)",
};



const wchar_t* BrokenLinkText[] =
{
	L"B��d 404",
	L"Nie odnaleziono strony.",
};


const wchar_t* gzBobbyRShipmentText[] =
{
	L"Ostatnie dostawy",
	L"Zam�wienie nr ",
	L"Ilo�� przedmiot�w",
	L"Zam�wiono:",
};


const wchar_t* gzCreditNames[]=
{
	L"Chris Camfield",
	L"Shaun Lyng",
	L"Kris M�rnes",
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


const wchar_t* gzCreditNameTitle[]=
{
	L"Game Internals Programmer", 			// Chris Camfield
	L"Co-designer/Writer",							// Shaun Lyng
	L"Strategic Systems & Editor Programmer",					//Kris Marnes
	L"Producer/Co-designer",						// Ian Currie
	L"Co-designer/Map Designer",				// Linda Currie
	L"Artist",													// Eric \"WTF\" Cheng
	L"Beta Coordinator, Support",				// Lynn Holowka
	L"Artist Extraordinaire",						// Norman \"NRG\" Olsen
	L"Sound Guru",											// George Brooks
	L"Screen Designer/Artist",					// Andrew Stacey
	L"Lead Artist/Animator",						// Scot Loving
	L"Lead Programmer",									// Andrew \"Big Cheese Doddle\" Emmons
	L"Programmer",											// Dave French
	L"Strategic Systems & Game Balance Programmer",					// Alex Meduna
	L"Portraits Artist",								// Joey \"Joeker\" Whelan",
};

const wchar_t* gzCreditNameFunny[]=
{
	L"", 																			// Chris Camfield
	L"(still learning punctuation)",					// Shaun Lyng
	L"(\"It's done. I'm just fixing it\")",	//Kris \"The Cow Rape Man\" Marnes
	L"(getting much too old for this)",				// Ian Currie
	L"(and working on Wizardry 8)",						// Linda Currie
	L"(forced at gunpoint to also do QA)",			// Eric \"WTF\" Cheng
	L"(Left us for the CFSA - go figure...)",	// Lynn Holowka
	L"",																			// Norman \"NRG\" Olsen
	L"",																			// George Brooks
	L"(Dead Head and jazz lover)",						// Andrew Stacey
	L"(his real name is Robert)",							// Scot Loving
	L"(the only responsible person)",					// Andrew \"Big Cheese Doddle\" Emmons
	L"(can now get back to motocrossing)",	// Dave French
	L"(stolen from Wizardry 8)",							// Alex Meduna
	L"(did items and loading screens too!)",	// Joey \"Joeker\" Whelan",
};

const wchar_t* sRepairsDoneString[] =
{
	L"%ls sko�czy�(a) naprawia� w�asne wyposa�enie",
	L"%ls sko�czy�(a) naprawia� bro� i ochraniacze wszystkich cz�onk�w oddzia�u",
	L"%ls sko�czy�(a) naprawia� wyposa�enie wszystkich cz�onk�w oddzia�u",
	L"%ls sko�czy�(a) naprawia� ekwipunek wszystkich cz�onk�w oddzia�u",
};


const wchar_t* zGioDifConfirmText[]=
{
	L"Wybrano opcj� Nowicjusz. Opcja ta jest przeznaczona dla niedo�wiadczonych graczy, lub dla tych, kt�rzy nie maj� ochoty na d�ugie i ci�kie walki. Pami�taj, �e opcja ta ma wp�yw na przebieg ca�ej gry. Czy na pewno chcesz gra� w trybie Nowicjusz?",
	L"Wybrano opcj� Do�wiadczony. Opcja ta jest przenaczona dla graczy posiadaj�cych ju� pewne do�wiadczenie w grach tego typu. Pami�taj, �e opcja ta ma wp�yw na przebieg ca�ej gry. Czy na pewno chcesz gra� w trybie Do�wiadczony?",
	L"Wybrano opcj� Ekspert. Jakby co, to ostrzegali�my ci�. Nie obwiniaj nas, je�li wr�cisz w plastikowym worku. Pami�taj, �e opcja ta ma wp�yw na przebieg ca�ej gry. Czy na pewno chcesz gra� w trybie Ekspert?",
};


const wchar_t* gzLateLocalizedString[] =
{
	L"%ls - nie odnaleziono pliku...",

	//1-5
	L"Robot nie mo�e opu�ci� sektora bez operatora.",

	//This message comes up if you have pending bombs waiting to explode in tactical.
	L"Nie mo�na teraz kompresowa� czasu.  Poczekaj na fajerwerki!",

	//'Name' refuses to move.
	L"%ls nie chce si� przesun��.",

	//%ls a merc name
	L"%ls ma zbyt ma�o energii, aby zmieni� pozycj�.",

	//A message that pops up when a vehicle runs out of gas.
	L"%ls nie ma paliwa i stoi w sektorze %c%d.",

	//6-10

	// the following two strings are combined with the pNewNoise[] strings above to report noises
	// heard above or below the merc
	L"G�RY",
	L"DO�U",

	//The following strings are used in autoresolve for autobandaging related feedback.
	L"�aden z twoich najemnik�w nie posiada wiedzy medycznej.",
	L"Brak �rodk�w medycznych, aby za�o�y� rannym opatrunki.",
	L"Zabrak�o �rodk�w medycznych, aby za�o�y� wszystkim rannym opatrunki.",
	L"�aden z twoich najemnik�w nie potrzebuje pomocy medycznej.",
	L"Automatyczne zak�adanie opatrunk�w rannym najemnikom.",
	L"Wszystkim twoim najemnikom za�o�ono opatrunki.",

	//14
	L"Arulco",

  L"(dach)",

	L"Zdrowie: %d/%d",

	//In autoresolve if there were 5 mercs fighting 8 enemies the text would be "5 vs. 8"
	//"vs." is the abbreviation of versus.
	L"%d vs. %d",

	L"%ls - brak wolnych miejsc!",  //(ex "The ice cream truck is full")

  L"%ls nie potrzebuje pierwszej pomocy lecz opieki lekarza lub d�u�szego odpoczynku.",

	//20
	//Happens when you get shot in the legs, and you fall down.
	L"%ls dosta�(a) w nogi i upad�(a)!",
	//Name can't speak right now.
	L"%ls nie mo�e teraz m�wi�.",

	//22-24 plural versions
	L"%d zielonych �o�nierzy samoobrony awansowa�o na weteran�w.",
	L"%d zielonych �o�nierzy samoobrony awansowa�o na regularnych �o�nierzy.",
	L"%d regularnych �o�nierzy samoobrony awansowa�o na weteran�w.",

	//25
	L"Prze��cznik",

	//26
	//Name has gone psycho -- when the game forces the player into burstmode (certain unstable characters)
	L"%ls dostaje �wira!",

	//27-28
	//Messages why a player can't time compress.
	L"Niebezpiecznie jest kompresowa� teraz czas, gdy� masz najemnik�w w sektorze %ls.",
	L"Niebezpiecznie jest kompresowa� teraz czas, gdy� masz najemnik�w w kopalni zaatakowanej przez robale.",

	//29-31 singular versions
	L"1 zielony �o�nierz samoobrony awansowa� na weterana.",
	L"1 zielony �o�nierz samoobrony awansowa� na regularnego �o�nierza.",
	L"1 regularny �o�nierz samoobrony awansowa� na weterana.",

	//32-34
	L"%ls nic nie m�wi.",
	L"Wyj�� na powierzchni�?",
	L"(Oddzia� %d)",

	//35
	//Ex: "Red has repaired Scope's MP5K".  Careful to maintain the proper order (Red before Scope, Scope before MP5K)
	L"%ls naprawi�(a) najemnikowi - %ls, jego/jej - %ls",

	//36
	L"DZIKI KOT",

	//37-38 "Name trips and falls"
	L"%ls potyka si� i upada",
	L"Nie mo�na st�d podnie�� tego przedmiotu.",

	//39
	L"�aden z twoich najemnik�w nie jest w stanie walczy�.  �o�nierze samoobrony sami b�d� walczy� z robalami.",

	//40-43
	//%ls is the name of merc.
	L"%ls nie ma �rodk�w medycznych!",
	L"%ls nie posiada odpowiedniej wiedzy, aby kogokolwiek wyleczy�!",
	L"%ls nie ma narz�dzi!",
	L"%ls nie posiada odpowiedniej wiedzy, aby cokolwiek naprawi�!",

	//44-45
	L"Czas naprawy",
	L"%ls nie widzi tej osoby.",

	//46-48
	L"%ls - przed�u�ka lufy jego/jej broni odpada!",
	L"W jednym sektorze, szkolenie samoobrony mo�e prowadzi� tylko %d instruktor(�w).",
  	L"Na pewno?",

	//49-50
	L"Kompresja czasu",
	L"Pojazd ma pe�ny zbiornik paliwa.",

	//51-52 Fast help text in mapscreen.
	L"Kontynuuj kompresj� czasu (|S|p|a|c|j|a)",
	L"Zatrzymaj kompresj� czasu (|E|s|c)",

	//53-54 "Magic has unjammed the Glock 18" or "Magic has unjammed Raven's H&K G11"
	L"%ls odblokowa�(a) - %ls",
	L"%ls odblokowa�(a) najemnikowi - %ls, jego/jej - %ls",

	//55
	L"Nie mo�na kompresowa� czasu, gdy otwarty jest inwentarz sektora.",

	L"Nie odnaleziono p�yty nr 2 Jagged Alliance 2.",

	L"Przedmioty zosta�y pomy�lnie po��czone.",

	//58
	//Displayed with the version information when cheats are enabled.
	L"Bie��cy/Maks. post�p: %d%%/%d%%",

	//59
	L"Eskortowa� Johna i Mary?",

  L"Prze��cznik aktywowany.",
};




#endif //POLISH
