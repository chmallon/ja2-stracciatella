#include "Language_Defines.h"
#ifdef FRENCH
#	include "Text.h"
#	include "FileMan.h"
#	include "JA2_Demo_Ads.h"
#endif


#ifdef FRENCH

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
	must fit on a single line non matter how long the string is.  All strings start with L" and end with ",
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
	L"cal .38",
	L"9mm",
	L"cal .45",
	L"cal .357",
	L"cal 12",
	L"CAWS",
	L"5.45mm",
	L"5.56mm",
	L"7.62mm OTAN",
	L"7.62mm PV",
	L"4.7mm",
	L"5.7mm",
	L"Monster",
	L"Roquette",
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
	L"cal .38",
	L"9mm",
	L"cal .45",
	L"cal .357",
	L"cal 12",
	L"CAWS",
	L"5.45mm",
	L"5.56mm",
	L"7.62mm O.",
	L"7.62mm PV",
	L"4.7mm",
	L"5.7mm",
	L"Monster",
	L"Roquette",
	L"", // dart
};


const wchar_t WeaponType[][30] =
{
	L"Divers",
	L"Pistolet",
	L"Pistolet-mitrailleur",
	L"Mitraillette",
	L"Fusil",
	L"Fusil de pr�cision",
	L"Fusil d'assaut",
	L"Mitrailleuse l�g�re",
	L"Fusil � pompe",
};

const wchar_t TeamTurnString[][STRING_LENGTH] =
{
	L"Tour du joueur", // player's turn
	L"Tour de l'adversaire",
	L"Tour des cr�atures",
	L"Tour de la milice",
	L"Tour des civils",
	// planning turn
};

const wchar_t Message[][STRING_LENGTH] =
{
	L"",

	// In the following 8 strings, the %ls is the merc's name, and the %d (if any) is a number.

	L"%ls est touch� � la t�te et perd un point de sagesse !",
	L"%ls est touch� � l'�paule et perd un point de dext�rit� !",
	L"%ls est touch� � la poitrine et perd un point de force !",
	L"%ls est touch� � la jambe et perd un point d'agilit� !",
	L"%ls est touch� � la t�te et perd %d points de sagesse !",
	L"%ls est touch� � l'�paule et perd %d points de dext�rit� !",
	L"%ls est touch� � la poitrine et perd %d points de force !",
	L"%ls est touch� � la jambe et perd %d points d'agilit� !",
	L"Interruption !",

	// The first %ls is a merc's name, the second is a string from pNoiseVolStr,
	// the third is a string from pNoiseTypeStr, and the last is a string from pDirectionStr

	L"", //OBSOLETE
	L"Les renforts sont arriv�s !",

	// In the following four lines, all %ls's are merc names

	L"%ls recharge.",
	L"%ls n'a pas assez de Points d'Action !",
	L"%ls applique les premiers soins (Appuyez sur une touche pour annuler).",
	L"%ls et %ls appliquent les premiers soins (Appuyez sur une touche pour annuler).",
	// the following 17 strings are used to create lists of gun advantages and disadvantages
	// (separated by commas)
	L"fiable",
	L"peu fiable",
	L"facile � entretenir",
	L"difficile � entretenir",
	L"puissant",
	L"peu puissant",
	L"cadence de tir �lev�e",
	L"faible cadence de tir",
	L"longue port�e",
	L"courte port�e",
	L"l�ger",
	L"encombrant",
	L"petit",
	L"tir en rafale",
	L"pas de tir en rafale",
	L"grand chargeur",
	L"petit chargeur",

	// In the following two lines, all %ls's are merc names

	L"Le camouflage de %ls s'est effac�.",
	L"Le camouflage de %ls est parti.",

	// The first %ls is a merc name and the second %ls is an item name

	L"La deuxi�me arme est vide !",
	L"%ls a vol� le/la %ls.",

	// The %ls is a merc name

	L"L'arme de %ls ne peut pas tirer en rafale.",

	L"Vous avez d�j� ajout� cet accessoire.",
	L"Combiner les objets ?",

	// Both %ls's are item names

	L"Vous ne pouvez combiner un(e) %ls avec un(e) %ls.",

	L"Aucun",
	L"Ejecter chargeur",
	L"Accessoire",

	//You cannot use "item(s)" and your "other item" at the same time.
	//Ex:  You cannot use sun goggles and you gas mask at the same time.
	L"Vous ne pouvez utiliser votre %ls et votre %ls simultan�ment.",

	L"Vous pouvez combiner cet accessoire avec certains objets en le mettant dans l'un des quatre emplacements disponibles.",
	L"Vous pouvez combiner cet accessoire avec certains objets en le mettant dans l'un des quatre emplacements disponibles (Ici, cet accessoire n'est pas compatible avec cet objet).",
	L"Ce secteur n'a pas �t� s�curis� !",
	L"Vous devez donner %ls � %ls",//inverted !! you still need to give the letter to X
	L"%ls a �t� touch� � la t�te !",
	L"Rompre le combat ?",
	L"Cet accessoire ne pourra plus �tre enlev�. D�sirez-vous toujours le mettre ?",
	L"%ls se sent beaucoup mieux !",
	L"%ls a gliss� sur des billes !",
	L"%ls n'est pas parvenu � ramasser le/la %ls !",
	L"%ls a r�par� le %ls",
	L"Interruption pour ",
	L"Voulez-vous vous rendre ?",
	L"Cette personne refuse votre aide.",
	L"JE NE CROIS PAS !",
  L"Pour utiliser l'h�licopt�re de Skyrider, vous devez ASSIGNER vos mercenaires au VEHICULE.",
	L"%ls ne peut recharger qu'UNE arme",
	L"Tour des chats sauvages",
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
	L"Pause",
	L"Normal",
	L"5 min",
	L"30 min",
	L"60 min",
	L"6 hrs",
};


// Assignment Strings: what assignment does the merc  have right now? For example, are they on a squad, training,
// administering medical aid (doctor) or training a town. All are abbreviated. 8 letters is the longest it can be.

const wchar_t* pAssignmentStrings[] =
{
	L"Esc. 1",
	L"Esc. 2",
	L"Esc. 3",
	L"Esc. 4",
	L"Esc. 5",
	L"Esc. 6",
	L"Esc. 7",
	L"Esc. 8",
	L"Esc. 9",
	L"Esc. 10",
	L"Esc. 11",
	L"Esc. 12",
	L"Esc. 13",
	L"Esc. 14",
	L"Esc. 15",
	L"Esc. 16",
	L"Esc. 17",
	L"Esc. 18",
	L"Esc. 19",
	L"Esc. 20",
	L"Service", // on active duty
	L"Docteur", // administering medical aid
	L"Patient", // getting medical aid
	L"Transport", // in a vehicle
	L"Transit", // in transit - abbreviated form
	L"R�paration", // repairing
	L"Formation", // training themselves
  L"Milice", // training a town to revolt
	L"Entra�neur", // training a teammate
	L"El�ve", // being trained by someone else
	L"Mort", // dead
	L"Incap.", // abbreviation for incapacitated
	L"Captur�", // Prisoner of war - captured
	L"H�pital", // patient in a hospital
	L"Vide",	// Vehicle is empty
};


const wchar_t* pMilitiaString[] =
{
	L"Milice", // the title of the militia box
	L"Disponibles", //the number of unassigned militia troops
	L"Vous ne pouvez r�organiser la milice lors d'un combat !",
};


const wchar_t* pMilitiaButtonString[] =
{
	L"Auto", // auto place the militia troops for the player
	L"OK", // done placing militia troops
};

const wchar_t* pConditionStrings[] =
{
	L"Excellent", //the state of a soldier .. excellent health
	L"Bon", // good health
	L"Satisfaisant", // fair health
	L"Bless�", // wounded health
	L"Fatigu�", // tired
	L"Epuis�", // bleeding to death
	L"Inconscient", // knocked out
	L"Mourant", // near death
	L"Mort", // dead
};

const wchar_t* pEpcMenuStrings[] =
{
	L"Service", // set merc on active duty
	L"Patient", // set as a patient to receive medical aid
	L"Transport", // tell merc to enter vehicle
	L"Laisser", // let the escorted character go off on their own
	L"Annuler", // close this menu
};


// look at pAssignmentString above for comments

const wchar_t* pPersonnelAssignmentStrings[] =
{
	L"Esc. 1",
	L"Esc. 2",
	L"Esc. 3",
	L"Esc. 4",
	L"Esc. 5",
	L"Esc. 6",
	L"Esc. 7",
	L"Esc. 8",
	L"Esc. 9",
	L"Esc. 10",
	L"Esc. 11",
	L"Esc. 12",
	L"Esc. 13",
	L"Esc. 14",
	L"Esc. 15",
	L"Esc. 16",
	L"Esc. 17",
	L"Esc. 18",
	L"Esc. 19",
	L"Esc. 20",
	L"Service",
	L"Docteur",
	L"Patient",
	L"Transport",
	L"Transit",
	L"R�paration",
	L"Formation",
  L"Milice",
	L"Entra�neur",
	L"El�ve",
	L"Mort",
	L"Incap.",
	L"Captur�",
	L"H�pital",
	L"Vide",	// Vehicle is empty
};


// refer to above for comments

const wchar_t* pLongAssignmentStrings[] =
{
	L"Esc. 1",
	L"Esc. 2",
	L"Esc. 3",
	L"Esc. 4",
	L"Esc. 5",
	L"Esc. 6",
	L"Esc. 7",
	L"Esc. 8",
	L"Esc. 9",
	L"Esc. 10",
	L"Esc. 11",
	L"Esc. 12",
	L"Esc. 13",
	L"Esc. 14",
	L"Esc. 15",
	L"Esc. 16",
	L"Esc. 17",
	L"Esc. 18",
	L"Esc. 19",
	L"Esc. 20",
	L"Service",
	L"Docteur",
	L"Patient",
	L"Transport",
	L"Transit",
	L"R�paration",
	L"Formation",
	L"Milice",
	L"Entra�neur",
	L"El�ve",
	L"Mort",
	L"Incap.",
	L"Captur�",
	L"H�pital", // patient in a hospital
	L"Vide",	// Vehicle is empty
};


// the contract options

const wchar_t* pContractStrings[] =
{
	L"Options du contrat :",
	L"", // a blank line, required
	L"Extension 1 jour", // offer merc a one day contract extension
	L"Extension 1 semaine", // 1 week
	L"Extension 2 semaines", // 2 week
	L"Renvoyer", // end merc's contract
	L"Annuler", // stop showing this menu
};

const wchar_t* pPOWStrings[] =
{
	L"Captur�",  //an acronym for Prisoner of War
	L"??",
};

const wchar_t* pLongAttributeStrings[] =
{
  L"FORCE",
	L"DEXTERITE",
	L"AGILITE",
	L"SAGESSE",
	L"TIR",
	L"MEDECINE",
	L"TECHNIQUE",
	L"COMMANDEMENT",
	L"EXPLOSIFS",
	L"NIVEAU",
};

const wchar_t* pInvPanelTitleStrings[] =
{
	L"Armure", // the armor rating of the merc
	L"Poids", // the weight the merc is carrying
	L"Cam.", // the merc's camouflage rating
};

const wchar_t* pShortAttributeStrings[] =
{
	L"Agi", // the abbreviated version of : agilit�
	L"Dex", // dext�rit�
	L"For", // strength
	L"Com", // leadership
	L"Sag", // sagesse
	L"Niv", // experience level
	L"Tir", // marksmanship skill
	L"Exp", // explosive skill
	L"Tec", // mechanical skill
	L"M�d", // medical skill
};


const wchar_t* pUpperLeftMapScreenStrings[] =
{
	L"Affectation", // the mercs current assignment
	L"Contrat", // the contract info about the merc
	L"Sant�", // the health level of the current merc
	L"Moral", // the morale of the current merc
	L"Cond.",	// the condition of the current vehicle
	L"Carb.",	// the fuel level of the current vehicle
};

const wchar_t* pTrainingStrings[] =
{
	L"Formation", // tell merc to train self
  L"Milice", // tell merc to train town
	L"Entra�neur", // tell merc to act as trainer
	L"El�ve", // tell merc to be train by other
};

const wchar_t* pGuardMenuStrings[] =
{
	L"Cadence de tir :", // the allowable rate of fire for a merc who is guarding
	L" Feu � volont�", // the merc can be aggressive in their choice of fire rates
	L" Economiser munitions", // conserve ammo
	L" Tir restreint", // fire only when the merc needs to
	L"Autres Options :", // other options available to merc
	L" Retraite", // merc can retreat
	L" Abri",  // merc is allowed to seek cover
	L" Assistance", // merc can assist teammates
	L"OK", // done with this menu
	L"Annuler", // cancel this menu
};

// This string has the same comments as above, however the * denotes the option has been selected by the player

const wchar_t* pOtherGuardMenuStrings[] =
{
	L"Cadence de tir :",
	L" *Feu � volont�*",
	L" *Economiser munitions*",
	L" *Tir restreint*",
	L"Autres Options :",
	L" *Retraite*",
	L" *Abri*",
	L" *Assistance*",
	L"OK",
	L"Annuler",
};

const wchar_t* pAssignMenuStrings[] =
{
	L"Service", // merc is on active duty
	L"Docteur", // the merc is acting as a doctor
	L"Patient", // the merc is receiving medical attention
	L"Transport", // the merc is in a vehicle
	L"R�paration", // the merc is repairing items
	L"Formation", // the merc is training
	L"Annuler", // cancel this menu
};

const wchar_t* pRemoveMercStrings[] =
{
	L"Enlever Merc", // remove dead merc from current team
	L"Annuler",
};

const wchar_t* pAttributeMenuStrings[] =
{
	L"Force",
	L"Dext�rit�",
	L"Agilit�",
	L"Sant�",
	L"Tir",
	L"M�decine",
	L"Technique",
	L"Commandement",
	L"Explosifs",
	L"Annuler",
};

const wchar_t* pTrainingMenuStrings[] =
{
 L"Formation", // train yourself
 L"Milice", // train the town
 L"Entra�neur", // train your teammates
 L"El�ve",  // be trained by an instructor
 L"Annuler", // cancel this menu
};


const wchar_t* pSquadMenuStrings[] =
{
	L"Esc. 1",
	L"Esc. 2",
	L"Esc. 3",
	L"Esc. 4",
	L"Esc. 5",
	L"Esc. 6",
	L"Esc. 7",
	L"Esc. 8",
	L"Esc. 9",
	L"Esc. 10",
	L"Esc. 11",
	L"Esc. 12",
	L"Esc. 13",
	L"Esc. 14",
	L"Esc. 15",
	L"Esc. 16",
	L"Esc. 17",
	L"Esc. 18",
	L"Esc. 19",
	L"Esc. 20",
	L"Annuler",
};


const wchar_t* pPersonnelScreenStrings[] =
{
	L"Sant� : ", // health of merc
	L"Agilit� : ",
	L"Dext�rit� : ",
	L"Force : ",
	L"Commandement : ",
	L"Sagesse : ",
	L"Niv. Exp. : ", // experience level
	L"Tir : ",
	L"Technique : ",
	L"Explosifs : ",
	L"M�decine : ",
	L"Acompte m�d. : ", // amount of medical deposit put down on the merc
	L"Contrat : ", // cost of current contract
	L"Tu�s : ", // number of kills by merc
	L"Participation : ", // number of assists on kills by merc
	L"Co�t/jour :", // daily cost of merc
	L"Co�t total :", // total cost of merc
	L"Contrat :", // cost of current contract
	L"Services rendus :", // total service rendered by merc
	L"Salaires dus :", // amount left on MERC merc to be paid
	L"Pr�cision :", // percentage of shots that hit target
	L"Combats :", // number of battles fought
	L"Blessures :", // number of times merc has been wounded
	L"Sp�cialit�s :",
	L"Aucune sp�cialit�",
};


//These string correspond to enums used in by the SkillTrait enums in SoldierProfileType.h
const wchar_t* gzMercSkillText[] =
{
	L"Aucune sp�cialit�",
	L"Crochetage",
	L"Combat � mains nues",
	L"Electronique",
	L"Op�rations de nuit",
	L"Lancer",
	L"Enseigner",
	L"Armes lourdes",
	L"Armes automatiques",
	L"Furtivit�",
	L"Ambidextre",
	L"Voleur",
	L"Arts martiaux",
	L"Couteau",
	L"Bonus toucher (sur le toit)",
	L"Camouflage",
	L"(Expert)",
};


// This is pop up help text for the options that are available to the merc

const wchar_t* pTacticalPopupButtonStrings[] =
{
	L"|Debout/Marcher",
	L"|Accroupi/Avancer",
	L"Debout/|Courir",
	L"|A terre/Ramper",
	L"|Regarder",
	L"Action",
	L"Parler",
	L"Examiner (|C|t|r|l)",

	// Pop up door menu
	L"Ouvrir � la main",
	L"Examen pouss�",
	L"Crocheter",
	L"Enfoncer",
	L"D�samorcer",
	L"Verrouiller",
	L"D�verrouiller",
	L"Utiliser explosif",
	L"Utiliser pied de biche",
	L"Annuler (|E|c|h|a|p)",
	L"Fermer",
};

// Door Traps. When we examine a door, it could have a particular trap on it. These are the traps.

const wchar_t* pDoorTrapStrings[] =
{
	L"aucun pi�ge",
	L"un pi�ge explosif",
	L"un pi�ge �lectrique",
	L"une alarme sonore",
	L"une alarme silencieuse",
};

// On the map screen, there are four columns. This text is popup help text that identifies the individual columns.

const wchar_t* pMapScreenMouseRegionHelpText[] =
{
	L"Choix personnage",
	L"Affectation",
	L"Destination",
	L"Merc |Contrat",
	L"Enlever Merc",
	L"Repos",
};

// volumes of noises

const wchar_t* pNoiseVolStr[] =
{
	L"FAIBLE",
	L"MOYEN",
	L"FORT",
	L"TRES FORT",
};

// types of noises

const wchar_t* pNoiseTypeStr[] = // OBSOLETE
{
	L"INCONNU",
	L"MOUVEMENT",
	L"GRINCEMENT",
	L"CLAPOTEMENT",
	L"IMPACT",
	L"COUP DE FEU",
	L"EXPLOSION",
	L"CRI",
	L"IMPACT",
	L"IMPACT",
	L"BRUIT",
	L"COLLISION",
};

// Directions that are used to report noises

const wchar_t* pDirectionStr[] =
{
	L"au NORD-EST",
	L"� l'EST",
	L"au SUD-EST",
	L"au SUD",
	L"au SUD-OUEST",
	L"� l'OUEST",
	L"au NORD-OUEST",
	L"au NORD",
};

// These are the different terrain types.

const wchar_t* pLandTypeStrings[] =
{
	L"Ville",
	L"Route",
	L"Plaine",
	L"D�sert",
	L"Bois",
	L"For�t",
	L"Marais",
	L"Eau",
	L"Collines",
	L"Infranchissable",
	L"Rivi�re",	//river from north to south
	L"Rivi�re",	//river from east to west
	L"Pays �tranger",
	//NONE of the following are used for directional travel, just for the sector description.
	L"Tropical",
	L"Cultures",
	L"Plaines, route",
	L"Bois, route",
	L"Ferme, route",
	L"Tropical, route",
	L"For�t, route",
	L"Route c�ti�re",
	L"Montagne, route",
	L"C�te, route",
	L"D�sert, route",
	L"Marais, route",
	L"Bois, site SAM",
	L"D�sert, site SAM",
	L"Tropical, site SAM",
	L"Meduna, site SAM",

	//These are descriptions for special sectors
	L"H�pital de Cambria",
	L"A�roport de Drassen",
	L"A�roport de Meduna",
	L"Site SAM",
	L"Base rebelle", //The rebel base underground in sector A10
	L"Prison de Tixa",	//The basement of the Tixa Prison (J9)
	L"Repaire de cr�atures",	//Any mine sector with creatures in it
	L"Sous-sols d'Orta",	//The basement of Orta (K4)
	L"Tunnel",				//The tunnel access from the maze garden in Meduna
										//leading to the secret shelter underneath the palace
	L"Abri",				//The shelter underneath the queen's palace
	L"",							//Unused
};

const wchar_t* gpStrategicString[] =
{
	L"",	//Unused
	L"%ls d�tect� dans le secteur %c%d et une autre escouade est en route.",	//STR_DETECTED_SINGULAR
	L"%ls d�tect� dans le secteur %c%d et d'autres escouades sont en route.",	//STR_DETECTED_PLURAL
	L"Voulez-vous coordonner vos mouvements de troupe ?",													//STR_COORDINATE

	//Dialog strings for enemies.

	L"L'ennemi vous propose de vous rendre.",			//STR_ENEMY_SURRENDER_OFFER
	L"L'ennemi a captur� vos mercenaires inconscients.",	//STR_ENEMY_CAPTURED

	//The text that goes on the autoresolve buttons

	L"Retraite", 		//The retreat button				//STR_AR_RETREAT_BUTTON
	L"OK",		//The done button				//STR_AR_DONE_BUTTON

	//The headers are for the autoresolve type (MUST BE UPPERCASE)

	L"DEFENSEURS",								//STR_AR_DEFEND_HEADER
	L"ATTAQUANTS",								//STR_AR_ATTACK_HEADER
	L"RENCONTRE",								//STR_AR_ENCOUNTER_HEADER
	L"Secteur",		//The Sector A9 part of the header		//STR_AR_SECTOR_HEADER

	//The battle ending conditions

	L"VICTOIRE !",								//STR_AR_OVER_VICTORY
	L"DEFAITE !",								//STR_AR_OVER_DEFEAT
	L"REDDITION !",							//STR_AR_OVER_SURRENDERED
	L"CAPTURE !",								//STR_AR_OVER_CAPTURED
	L"RETRAITE !",								//STR_AR_OVER_RETREATED

	//These are the labels for the different types of enemies we fight in autoresolve.

	L"Mort",							//STR_AR_MILITIA_NAME,
	L"Mort",								//STR_AR_ELITE_NAME,
	L"Mort",								//STR_AR_TROOP_NAME,
	L"Admin",								//STR_AR_ADMINISTRATOR_NAME,
	L"Cr�ature",								//STR_AR_CREATURE_NAME,

	//Label for the length of time the battle took

	L"Temps �coul�",							//STR_AR_TIME_ELAPSED,

	//Labels for status of merc if retreating.  (UPPERCASE)

	L"SE RETIRE",								//STR_AR_MERC_RETREATED,
	L"EN RETRAITE",								//STR_AR_MERC_RETREATING,
	L"RETRAITE",								//STR_AR_MERC_RETREAT,

	//PRE BATTLE INTERFACE STRINGS
	//Goes on the three buttons in the prebattle interface.  The Auto resolve button represents
	//a system that automatically resolves the combat for the player without having to do anything.
	//These strings must be short (two lines -- 6-8 chars per line)

	L"Auto.",							//STR_PB_AUTORESOLVE_BTN,
	L"Combat",							//STR_PB_GOTOSECTOR_BTN,
	L"Retraite",							//STR_PB_RETREATMERCS_BTN,

	//The different headers(titles) for the prebattle interface.
	L"ENNEMI REPERE",							//STR_PB_ENEMYENCOUNTER_HEADER,
	L"ATTAQUE ENNEMIE",							//STR_PB_ENEMYINVASION_HEADER, // 30
	L"EMBUSCADE !",								//STR_PB_ENEMYAMBUSH_HEADER
	L"VOUS PENETREZ EN SECTEUR ENNEMI",				//STR_PB_ENTERINGENEMYSECTOR_HEADER
	L"ATTAQUE DE CREATURES",							//STR_PB_CREATUREATTACK_HEADER
	L"ATTAQUE DE CHATS SAUVAGES",							//STR_PB_BLOODCATAMBUSH_HEADER
	L"VOUS ENTREZ DANS LE REPAIRE DES CHATS SAUVAGES",			//STR_PB_ENTERINGBLOODCATLAIR_HEADER

	//Various single words for direct translation.  The Civilians represent the civilian
	//militia occupying the sector being attacked.  Limited to 9-10 chars

	L"Lieu",
	L"Ennemis",
	L"Mercs",
	L"Milice",
	L"Cr�atures",
	L"Chats",
	L"Secteur",
	L"Aucun",		//If there are non uninvolved mercs in this fight.
	L"N/A",			//Acronym of Not Applicable
	L"j",			//One letter abbreviation of day
	L"h",			//One letter abbreviation of hour

	//TACTICAL PLACEMENT USER INTERFACE STRINGS
	//The four buttons

	L"Annuler",
	L"Dispers�",
	L"Group�",
	L"OK",

	//The help text for the four buttons.  Use \n to denote new line (just like enter).

	L"|Annule le d�ploiement des mercenaires \net vous permet de les d�ployer vous-m�me.",
	L"Disperse |al�atoirement vos mercenaires \n� chaque fois.",
	L"Vous permet de placer votre groupe |de mercenaires.",
	L"Cliquez sur ce bouton lorsque vous avez d�ploy� \nvos mercenaires. (|E|n|t|r|�|e)",
	L"Vous devez d�ployer vos mercenaires \navant d'engager le combat.",

	//Various strings (translate word for word)

	L"Secteur",
	L"D�finissez les points d'entr�e",

	//Strings used for various popup message boxes.  Can be as long as desired.

	L"Il semblerait que l'endroit soit inaccessible...",
	L"D�ployez vos mercenaires dans la zone en surbrillance.",

	//This message is for mercs arriving in sectors.  Ex:  Red has arrived in sector A9.
	//Don't uppercase first character, or add spaces on either end.

	L"est arriv� dans le secteur",

	//These entries are for button popup help text for the prebattle interface.  All popup help
	//text supports the use of \n to denote new line.  Do not use spaces before or after the \n.
	L"|R�solution automatique du combat\nsans charger la carte.",
	L"|R�solution automatique impossible lorsque\nvous attaquez.",
	L"|P�n�trez dans le secteur pour engager le combat.",
	L"|Faire retraite vers le secteur pr�c�dent.",				//singular version
	L"|Faire retraite vers les secteurs pr�c�dents.", //multiple groups with same previous sector

	//various popup messages for battle conditions.

	//%c%d is the sector -- ex:  A9
	L"L'ennemi attaque votre milice dans le secteur %c%d.",
	//%c%d is the sector -- ex:  A9
	L"Les cr�atures attaquent votre milice dans le secteur %c%d.",
	//1st %d refers to the number of civilians eaten by monsters,  %c%d is the sector -- ex:  A9
	//Note:  the minimum number of civilians eaten will be two.
	L"Les cr�atures ont tu� %d civils dans le secteur %ls.",
	//%ls is the sector location -- ex:  A9: Omerta
	L"L'ennemi attaque vos mercenaires dans le secteur %ls. Aucun de vos hommes ne peut combattre !",
	//%ls is the sector location -- ex:  A9: Omerta
	L"Les cr�atures attaquent vos mercenaires dans le secteur %ls. Aucun de vos hommes ne peut combattre !",

};

const wchar_t* gpGameClockString[] =
{
	//This is the day represented in the game clock.  Must be very short, 4 characters max.
	L"Jour",
};

//When the merc finds a key, they can get a description of it which
//tells them where and when they found it.
const wchar_t* sKeyDescriptionStrings[2] =
{
	L"Secteur :",
	L"Jour :",
};

//The headers used to describe various weapon statistics.

const wchar_t gWeaponStatsDesc[][14] =
{
	L"Poids (%ls):",
	L"Etat :",
	L"Munitions :", 		// Number of bullets left in a magazine
	L"Por. :",		// Range
	L"D�g. :",		// Damage
	L"PA :",			// abbreviation for Action Points
	L"",
	L"=",
	L"=",
};

//The headers used for the merc's money.

const wchar_t gMoneyStatsDesc[][13] =
{
	L"Montant",
	L"Restant :", //this is the overall balance
	L"Montant",
	L"Partager :", // the amount he wants to separate from the overall balance to get two piles of money

	L"Actuel",
	L"Solde",
	L"Montant �",
	L"Retirer",
};

//The health of various creatures, enemies, characters in the game. The numbers following each are for comment
//only, but represent the precentage of points remaining.

const wchar_t zHealthStr[][13] =
{
	L"MOURANT",		//	>= 0
	L"CRITIQUE", 		//	>= 15
	L"FAIBLE",		//	>= 30
	L"BLESSE",    	//	>= 45
	L"SATISFAISANT",    	//	>= 60
	L"BON",     	// 	>= 75
L"EXCELLENT",		// 	>= 90
};

const wchar_t* gzMoneyAmounts[6] =
{
	L"1000",
	L"100",
	L"10",
	L"OK",
	L"Partager",
	L"Retirer",
};

// short words meaning "Advantages" for "Pros" and "Disadvantages" for "Cons."
const wchar_t gzProsLabel[10] =
{
	L"Plus :",
};

const wchar_t gzConsLabel[10] =
{
	L"Moins :",
};

//Conversation options a player has when encountering an NPC
const wchar_t zTalkMenuStrings[6][ SMALL_STRING_LENGTH ] =
{
	L"Pardon ?", 	//meaning "Repeat yourself"
	L"Amical",		//approach in a friendly
	L"Direct",		//approach directly - let's get down to business
	L"Mena�ant",		//approach threateningly - talk now, or I'll blow your face off
	L"Donner",
	L"Recruter",
};

//Some NPCs buy, sell or repair items. These different options are available for those NPCs as well.
const wchar_t zDealerStrings[4][ SMALL_STRING_LENGTH ]=
{
	L"Acheter/Vendre",
	L"Acheter",
	L"Vendre",
	L"R�parer",
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
 L"Camion de glaces",
 L"Jeep",
 L"Char",
 L"H�licopt�re",
};

const wchar_t* pShortVehicleStrings[] =
{
	L"Eldor.",
	L"Hummer",			// the HMVV
	L"Camion",
	L"Jeep",
	L"Char",
	L"H�lico", 				// the helicopter
};

const wchar_t* zVehicleName[] =
{
	L"Eldorado",
	L"Hummer",		//a military jeep. This is a brand name.
	L"Camion",			// Ice cream truck
	L"Jeep",
	L"Char",
	L"H�lico", 		//an abbreviation for Helicopter
};


//These are messages Used in the Tactical Screen

const wchar_t TacticalStr[][ MED_STRING_LENGTH ] =
{
	L"Raid a�rien",
	L"Appliquer les premiers soins ?",

	// CAMFIELD NUKE THIS and add quote #66.

	L"%ls a remarqu� qu'il manque des objets dans cet envoi.",

	// The %ls is a string from pDoorTrapStrings

	L"La serrure est pi�g�e par %ls.",
	L"Pas de serrure.",
	L"R�ussite !",
	L"Echec.",
	L"R�ussite !",
	L"Echec.",
	L"La serrure n'est pas pi�g�e.",
	L"R�ussite !",
	// The %ls is a merc name
	L"%ls ne poss�de pas la bonne cl�.",
	L"Le pi�ge est d�samorc�.",
	L"La serrure n'est pas pi�g�e.",
	L"Verrouill�e.",
	L"PORTE",
	L"PIEGEE",
	L"VERROUILLEE",
	L"OUVERTE",
	L"ENFONCEE",
	L"Un interrupteur. Voulez-vous l'actionner ?",
	L"D�samorcer le pi�ge ?",
	L"Pr�c...",
	L"Suiv...",
	L"Plus...",

	// In the next 2 strings, %ls is an item name

	L"%ls pos�(e) � terre.",
	L"%ls donn�(e) � %ls.",

	// In the next 2 strings, %ls is a name

	L"%ls a �t� pay�.",
	L"%d dus � %ls.",
	L"Choisissez la fr�quence :",  	//in this case, frequency refers to a radio signal
	L"Nombre de tours avant explosion :",	//how much time, in turns, until the bomb blows
	L"D�finir fr�quence :", 	//in this case, frequency refers to a radio signal
	L"D�samorcer le pi�ge ?",
	L"Enlever le drapeau bleu ?",
	L"Poser un drapeau bleu ?",
	L"Fin du tour",

	// In the next string, %ls is a name. Stance refers to way they are standing.

	L"Voulez-vous vraiment attaquer %ls ?",
	L"Les v�hicules ne peuvent changer de position.",
	L"Le robot ne peut changer de position.",

	// In the next 3 strings, %ls is a name

	L"%ls ne peut adopter cette position ici.",
	L"%ls ne peut recevoir de premiers soins ici.",
	L"%ls n'a pas besoin de premiers soins.",
	L"Impossible d'aller ici.",
	L"Votre escouade est au complet. Vous ne pouvez pas ajouter quelqu'un.",	//there's non room for a recruit on the player's team

	// In the next string, %ls is a name

	L"%ls a �t� recrut�(e).",

	// Here %ls is a name and %d is a number

	L"Vous devez %d $ � %ls.",

	// In the next string, %ls is a name

	L"Escorter %ls?",

	// In the next string, the first %ls is a name and the second %ls is an amount of money (including $ sign)

	L"Engager %ls � %ls la journ�e ?",

	// This line is used repeatedly to ask player if they wish to participate in a boxing match.

	L"Voulez-vous engager le combat ?",

	// In the next string, the first %ls is an item name and the
	// second %ls is an amount of money (including $ sign)

	L"Acheter %ls pour %ls ?",

	// In the next string, %ls is a name

	L"%ls est escort�(e) par l'escouade %d.",

	// These messages are displayed during play to alert the player to a particular situation

	L"ENRAYE",					//weapon is jammed.
	L"Le robot a besoin de munitions calibre %ls.",		//Robot is out of ammo
	L"Lancer ici ? Aucune chance.",		//Merc can't throw to the destination he selected

	// These are different buttons that the player can turn on and off.

	L"Furtivit� (|Z)",
	L"|Carte",
	L"|OK (Fin du tour)",
	L"Parler �",
	L"Muet",
	L"Position haute (|P|g|U|p)",
	L"Niveau du curseur (|T|a|b)",
	L"Escalader / Sauter",
	L"Position basse (|P|g|D|n)",
	L"Examiner (|C|t|r|l)",
	L"Mercenaire pr�c�dent",
	L"Mercenaire suivant (E|s|p|a|c|e)",
	L"|Options",
	L"|Rafale",
	L"|Regarder/Pivoter",
	L"Sant� : %d/%d\nEnergie : %d/%d\nMoral : %ls",
	L"Pardon ?",					//this means "what?"
	L"Suite",					//an abbrieviation for "Continued"
	L"Sourdine d�sactiv�e pour %ls.",
	L"Sourdine activ�e pour %ls.",
	L"Etat : %d/%d\nCarburant : %d/%d",
	L"Sortir du v�hicule" ,
	L"Changer d'escouade ( |M|a|j| |E|s|p|a|c|e )",
	L"Conduire",
	L"N/A",						//this is an acronym for "Not Applicable."
	L"Utiliser (Mains nues)",
	L"Utiliser (Arme � feu)",
	L"Utiliser (Couteau)",
	L"Utiliser (Explosifs)",
	L"Utiliser (Trousse de soins)",
	L"(Prendre)",
	L"(Recharger)",
	L"(Donner)",
	L"%ls part.",
	L"%ls arrive.",
	L"%ls n'a plus de Points d'Action.",
	L"%ls n'est pas disponible.",
	L"%ls est couvert de bandages.",
	L"%ls n'a plus de bandages.",
	L"Ennemi dans le secteur !",
	L"Pas d'ennemi en vue.",
	L"Pas assez de Points d'Action.",
	L"T�l�commande inutilis�e.",
	L"La rafale a vid� le chargeur !",
	L"SOLDAT",
	L"CREPITUS",
	L"Milice",
	L"CIVIL",
	L"Quitter Secteur",
	L"OK",
	L"Annuler",
	L"Mercenaire",
	L"Tous",
	L"GO",
	L"Carte",
	L"Vous ne pouvez pas quitter ce secteur par ce c�t�.",
	L"%ls est trop loin.",
	L"Effacer cime des arbres",
	L"Afficher cime des arbres",
	L"CORBEAU",				//Crow, as in the large black bird
	L"COU",
	L"TETE",
	L"TORSE",
	L"JAMBES",
	L"Donner informations � la Reine ?",
	L"Acquisition de l'ID digitale",
	L"ID digitale refus�e. Arme d�sactiv�e.",
	L"Cible acquise",
	L"Chemin bloqu�",
	L"D�p�t/Retrait",		//Help text over the $ button on the Single Merc Panel
	L"Personne n'a besoin de premiers soins.",
	L"Enr.",											// Short form of JAMMED, for small inv slots
	L"Impossible d'aller ici.",					// used ( now ) for when we click on a cliff
	L"Chemin bloqu�. Voulez-vous changer de place avec cette personne ?",
	L"La personne refuse de bouger.",
	// In the following message, '%ls' would be replaced with a quantity of money (e.g. $200)
	L"Etes-vous d'accord pour payer %ls ?",
	L"Acceptez-vous le traitement m�dical gratuit ?",
	L"Voulez-vous �pouser Daryl ?",
	L"Trousseau de Cl�s",
	L"Vous ne pouvez pas faire �a avec ce personnage.",
	L"Epargner Krott ?",
	L"Hors de port�e",
	L"Mineur",
	L"Un v�hicule ne peut rouler qu'entre des secteurs",
	L"Impossible d'apposer des bandages maintenant",
	L"Chemin bloqu� pour %ls",
	L"Vos mercenaires captur�s par l'arm�e de Deidranna sont emprisonn�s ici !",
	L"Verrou touch�",
	L"Verrou d�truit",
	L"Quelqu'un d'autre veut essayer sur cette porte.",
	L"Etat : %d/%d\nCarburant : %d/%d",
L"%ls ne peut pas voir %ls.",  // Cannot see person trying to talk to
};

//Varying helptext explains (for the "Go to Sector/Map" checkbox) what will happen given different circumstances in the "exiting sector" interface.
const wchar_t* pExitingSectorHelpText[] =
{
	//Helptext for the "Go to Sector" checkbox button, that explains what will happen when the box is checked.
	L"Si vous cochez ce bouton, le secteur adjacent sera imm�diatement charg�.",
	L"Si vous cochez ce bouton, vous arriverez directement dans l'�cran de carte\nle temps que vos mercenaires arrivent.",

	//If you attempt to leave a sector when you have multiple squads in a hostile sector.
	L"Vous ne pouvez laisser vos mercenaires ici.\nVous devez d'abord nettoyer ce secteur.",

	//Because you only have one squad in the sector, and the "move all" option is checked, the "go to sector" option is locked to on.
	//The helptext explains why it is locked.
	L"Faites sortir vos derniers mercenaires du secteur\npour charger le secteur adjacent.",
	L"Faites sortir vos derniers mercenaires du secteur\npour aller dans l'�cran de carte le temps que vos mercenaires fassent le voyage.",

	//If an EPC is the selected merc, it won't allow the merc to leave alone as the merc is being escorted.  The "single" button is disabled.
	L"%ls doit �tre escort�(e) par vos mercenaires et ne peut quitter ce secteur tout seul.",

	//If only one conscious merc is left and is selected, and there are EPCs in the squad, the merc will be prohibited from leaving alone.
	//There are several strings depending on the gender of the merc and how many EPCs are in the squad.
	//DO NOT USE THE NEWLINE HERE AS IT IS USED FOR BOTH HELPTEXT AND SCREEN MESSAGES!
	L"%ls escorte %ls, il ne peut quitter ce secteur seul.", //male singular
	L"%ls escorte %ls, elle ne peut quitter ce secteur seule.", //female singular
	L"%ls escorte plusieurs personnages, il ne peut quitter ce secteur seul.", //male plural
	L"%ls escorte plusieurs personnages, elle ne peut quitter ce secteur seule.", //female plural

	//If one or more of your mercs in the selected squad aren't in range of the traversal area, then the  "move all" option is disabled,
	//and this helptext explains why.
	L"Tous vos mercenaires doivent �tre dans les environs\npour que l'escouade avance.",

	L"", //UNUSED

	//Standard helptext for single movement.  Explains what will happen (splitting the squad)
	L"Si vous cochez ce bouton, %ls voyagera seul et sera\nautomatiquement assign� � une nouvelle escouade.",

	//Standard helptext for all movement.  Explains what will happen (moving the squad)
	L"Si vous cochez ce bouton, votre escouade\nquittera le secteur.",

	//This strings is used BEFORE the "exiting sector" interface is created.  If you have an EPC selected and you attempt to tactically
	//traverse the EPC while the escorting mercs aren't near enough (or dead, dying, or unconscious), this message will appear and the
	//"exiting sector" interface will not appear.  This is just like the situation where
	//This string is special, as it is not used as helptext.  Do not use the special newline character (\n) for this string.
	L"%ls est escort� par vos mercenaires et ne peut quitter ce secteur seul. Vos mercenaires doivent �tre � proximit�.",
};



const wchar_t* pRepairStrings[] =
{
	L"Objets", 		// tell merc to repair items in inventory
	L"Site SAM", 		// tell merc to repair SAM site - SAM is an acronym for Surface to Air Missile
	L"Annuler", 		// cancel this menu
	L"Robot", 		// repair the robot
};


// NOTE: combine prestatbuildstring with statgain to get a line like the example below.
// "John has gained 3 points of marksmanship skill."

const wchar_t* sPreStatBuildString[] =
{
	L"perd", 			// the merc has lost a statistic
	L"gagne", 		// the merc has gained a statistic
	L"point de",	// singular
	L"points de",	// plural
	L"niveau d'",	// singular
	L"niveaux d'",	// plural
};

const wchar_t* sStatGainStrings[] =
{
	L"sant�.",
	L"agilit�.",
	L"dext�rit�.",
	L"sagesse.",
	L"comp�tence m�dicale.",
	L"comp�tence en explosifs.",
	L"comp�tence technique.",
	L"tir",
	L"exp�rience.",
	L"force.",
	L"commandement.",
};


const wchar_t* pHelicopterEtaStrings[] =
{
	L"Distance totale :  ", 			// total distance for helicopter to travel
	L" Aller :  ", 			// distance to travel to destination
	L" Retour : ", 			// distance to return from destination to airport
	L"Co�t : ", 		// total cost of trip by helicopter
	L"AHP :  ", 			// ETA is an acronym for "estimated time of arrival"
	L"L'h�licopt�re n'a plus de carburant et doit se poser en terrain ennemi !",	// warning that the sector the helicopter is going to use for refueling is under enemy control ->
  L"Passagers : ",
  L"S�lectionner Skyrider ou l'aire d'atterrissage ?",
  L"Skyrider",
  L"Arriv�e",
};

const wchar_t* sMapLevelString[] =
{
	L"Niveau souterrain :", 			// what level below the ground is the player viewing in mapscreen
};

const wchar_t* gsLoyalString[] =
{
	L"Loyaut�", 			// the loyalty rating of a town ie : Loyal 53%
};


// error message for when player is trying to give a merc a travel order while he's underground.

const wchar_t* gsUndergroundString[] =
{
	L"Impossible de donner des ordres.",
};

const wchar_t* gsTimeStrings[] =
{
	L"h",				// hours abbreviation
	L"m",				// minutes abbreviation
	L"s",				// seconds abbreviation
	L"j",				// days abbreviation
};

// text for the various facilities in the sector

const wchar_t* sFacilitiesStrings[] =
{
	L"Aucun",
	L"H�pital",
	L"Industrie",
	L"Prison",
	L"Militaire",
	L"A�roport",
	L"Champ de tir",		// a field for soldiers to practise their shooting skills
};

// text for inventory pop up button

const wchar_t* pMapPopUpInventoryText[] =
{
	L"Inventaire",
	L"Quitter",
};

// town strings

const wchar_t* pwTownInfoStrings[] =
{
	L"Taille",					// 0 // size of the town in sectors
	L"", 						// blank line, required
	L"Contr�le", 					// how much of town is controlled
	L"Aucune", 					// none of this town
	L"Mine associ�e", 				// mine associated with this town
	L"Loyaut�",					// 5 // the loyalty level of this town
	L"Forces entra�n�es", 					// the forces in the town trained by the player
	L"",
	L"Principales installations", 				// main facilities in this town
	L"Niveau", 					// the training level of civilians in this town
	L"Formation",				// 10 // state of civilian training in town
	L"Milice", 					// the state of the trained civilians in the town
};

// Mine strings

const wchar_t* pwMineStrings[] =
{
	L"Mine",						// 0
	L"Argent",
	L"Or",
	L"Production quotidienne",
	L"Production estim�e",
	L"Abandonn�e",				// 5
	L"Ferm�e",
	L"Epuis�e",
	L"Production",
	L"Etat",
	L"Productivit�",
	L"Type de minerai",				// 10
	L"Contr�le de la ville",
	L"Loyaut� de la ville",
//	L"Mineurs au travail",
};

// blank sector strings

const wchar_t* pwMiscSectorStrings[] =
{
	L"Forces ennemies",
	L"Secteur",
	L"# d'objets",
	L"Inconnu",
	L"Contr�l�",
	L"Oui",
	L"Non",
};

// error strings for inventory

const wchar_t* pMapInventoryErrorString[] =
{
	L"%ls n'est pas assez pr�s.",	//Merc is in sector with item but not close enough
	L"S�lection impossible.",  //MARK CARTER
	L"%ls n'est pas dans le bon secteur.",
	L"En combat, vous devez prendre les objets vous-m�me.",
	L"En combat, vous devez abandonner les objets vous-m�me.",
	L"%ls n'est pas dans le bon secteur.",
};

const wchar_t* pMapInventoryStrings[] =
{
	L"Lieu", 			// sector these items are in
	L"Objets", 		// total number of items in sector
};


// help text for the user

const wchar_t* pMapScreenFastHelpTextList[] =
{
	L"Cliquez sur la colonne Affectation pour assigner un mercenaire � une nouvelle t�che",
	L"Cliquez sur la colonne Destination pour ordonner � un mercenaire de se rendre dans un secteur",
	L"Utilisez la compression du temps pour que le voyage du mercenaire vous paraisse moins long.",
	L"Cliquez sur un secteur pour le s�lectionner. Cliquez � nouveau pour donner un ordre de mouvement � un mercenaire ou effectuez un clic droit pour obtenir des informations sur le secteur.",
	L"Appuyez sur 'H' pour afficher l'aide en ligne.",
	L"Test Text",
	L"Test Text",
	L"Test Text",
	L"Test Text",
	L"Cet �cran ne vous est d'aucune utilit� tant que vous n'�tes pas arriv� � Arulco. Une fois votre �quipe constitu�e, cliquez sur le bouton de compression du temps en bas � droite. Le temps vous para�tra moins long...",
};

// movement menu text

const wchar_t* pMovementMenuStrings[] =
{
	L"D�placement", 	// title for movement box
	L"Route", 		// done with movement menu, start plotting movement
	L"Annuler", 		// cancel this menu
	L"Autre",		// title for group of mercs not on squads nor in vehicles
};


const wchar_t* pUpdateMercStrings[] =
{
	L"Oups :", 			// an error has occured
	L"Expiration du contrat :", 	// this pop up came up due to a merc contract ending
	L"T�ches accomplies :", // this pop up....due to more than one merc finishing assignments
	L"Mercenaires disponibles :", // this pop up ....due to more than one merc waking up and returing to work
	L"Mercenaires au repos :", // this pop up ....due to more than one merc being tired and going to sleep
	L"Contrats arrivant � �ch�ance :", 	// this pop up came up due to a merc contract ending
};

// map screen map border buttons help text

const wchar_t* pMapScreenBorderButtonHelpText[] =
{
	L"Afficher |Villes",
	L"Afficher |Mines",
	L"Afficher |Escouades & Ennemis",
	L"Afficher |Espace a�rien",
	L"Afficher |Objets",
	L"Afficher Milice & Ennemis (|Z)",
};


const wchar_t* pMapScreenBottomFastHelp[] =
{
	L"|Poste de Travail",
	L"Tactique (|E|c|h|a|p)",
	L"|Options",
	L"Compression du temps (|+)", 	// time compress more
	L"Compression du temps (|-)", 	// time compress less
	L"Message pr�c�dent (|U|p)\nPage pr�c�dente (|P|g|U|p)", 	// previous message in scrollable list
	L"Message suivant (|D|o|w|n)\nPage suivante (|P|g|D|n)", 	// next message in the scrollable list
	L"Interrompre/Reprendre (|S|p|a|c|e)",	// start/stop time compression
};

const wchar_t* pMapScreenBottomText[] =
{
	L"Solde actuel", 	// current balance in player bank account
};

const wchar_t* pMercDeadString[] =
{
	L"%ls est mort.",
};


const wchar_t* pDayStrings[] =
{
	L"Jour",
};

// the list of email sender names

const wchar_t* pSenderNameList[] =
{
	L"Enrico",
	L"Psych Pro Inc",
	L"Help Desk",
	L"Psych Pro Inc",
	L"Speck",
	L"R.I.S.",		//5
	L"Barry",
	L"Blood",
	L"Lynx",
	L"Grizzly",
	L"Vicki",			//10
	L"Trevor",
	L"Grunty",
	L"Ivan",
	L"Steroid",
	L"Igor",			//15
	L"Shadow",
	L"Red",
	L"Reaper",
	L"Fidel",
	L"Fox",				//20
	L"Sidney",
	L"Gus",
	L"Buns",
	L"Ice",
	L"Spider",		//25
	L"Cliff",
	L"Bull",
	L"Hitman",
	L"Buzz",
	L"Raider",		//30
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
	L"M.I.S. Assurance",
	L"Bobby Ray",
	L"Kingpin",
	L"John Kulba",
	L"A.I.M.",
};


// new mail notify string

const wchar_t* pNewMailStrings[] =
{
 L"Vous avez des messages...",
};


// confirm player's intent to delete messages

const wchar_t* pDeleteMailStrings[] =
{
 L"Effacer message ?",
 L"Effacer message NON CONSULTE ?",
};


// the sort header strings

const wchar_t* pEmailHeaders[] =
{
	L"De :",
	L"Sujet :",
	L"Date :",
};

// email titlebar text

const wchar_t* pEmailTitleText[] =
{
	L"Bo�te aux lettres",
};


// the financial screen strings
const wchar_t* pFinanceTitle[] =
{
	L"Bookkeeper Plus",		//the name we made up for the financial program in the game
};

const wchar_t* pFinanceSummary[] =
{
	L"Cr�dit :", 				// credit (subtract from) to player's account
	L"D�bit :", 				// debit (add to) to player's account
	L"Revenus (hier) :",
	L"D�p�ts (hier) :",
	L"D�penses (hier) :",
	L"Solde (fin de journ�e) :",
	L"Revenus (aujourd'hui) :",
	L"D�p�ts (aujourd'hui) :",
	L"D�penses (aujourd'hui) :",
	L"Solde actuel :",
	L"Revenus (pr�vision) :",
	L"Solde (pr�vision) :", 		// projected balance for player for tommorow
};


// headers to each list in financial screen

const wchar_t* pFinanceHeaders[] =
{
  L"Jour", 				// the day column
	L"Cr�dit", 			// the credits column (to ADD money to your account)
	L"D�bit",				// the debits column (to SUBTRACT money from your account)
	L"Transaction", // transaction type - see TransactionText below
	L"Solde", 		// balance at this point in time
	L"Page", 				// page number
	L"Jour(s)", 			// the day(s) of transactions this page displays
};


const wchar_t* pTransactionText[] =
{
	L"Int�r�ts cumul�s",			// interest the player has accumulated so far
	L"D�p�t anonyme",
	L"Commission",
	L"Engag�", 				// Merc was hired
	L"Achats Bobby Ray", 		// Bobby Ray is the name of an arms dealer
	L"R�glement M.E.R.C.",
	L"Acompte m�dical pour %ls", 		// medical deposit for merc
	L"Analyse IMP", 		// IMP is the acronym for International Mercenary Profiling
	L"Assurance pour %ls",
	L"R�duction d'assurance pour %ls",
	L"Extension d'assurance pour %ls", 				// johnny contract extended
	L"Annulation d'assurance pour %ls",
	L"Indemnisation pour %ls", 		// insurance claim for merc
	L"1 jour", 				// merc's contract extended for a day
	L"1 semaine", 				// merc's contract extended for a week
	L"2 semaines", 				// ... for 2 weeks
	L"Revenus des mines",
	L"", //String nuked
	L"Achat de fleurs",
	L"Remboursement m�dical pour %ls",
	L"Remb. m�dical partiel pour %ls",
	L"Pas de remb. m�dical pour %ls",
	L"Paiement � %ls",		// %ls is the name of the npc being paid
	L"Transfert de fonds pour %ls", 			// transfer funds to a merc
	L"Transfert de fonds de %ls", 		// transfer funds from a merc
	L"Co�t milice de %ls", // initial cost to equip a town's militia
	L"Achats � %ls.",	//is used for the Shop keeper interface.  The dealers name will be appended to the en d of the string.
	L"Montant d�pos� par %ls.",
};

const wchar_t* pTransactionAlternateText[] =
{
	L"Assurance pour", 				// insurance for a merc
	L"Ext. contrat de %ls (1 jour).", 				// entend mercs contract by a day
	L"Ext. contrat de %ls (1 semaine).",
	L"Ext. contrat de %ls (2 semaines).",
};

// helicopter pilot payment

const wchar_t* pSkyriderText[] =
{
	L"Skyrider a re�u %d $", 			// skyrider was paid an amount of money
	L"Skyrider attend toujours ses %d $", 		// skyrider is still owed an amount of money
	L"Skyrider a fait le plein",	// skyrider has finished refueling
	L"",//unused
	L"",//unused
	L"Skyrider est pr�t � red�coller.", // Skyrider was grounded but has been freed
	L"Skyrider n'a pas de passagers. Si vous voulez envoyer des mercenaires dans ce secteur, n'oubliez pas de les assigner � l'h�licopt�re.",
};


// strings for different levels of merc morale

const wchar_t* pMoralStrings[] =
{
	L"Superbe",
	L"Bon",
	L"Stable",
	L"Bas",
	L"Paniqu�",
	L"Mauvais",
};

// Mercs equipment has now arrived and is now available in Omerta or Drassen.

const wchar_t* pLeftEquipmentString[] =
{
	L"L'�quipement de %ls est maintenant disponible � Omerta (A9).",
	L"L'�quipement de %ls est maintenant disponible � Drassen (B13).",
};

// Status that appears on the Map Screen

const wchar_t* pMapScreenStatusStrings[] =
{
	L"Sant�",
	L"Energie",
	L"Moral",
	L"Etat",	// the condition of the current vehicle (its "Sant�")
	L"Carburant",	// the fuel level of the current vehicle (its "energy")
};


const wchar_t* pMapScreenPrevNextCharButtonHelpText[] =
{
	L"Mercenaire pr�c�dent (|G|a|u|c|h|e)", 			// previous merc in the list
	L"Mercenaire suivant (|D|r|o|i|t|e)", 				// next merc in the list
};


const wchar_t* pEtaString[] =
{
	L"HPA :", 				// eta is an acronym for Estimated Time of Arrival
};

const wchar_t* pTrashItemText[] =
{
	L"Vous ne le reverrez jamais. Vous �tes s�r de vous ?", 	// do you want to continue and lose the item forever
	L"Cet objet a l'air VRAIMENT important. Vous �tes bien s�r (mais alors BIEN SUR) de vouloir l'abandonner ?", // does the user REALLY want to trash this item
};


const wchar_t* pMapErrorString[] =
{
	L"L'escouade ne peut se d�placer si l'un de ses membres se repose.",

//1-5
	L"D�placez d'abord votre escouade.",
	L"Des ordres de mouvement ? C'est un secteur hostile !",
	L"Les mercenaires doivent d'abord �tre assign�s � un v�hicule.",
	L"Vous n'avez plus aucun membre dans votre escouade.", 		// you have non members, can't do anything
	L"Le mercenaire ne peut ob�ir.",			 		// merc can't comply with your order
//6-10
	L"doit �tre escort�. Mettez-le dans une escouade.", // merc can't move unescorted .. for a male
	L"doit �tre escort�e. Mettez-la dans une escouade.", // for a female
	L"Ce mercenaire n'est pas encore arriv� !",
	L"Il faudrait d'abord revoir les termes du contrat...",
	L"",
//11-15
	L"Des ordres de mouvement ? Vous �tes en plein combat !",
	L"Vous �tes tomb� dans une embuscade de chats sauvages dans le secteur %ls !",
	L"Vous venez d'entrer dans le repaire des chats sauvages (secteur I16) !",
	L"",
	L"Le site SAM en %ls est sous contr�le ennemi.",
//16-20
	L"La mine en %ls est sous contr�le ennemi. Votre revenu journalier est r�duit de %ls.",
	L"L'ennemi vient de prendre le contr�le du secteur %ls.",
	L"L'un au moins de vos mercenaires ne peut effectuer cette t�che.",
	L"%ls ne peut rejoindre %ls (plein).",
	L"%ls ne peut rejoindre %ls (�loignement).",
//21-25
	L"La mine en %ls a �t� reprise par les forces de Deidranna !",
	L"Les forces de Deidranna viennent d'envahir le site SAM en %ls",
	L"Les forces de Deidranna viennent d'envahir %ls",
	L"Les forces de Deidranna ont �t� aper�ues en %ls.",
	L"Les forces de Deidranna viennent de prendre %ls.",
//26-30
	L"L'un au moins de vos mercenaires ne peut se reposer.",
	L"L'un au moins de vos mercenaires ne peut �tre r�veill�.",
	L"La milice n'appara�t sur l'�cran qu'une fois son entra�nement achev�.",
	L"%ls ne peut recevoir d'ordre de mouvement pour le moment.",
	L"Les miliciens qui ne se trouvent pas dans les limites d'une ville ne peuvent �tre d�plac�s.",
//31-35
	L"Vous ne pouvez pas entra�ner de milice en %ls.",
	L"Un v�hicule ne peut se d�placer s'il est vide !",
	L"L'�tat de sant� de %ls ne lui permet pas de voyager !",
	L"Vous devez d'abord quitter le mus�e !",
	L"%ls est mort !",
//36-40
	L"%ls ne peut passer � %ls (en mouvement)",
	L"%ls ne peut pas p�n�trer dans le v�hicule de cette fa�on",
	L"%ls ne peut rejoindre %ls",
	L"Vous devez d'abord engager des mercenaires !",
	L"Ce v�hicule ne peut circuler que sur les routes !",
//41-45
	L"Vous ne pouvez r�affecter des mercenaires qui sont en d�placement",
	L"Plus d'essence !",
	L"%ls est trop fatigu�(e) pour entreprendre ce voyage.",
	L"Personne n'est capable de conduire ce v�hicule.",
	L"L'un au moins des membres de cette escouade ne peut se d�placer.",
//46-50
	L"L'un au moins des AUTRES mercenaires ne peut se d�placer.",
	L"Le v�hicule est trop endommag� !",
	L"Deux mercenaires au plus peuvent �tre assign�s � l'entra�nement de la milice dans chaque secteur.",
	L"Le robot ne peut se d�placer sans son contr�leur. Mettez-les ensemble dans la m�me escouade.",
};


// help text used during strategic route plotting
const wchar_t* pMapPlotStrings[] =
{
	L"Cliquez � nouveau sur votre destination pour la confirmer ou cliquez sur d'autres secteurs pour d�finir de nouvelles �tapes.",
	L"Route confirm�e.",
	L"Destination inchang�e.",
	L"Route annul�e.",
	L"Route raccourcie.",
};


// help text used when moving the merc arrival sector
const wchar_t* pBullseyeStrings[] =
{
	L"Cliquez sur la nouvelle destination de vos mercenaires.",
	L"OK. Les mercenaires arriveront en %ls",
	L"Les mercenaires ne peuvent �tre d�ploy�s ici, l'espace a�rien n'est pas s�curis� !",
	L"Annul�. Secteur d'arriv�e inchang�.",
	L"L'espace a�rien en %ls n'est plus s�r ! Le secteur d'arriv�e est maintenant %ls.",
};


// help text for mouse regions

const wchar_t* pMiscMapScreenMouseRegionHelpText[] =
{
	L"Inventaire (|E|n|t|r|�|e)",
	L"Lancer objet",
	L"Quitter Inventaire (|E|n|t|r|�|e)",
};



// male version of where equipment is left
const wchar_t* pMercHeLeaveString[] =
{
	L"%ls doit-il abandonner son �quipement sur place (%ls) ou � Drassen (B13) avant de quitter Arulco ?",
	L"%ls doit-il abandonner son �quipement sur place (%ls) ou � Omerta (A9) avant de quitter Arulco ?",
	L"est sur le point de partir et laissera son �quipement � Omerta (A9).",
	L"est sur le point de partir et laissera son �quipement � Drassen (B13).",
	L"%ls est sur le point de partir et laissera son �quipement en %ls.",
};


// female version
const wchar_t* pMercSheLeaveString[] =
{
	L"%ls doit-elle abandonner son �quipement sur place (%ls) ou � Drassen (B13) avant de quitter Arulco ?",
	L"%ls doit-elle abandonner son �quipement sur place (%ls) ou � Omerta (A9) avant de quitter Arulco ?",
	L"est sur le point de partir et laissera son �quipement � Omerta (A9).",
	L"est sur le point de partir et laissera son �quipement � Drassen (B13).",
	L"%ls est sur le point de partir et laissera son �quipement en %ls.",
};


const wchar_t* pMercContractOverStrings[] =
{
	L"a rempli son contrat, il est rentr� chez lui.", 		// merc's contract is over and has departed
	L"a rempli son contrat, elle est rentr�e chez elle.", 		// merc's contract is over and has departed
	L"est parti, son contrat ayant �t� annul�.", 		// merc's contract has been terminated
	L"est partie, son contrat ayant �t� annul�.",		// merc's contract has been terminated
	L"Vous devez trop d'argent au M.E.R.C., %ls quitte Arulco.", // Your M.E.R.C. account is invalid so merc left
};

// Text used on IMP Web Pages

const wchar_t* pImpPopUpStrings[] =
{
	L"Code Incorrect",
	L"Vous allez �tablir un nouveau profil. Etes-vous s�r de vouloir recommencer ?",
	L"Veuillez entrer votre nom et votre sexe.",
	L"Vous n'avez pas les moyens de vous offrir une analyse de profil.",
L"Option inaccessible pour le moment.",
	L"Pour que cette analyse soit efficace, il doit vous rester au moins une place dans votre escouade.",
	L"Profil d�j� �tabli.",
};


// button labels used on the IMP site

const wchar_t* pImpButtonText[] =
{
	L"Nous", 			// about the IMP site
	L"COMMENCER", 			// begin profiling
	L"Personnalit�", 		// personality section
	L"Caract�ristiques", 		// personal stats/attributes section
	L"Portrait", 			// the personal portrait selection
	L"Voix %d", 			// the voice selection
	L"OK", 			// done profiling
	L"Recommencer", 		// start over profiling
	L"Oui, la r�ponse en surbrillance me convient.",
	L"Oui",
	L"Non",
	L"Termin�", 			// finished answering questions
	L"Pr�c.", 			// previous question..abbreviated form
	L"Suiv.", 			// next question
	L"OUI, JE SUIS SUR.", 		// oui, I am certain
	L"NON, JE VEUX RECOMMENCER.", // non, I want to start over the profiling process
	L"OUI",
	L"NON",
	L"Retour", 			// back one page
	L"Annuler", 			// cancel selection
	L"Oui, je suis s�r.",
	L"Non, je ne suis pas s�r.",
	L"Registre", 			// the IMP site registry..when name and gender is selected
	L"Analyse", 			// analyzing your profile results
	L"OK",
	L"Voix",
};

const wchar_t* pExtraIMPStrings[] =
{
	L"Pour lancer l'analyse, cliquez sur Personnalit�.",
	L"Cliquez maintenant sur Caract�ristiques.",
	L"Passons maintenant � la galerie de portraits.",
	L"Pour que l'analyse soit compl�te, choisissez une voix.",
};

const wchar_t* pFilesTitle[] =
{
  L"Fichiers",
};

const wchar_t* pFilesSenderList[] =
{
L"Rapport Arulco", 		// the recon report sent to the player. Recon is an abbreviation for reconissance
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
	L"Historique",
};

const wchar_t* pHistoryHeaders[] =
{
	L"Jour", 			// the day the history event occurred
	L"Page", 			// the current page in the history report we are in
	L"Jour", 			// the days the history report occurs over
	L"Lieu", 			// location (in sector) the event occurred
	L"Ev�nement", 			// the event label
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
	L"%ls engag�(e) sur le site A.I.M.", 										// merc was hired from the aim site
	L"%ls engag�(e) sur le site M.E.R.C.", 									// merc was hired from the aim site
	L"%ls meurt.", 															// merc was killed
	L"Versements M.E.R.C.",								// paid outstanding bills at MERC
	L"Ordre de mission d'Enrico Chivaldori accept�",
	//6-10
	L"Profil IMP",
	L"Souscription d'un contrat d'assurance pour %ls.", 				// insurance contract purchased
	L"Annulation du contrat d'assurance de %ls.", 				// insurance contract canceled
	L"Indemnit� pour %ls.", 							// insurance claim payout for merc
	L"Extension du contrat de %ls (1 jour).", 						// Extented "mercs name"'s for a day
	//11-15
	L"Extension du contrat de %ls (1 semaine).", 					// Extented "mercs name"'s for a week
	L"Extension du contrat de %ls (2 semaines).", 					// Extented "mercs name"'s 2 weeks
	L"%ls a �t� renvoy�(e).", 													// "merc's name" was dismissed.
	L"%ls a d�missionn�.", 																		// "merc's name" quit.
	L"qu�te commenc�e.", 															// a particular quest started
	//16-20
	L"qu�te achev�e.",
	L"Entretien avec le chef des mineurs de %ls",									// talked to head miner of town
	L"Lib�ration de %ls",
	L"Activation du mode triche",
	L"Le ravitaillement devrait arriver demain � Omerta",
	//21-25
	L"%ls a quitt� l'escouade pour �pouser Daryl Hick",
	L"Expiration du contrat de %ls.",
	L"Recrutement de %ls.",
	L"Plainte d'Enrico pour manque de r�sultats",
	L"Victoire",
	//26-30
	L"La mine de %ls commence � s'�puiser",
	L"La mine de %ls est �puis�e",
	L"La mine de %ls a �t� ferm�e",
	L"La mine de %ls a �t� r�ouverte",
	L"Une prison du nom de Tixa a �t� d�couverte.",
	//31-35
	L"Rumeurs sur une usine d'armes secr�tes : Orta.",
	L"Les chercheurs d'Orta vous donnent des fusils � roquettes.",
	L"Deidranna fait des exp�riences sur les cadavres.",
	L"Frank parle de combats organis�s � San Mona.",
	L"Un t�moin pense avoir aper�u quelque chose dans les mines.",
	//36-40
	L"Rencontre avec Devin - vend des explosifs.",
	L"Rencontre avec Mike, le fameux ex-mercenaire de l'AIM !",
	L"Rencontre avec Tony - vend des armes.",
	L"Fusil � roquettes r�cup�r� aupr�s du Sergent Krott.",
	L"Acte de propri�t� du magasin d'Angel donn� � Kyle.",
	//41-45
	L"Madlab propose de construire un robot.",
	L"Gabby fait des d�coctions rendant invisible aux cr�atures.",
	L"Keith est hors-jeu.",
	L"Howard fournit du cyanure � la Reine Deidranna.",
	L"Rencontre avec Keith - vendeur � Cambria.",
	//46-50
	L"Rencontre avec Howard - pharmacien � Balime",
	L"Rencontre avec Perko - r�parateur en tous genres.",
	L"Rencontre avec Sam de Balime - vendeur de mat�riel.",
	L"Franz vend du mat�riel �lectronique.",
	L"Arnold tient un magasin de r�parations � Grumm.",
	//51-55
	L"Fredo r�pare le mat�riel �lectronique � Grumm.",
	L"Don provenant d'un homme influent de Balime.",
	L"Rencontre avec Jake, vendeur de pi�ces d�tach�es.",
	L"Cl� �lectronique re�ue.",
	L"Corruption de Walter pour ouvrir l'acc�s aux sous-sols.",
	//56-60
	L"Dave refait gratuitement le plein s'il a du carburant.",
	L"Pot-de-vin donn� � Pablo.",
	L"Kingpin cache un tr�sor dans la mine de San Mona.",
	L"Victoire de %ls dans l'Extreme Fighting",
	L"D�faite de %ls dans l'Extreme Fighting",
	//61-65
	L"Disqualification de %ls dans l'Extreme Fighting",
	L"Importante somme d�couverte dans la mine abandonn�e.",
	L"Rencontre avec un tueur engag� par Kingpin.",
	L"Perte du secteur",				//ENEMY_INVASION_CODE
	L"Secteur d�fendu",
	//66-70
	L"D�faite",							//ENEMY_ENCOUNTER_CODE
	L"Embuscade",						//ENEMY_AMBUSH_CODE
	L"Embuscade ennemie d�jou�e",
	L"Echec de l'attaque",			//ENTERING_ENEMY_SECTOR_CODE
	L"R�ussite de l'attaque !",
	//71-75
	L"Attaque de cr�atures",			//CREATURE_ATTACK_CODE
	L"Attaque de chats sauvages",			//BLOODCAT_AMBUSH_CODE
	L"Elimination des chats sauvages",
	L"%ls a �t� tu�(e)",
	L"T�te de terroriste donn�e � Carmen",
	L"Reste Slay",
	L"%ls a �t� tu�(e)",
};

const wchar_t* pHistoryLocations[] =
{
	L"N/A",						// N/A is an acronym for Not Applicable
};

// icon text strings that appear on the laptop

const wchar_t* pLaptopIcons[] =
{
	L"E-mail",
	L"Internet",
	L"Finances",
	L"Personnel",
	L"Historique",
	L"Fichiers",
	L"Eteindre",
	L"sir-FER 4.0",			// our play on the company name (Sirtech) and web surFER
};

// bookmarks for different websites
// IMPORTANT make sure you move down the Cancel string as bookmarks are being added

const wchar_t* pBookMarkStrings[] =
{
	L"A.I.M.",
	L"Bobby Ray",
	L"I.M.P",
	L"M.E.R.C.",
	L"Morgue",
	L"Fleuriste",
	L"Assurance",
	L"Annuler",
};

// When loading or download a web page

const wchar_t* pDownloadString[] =
{
	L"T�l�chargement",
	L"Chargement",
};

//This is the text used on the bank machines, here called ATMs for Automatic Teller Machine

const wchar_t* gsAtmStartButtonText[] =
{
	L"Stats", 			// view stats of the merc
	L"Inventaire", 			// view the inventory of the merc
	L"T�che",
};

// Web error messages. Please use foreign language equivilant for these messages.
// DNS is the acronym for Domain Name Server
// URL is the acronym for Uniform Resource Locator

const wchar_t* pErrorStrings[] =
{
	L"Connexion � l'h�te.",
};


const wchar_t* pPersonnelString[] =
{
	L"Mercs :", 			// mercs we have
};


const wchar_t* pWebTitle[ ]=
{
	L"sir-FER 4.0",		// our name for the version of the browser, play on company name
};


// The titles for the web program title bar, for each page loaded

const wchar_t* pWebPagesTitles[] =
{
	L"A.I.M.",
	L"Membres A.I.M.",
	L"Galerie A.I.M.",		// a mug shot is another name for a portrait
	L"Tri A.I.M.",
	L"A.I.M.",
	L"Anciens A.I.M.",
	L"R�glement A.I.M.",
	L"Historique A.I.M.",
	L"Liens A.I.M.",
	L"M.E.R.C.",
	L"Comptes M.E.R.C.",
	L"Enregistrement M.E.R.C.",
	L"Index M.E.R.C.",
	L"Bobby Ray",
	L"Bobby Ray - Armes",
	L"Bobby Ray - Munitions",
	L"Bobby Ray - Armures",
	L"Bobby Ray - Divers",							//misc is an abbreviation for miscellaneous
	L"Bobby Ray - Occasions",
	L"Bobby Ray - Commande",
	L"I.M.P.",
	L"I.M.P.",
	L"Service des Fleuristes Associ�s",
	L"Service des Fleuristes Associ�s - Exposition",
	L"Service des Fleuristes Associ�s - Bon de commande",
	L"Service des Fleuristes Associ�s - Cartes",
	L"Malleus, Incus & Stapes Courtiers",
	L"Information",
	L"Contrat",
	L"Commentaires",
	L"Morgue McGillicutty",
	L"",
	L"URL introuvable.",
	L"Bobby Ray - Derni�res commandes",
	L"",
	L"",
};

const wchar_t* pShowBookmarkString[] =
{
	L"Sir-Help",
	L"Cliquez � nouveau pour acc�der aux Favoris.",
};

const wchar_t* pLaptopTitles[] =
{
	L"Bo�te aux lettres",
	L"Fichiers",
	L"Personnel",
	L"Bookkeeper Plus",
	L"Historique",
};

const wchar_t* pPersonnelDepartedStateStrings[] =
{
	//reasons why a merc has left.
	L"Mort en mission",
	L"Parti(e)",
	L"Autre",
	L"Mariage",
	L"Contrat termin�",
	L"Quitter",
};
// personnel strings appearing in the Personnel Manager on the laptop

const wchar_t* pPersonelTeamStrings[] =
{
	L"Equipe actuelle",
	L"D�parts",
	L"Co�t quotidien :",
	L"Co�t maximum :",
	L"Co�t minimum :",
	L"Morts en mission :",
	L"Partis :",
	L"Autres :",
};


const wchar_t* pPersonnelCurrentTeamStatsStrings[] =
{
	L"Minimum",
	L"Moyenne",
	L"Maximum",
};


const wchar_t* pPersonnelTeamStatsStrings[] =
{
	L"SAN",
	L"AGI",
	L"DEX",
	L"FOR",
	L"COM",
	L"SAG",
	L"NIV",
	L"TIR",
	L"TECH",
	L"EXPL",
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
	L"Contrat",
};

// text that appears on the update panel buttons

const wchar_t* pUpdatePanelButtons[] =
{
	L"Continuer",
	L"Stop",
};

// Text which appears when everyone on your team is incapacitated and incapable of battle

const wchar_t LargeTacticalStr[][ LARGE_STRING_LENGTH ] =
{
	L"Vous avez �t� vaincu dans ce secteur !",
	L"L'ennemi, sans aucune compassion, ne fait pas de quartier !",
	L"Vos mercenaires inconscients ont �t� captur�s !",
	L"Vos mercenaires ont �t� faits prisonniers.",
};


//Insurance Contract.c
//The text on the buttons at the bottom of the screen.

const wchar_t* InsContractText[] =
{
	L"Pr�c�dent",
	L"Suivant",
	L"Accepter",
	L"Annuler",
};



//Insurance Info
// Text on the buttons on the bottom of the screen

const wchar_t* InsInfoText[] =
{
	L"Pr�c�dent",
	L"Suivant",
};



//For use at the M.E.R.C. web site. Text relating to the player's account with MERC

const wchar_t* MercAccountText[] =
{
	// Text on the buttons on the bottom of the screen
	L"Autoriser",
	L"Home",
	L"Compte # :",
	L"Merc",
	L"Jours",
	L"Taux",	//5
	L"Montant",
	L"Total :",
	L"D�sirez-vous autoriser le versement de %ls ?",		//the %ls is a string that contains the dollar amount ( ex. "$150" )
};



//For use at the M.E.R.C. web site. Text relating a MERC mercenary


const wchar_t* MercInfo[] =
{
	L"Sant�",
	L"Agilit�",
	L"Dext�rit�",
	L"Force",
	L"Commandement",
	L"Sagesse",
	L"Niveau",
	L"Tir",
	L"Technique",
	L"Explosifs",
	L"M�decine",

	L"Pr�c�dent",
	L"Engager",
	L"Suivant",
	L"Infos compl�mentaires",
	L"Home",
	L"Engag�",
	L"Salaire :",
	L"Par jour",
	L"D�c�d�(e)",

	L"Vous ne pouvez engager plus de 18 mercenaires.",
	L"Indisponible",
};



// For use at the M.E.R.C. web site. Text relating to opening an account with MERC

const wchar_t* MercNoAccountText[] =
{
	//Text on the buttons at the bottom of the screen
	L"Ouvrir compte",
	L"Annuler",
	L"Vous ne poss�dez pas de compte. D�sirez-vous en ouvrir un ?",
};



// For use at the M.E.R.C. web site. MERC Homepage

const wchar_t* MercHomePageText[] =
{
	//Description of various parts on the MERC page
	L"Speck T. Kline, fondateur",
	L"Cliquez ici pour ouvrir un compte",
	L"Cliquez ici pour voir votre compte",
	L"Cliquez ici pour consulter les fichiers",
	// The version number on the video conferencing system that pops up when Speck is talking
	L"Speck Com v3.2",
};

// For use at MiGillicutty's Web Page.

const wchar_t* sFuneralString[] =
{
	L"Morgue McGillicutty : A votre �coute depuis 1983.",
	L"Murray \"Pops\" McGillicutty, notre directeur bien aim�, est un ancien mercenaire de l'AIM. Sa sp�cialit� : la mort des autres.",
	L"Pops l'a c�toy�e pendant si longtemps qu'il est un expert de la mort, � tous points de vue.",
	L"La morgue McGillicutty vous offre un large �ventail de services fun�raires, depuis une �coute compr�hensive jusqu'� la reconstitution des corps... dispers�s.",
	L"Laissez donc la morgue McGillicutty vous aider, pour que votre compagnon repose enfin en paix.",

	// Text for the various links available at the bottom of the page
	L"ENVOYER FLEURS",
	L"CERCUEILS & URNES",
	L"CREMATION",
	L"SERVICES FUNERAIRES",
	L"NOTRE ETIQUETTE",

	// The text that comes up when you click on any of the links ( except for send flowers ).
	L"Le concepteur de ce site s'est malheureusement absent� pour cause de d�c�s familial. Il reviendra d�s que possible pour rendre ce service encore plus efficace.",
	L"Veuillez croire en nos sentiments les plus respectueux dans cette p�riode qui doit vous �tre douloureuse.",
};

// Text for the florist Home page

const wchar_t* sFloristText[] =
{
	//Text on the button on the bottom of the page

	L"Vitrine",

	//Address of United Florist

	L"\"Nous livrons partout dans le monde\"",
	L"0-800-SENTMOI",
	L"333 NoseGay Dr, Seedy City, CA USA 90210",
	L"http://www.sentmoi.com",

	// detail of the florist page

	L"Rapides et efficaces !",
	L"Livraison en 24 heures partout dans le monde (ou presque).",
	L"Les prix les plus bas (ou presque) !",
	L"Si vous trouvez moins cher, nous vous livrons gratuitement une douzaine de roses !",
	L"Flore, Faune & Fleurs depuis 1981.",
	L"Nos bombardiers (recycl�s) vous livrent votre bouquet dans un rayon de 20 km (ou presque). N'importe quand - N'importe o� !",
	L"Nous r�pondons � tous vos besoins (ou presque) !",
	L"Bruce, notre expert fleuriste-conseil, trouvera pour vous les plus belles fleurs et vous composera le plus beau bouquet que vous ayez vu !",
	L"Et n'oubliez pas que si nous ne l'avons pas, nous pouvons le faire pousser - et vite !",
};



//Florist OrderForm

const wchar_t* sOrderFormText[] =
{
	//Text on the buttons

	L"Retour",
	L"Envoi",
	L"Annuler",
	L"Galerie",

	L"Nom du bouquet :",
	L"Prix :",			//5
	L"R�f�rence :",
	L"Date de livraison",
	L"jour suivant",
	L"d�s que possible",
	L"Lieu de livraison",			//10
	L"Autres services",
	L"Pot Pourri (10$)",
	L"Roses Noires (20$)",
	L"Nature Morte (10$)",
	L"G�teau (si dispo)(10$)",		//15
	L"Carte personnelle :",
	L"Veuillez �crire votre message en 75 caract�res maximum...",
	L"...ou utiliser l'une de nos",

	L"CARTES STANDARDS",
	L"Informations",//20

	//The text that goes beside the area where the user can enter their name

	L"Nom :",
};




//Florist Gallery.c

const wchar_t* sFloristGalleryText[] =
{
	//text on the buttons

	L"Pr�c.",	//abbreviation for previous
	L"Suiv.",	//abbreviation for next

	L"Cliquez sur le bouquet que vous d�sirez commander.",
	L"Note : les bouquets \"pot pourri\" et \"nature morte\" vous seront factur�s 10$ suppl�mentaires.",

	//text on the button

	L"Home",
};

//Florist Cards

const wchar_t* sFloristCards[] =
{
	L"Faites votre choix",
	L"Retour",
};



// Text for Bobby Ray's Mail Order Site

const wchar_t* BobbyROrderFormText[] =
{
	L"Commande",				//Title of the page
	L"Qt�",					// The number of items ordered
	L"Poids (%ls)",			// The weight of the item
	L"Description",				// The name of the item
	L"Prix unitaire",				// the item's weight
	L"Total",				//5	// The total price of all of items of the same type
	L"Sous-total",				// The sub total of all the item totals added
	L"Transport",		// S&H is an acronym for Shipping and Handling
	L"Total",			// The grand total of all item totals + the shipping and handling
	L"Lieu de livraison",
	L"Type d'envoi",			//10	// See below
	L"Co�t (par %ls.)",			// The cost to ship the items
	L"Du jour au lendemain",			// Gets deliverd the next day
	L"2 c'est mieux qu'un",			// Gets delivered in 2 days
	L"Jamais 2 sans 3",			// Gets delivered in 3 days
	L"Effacer commande",//15			// Clears the order page
	L"Confirmer commande",			// Accept the order
	L"Retour",				// text on the button that returns to the previous page
	L"Home",				// Text on the button that returns to the home page
	L"* Mat�riel d'occasion",		// Disclaimer stating that the item is used
	L"Vous n'avez pas les moyens.",		//20	// A popup message that to warn of not enough money
	L"<AUCUNE>",				// Gets displayed when there is non valid city selected
	L"Etes-vous s�r de vouloir envoyer cette commande � %ls ?",		// A popup that asks if the city selected is the correct one
	L"Poids total **",			// Displays the weight of the package
	L"** Pds Min.",				// Disclaimer states that there is a minimum weight for the package
	L"Envois",
};


// This text is used when on the various Bobby Ray Web site pages that sell items

const wchar_t* BobbyRText[] =
{
	L"Pour commander",				// Title
	// instructions on how to order
	L"Cliquez sur les objets d�sir�s. Cliquez � nouveau pour s�lectionner plusieurs exemplaires d'un m�me objet. Effectuez un clic droit pour d�s�lectionner un objet. Il ne vous reste plus qu'� passer commande.",

	//Text on the buttons to go the various links

	L"Objets pr�c�dents",		//
	L"Armes", 			//3
	L"Munitions",			//4
	L"Armures",			//5
	L"Divers",			//6	//misc is an abbreviation for miscellaneous
	L"Occasion",			//7
	L"Autres objets",
	L"BON DE COMMANDE",
	L"Home",			//10

	//The following 2 lines are used on the Ammunition page.
	//They are used for help text to display how many items the player's merc has
	//that can use this type of ammo

	L"Votre �quipe poss�de",//11
	L"arme(s) qui utilise(nt) ce type de munitions", //12

	//The following lines provide information on the items

	L"Poids :",		// Weight of all the items of the same type
	L"Cal :",			// the caliber of the gun
	L"Chrg :",			// number of rounds of ammo the Magazine can hold
	L"Por :",			// The range of the gun
	L"Dgt :",			// Damage of the weapon
	L"CDT :",			// Weapon's Rate Of Fire, acronym ROF
	L"Prix :",			// Cost of the item
	L"En r�serve :",			// The number of items still in the store's inventory
	L"Qt� command�e :",		// The number of items on order
	L"Endommag�",			// If the item is damaged
	L"Poids :",			// the Weight of the item
	L"Sous-total :",			// The total cost of all items on order
	L"* %% efficacit�",		// if the item is damaged, displays the percent function of the item

	//Popup that tells the player that they can only order 10 items at a time

	L"Pas de chance ! Vous ne pouvez commander que 10 objets � la fois. Si vous d�sirez passer une commande plus importante, il vous faudra remplir un nouveau bon de commande.",

	// A popup that tells the user that they are trying to order more items then the store has in stock

	L"Nous sommes navr�s, mais nos stocks sont vides. N'h�sitez pas � revenir plus tard !",

	//A popup that tells the user that the store is temporarily sold out

	L"Nous sommes navr�s, mais nous n'en avons plus en rayon.",

};


// Text for Bobby Ray's Home Page

const wchar_t* BobbyRaysFrontText[] =
{
	//Details on the web site

	L"Vous cherchez des armes et du mat�riel militaire ? Vous avez frapp� � la bonne porte",
	L"Un seul credo : force de frappe !",
	L"Occasions et secondes mains",

	//Text for the various links to the sub pages

	L"Divers",
	L"ARMES",
	L"MUNITIONS",		//5
	L"ARMURES",

	//Details on the web site

	L"Si nous n'en vendons pas, c'est que �a n'existe pas !",
	L"En Construction",
};



// Text for the AIM page.
// This is the text used when the user selects the way to sort the aim mercanaries on the AIM mug shot page

const wchar_t* AimSortText[] =
{
	L"Membres A.I.M.",				// Title
	// Title for the way to sort
	L"Tri par :",

	// sort by...

	L"Prix",
	L"Exp�rience",
	L"Tir",
	L"M�decine",
	L"Explosifs",
	L"Technique",

	//Text of the links to other AIM pages

	L"Afficher l'index de la galerie de portraits",
	L"Consulter le fichier individuel",
	L"Consulter la galerie des anciens de l'A.I.M.",

	// text to display how the entries will be sorted

	L"Ascendant",
	L"Descendant",
};


//Aim Policies.c
//The page in which the AIM policies and regulations are displayed

const wchar_t* AimPolicyText[] =
{
	// The text on the buttons at the bottom of the page

	L"Pr�c�dent",
	L"Home AIM",
	L"Index",
	L"Suivant",
	L"Je refuse",
	L"J'accepte",
};



//Aim Member.c
//The page in which the players hires AIM mercenaries

// Instructions to the user to either start video conferencing with the merc, or to go the mug shot index

const wchar_t* AimMemberText[] =
{
	L"Cliquez pour",
	L"contacter le mercenaire.",
	L"Clic droit pour",
	L"afficher l'index.",
};

//Aim Member.c
//The page in which the players hires AIM mercenaries

const wchar_t* CharacterInfo[] =
{
	// The various attributes of the merc

	L"Sant�",
	L"Agilit�",
	L"Dext�rit�",
	L"Force",
	L"Commandement",
	L"Sagesse",
	L"Niveau",
	L"Tir",
	L"Technique",
	L"Explosifs",
	L"M�decine",				//10

	// the contract expenses' area

	L"Paie",
	L"Contrat",
	L"1 jour",
	L"1 semaine",
	L"2 semaines",

	// text for the buttons that either go to the previous merc,
	// start talking to the merc, or go to the next merc

	L"Pr�c�dent",
	L"Contacter",
	L"Suivant",

	L"Info. compl�mentaires",				// Title for the additional info for the merc's bio
	L"Membres actifs",		//20		// Title of the page
	L"Mat�riel optionnel :",				// Displays the optional gear cost
	L"D�p�t M�dical",			// If the merc required a medical deposit, this is displayed
};


//Aim Member.c
//The page in which the player's hires AIM mercenaries

//The following text is used with the video conference popup

const wchar_t* VideoConfercingText[] =
{
	L"Contrat :",				//Title beside the cost of hiring the merc

	//Text on the buttons to select the length of time the merc can be hired

	L"1 jour",
	L"1 semaine",
	L"2 semaines",

	//Text on the buttons to determine if you want the merc to come with the equipment

	L"Pas d'�quipement",
	L"Acheter �quipement",

	// Text on the Buttons

	L"TRANSFERT",			// to actually hire the merc
	L"Annuler",				// go back to the previous menu
	L"ENGAGER",				// go to menu in which you can hire the merc
	L"RACCROCHER",				// stops talking with the merc
	L"OK",
	L"MESSAGE",			// if the merc is not there, you can leave a message

	//Text on the top of the video conference popup

	L"Conf�rence vid�o avec",
	L"Connexion. . .",

	L"d�p�t compris"			// Displays if you are hiring the merc with the medical deposit
};



//Aim Member.c
//The page in which the player hires AIM mercenaries

// The text that pops up when you select the TRANSFER FUNDS button

const wchar_t* AimPopUpText[] =
{
	L"TRANSFERT ACCEPTE",	// You hired the merc
	L"TRANSFERT REFUSE",		// Player doesn't have enough money, message 1
	L"FONDS INSUFFISANTS",				// Player doesn't have enough money, message 2

	// if the merc is not available, one of the following is displayed over the merc's face

	L"En mission",
	L"Veuillez laisser un message",
	L"D�c�d�",

	//If you try to hire more mercs than game can support

	L"Votre �quipe contient d�j� 18 mercenaires.",

	L"Message pr�-enregistr�",
	L"Message enregistr�",
};


//AIM Link.c

const wchar_t* AimLinkText[] =
{
	L"Liens A.I.M.",	//The title of the AIM links page
};



//Aim History

// This page displays the history of AIM

const wchar_t* AimHistoryText[] =
{
	L"Historique A.I.M.",					//Title

	// Text on the buttons at the bottom of the page

	L"Pr�c�dent",
	L"Home",
	L"Anciens",
	L"Suivant",
};


//Aim Mug Shot Index

//The page in which all the AIM members' portraits are displayed in the order selected by the AIM sort page.

const wchar_t* AimFiText[] =
{
	// displays the way in which the mercs were sorted

	L"Prix",
	L"Exp�rience",
	L"Tir",
	L"M�decine",
	L"Explosifs",
	L"Technique",

	// The title of the page, the above text gets added at the end of this text

	L"Tri ascendant des membres de l'A.I.M. par %ls",
	L"Tri descendant des membres de l'A.I.M. par %ls",

	// Instructions to the players on what to do

	L"Cliquez pour",
	L"s�lectionner le mercenaire",			//10
	L"Clic droit pour",
	L"les options de tri",

	// Gets displayed on top of the merc's portrait if they are...

	L"Absent",
	L"D�c�d�",						//14
	L"En mission",
};



//AimArchives.
// The page that displays information about the older AIM alumni merc... mercs who are non longer with AIM

const wchar_t* AimAlumniText[] =
{
	// Text of the buttons

	L"PAGE 1",
	L"PAGE 2",
	L"PAGE 3",

	L"Anciens",	// Title of the page

	L"OK"			// Stops displaying information on selected merc
};






//AIM Home Page

const wchar_t* AimScreenText[] =
{
	// AIM disclaimers

	L"A.I.M. et le logo A.I.M. sont des marques d�pos�es dans la plupart des pays.",
	L"N'esp�rez m�me pas nous copier !",
	L"Copyright 1998-1999 A.I.M., Ltd. Tous droits r�serv�s.",

	//Text for an advertisement that gets displayed on the AIM page

	L"Service des Fleuristes Associ�s",
	L"\"Nous livrons partout dans le monde\"",				//10
	L"Faites-le dans les r�gles de l'art",
	L"... la premi�re fois",
	L"Si nous ne l'avons pas, c'est que vous n'en avez pas besoin.",
};


//Aim Home Page

const wchar_t* AimBottomMenuText[] =
{
	//Text for the links at the bottom of all AIM pages
	L"Home",
	L"Membres",
	L"Anciens",
	L"R�glement",
	L"Historique",
	L"Liens",
};



//ShopKeeper Interface
// The shopkeeper interface is displayed when the merc wants to interact with
// the various store clerks scattered through out the game.

const wchar_t* SKI_Text[ ] =
{
	L"MARCHANDISE EN STOCK",		//Header for the merchandise available
	L"PAGE",				//The current store inventory page being displayed
	L"COUT TOTAL",				//The total cost of the the items in the Dealer inventory area
	L"VALEUR TOTALE",			//The total value of items player wishes to sell
	L"EVALUATION",				//Button text for dealer to evaluate items the player wants to sell
	L"TRANSACTION",			//Button text which completes the deal. Makes the transaction.
	L"OK",				//Text for the button which will leave the shopkeeper interface.
	L"COUT REPARATION",			//The amount the dealer will charge to repair the merc's goods
	L"1 HEURE",			// SINGULAR! The text underneath the inventory slot when an item is given to the dealer to be repaired
	L"%d HEURES",		// PLURAL!   The text underneath the inventory slot when an item is given to the dealer to be repaired
	L"REPARE",		// Text appearing over an item that has just been repaired by a NPC repairman dealer
	L"Plus d'emplacements libres.",	//Message box that tells the user there is non more room to put there stuff
	L"%d MINUTES",		// The text underneath the inventory slot when an item is given to the dealer to be repaired
	L"Objet l�ch� � terre.",
};


const wchar_t* SkiMessageBoxText[] =
{
	L"Voulez-vous d�duire %ls de votre compte pour combler la diff�rence ?",
	L"Pas assez d'argent. Il vous manque %ls",
	L"Voulez-vous d�duire %ls de votre compte pour couvrir le co�t ?",
	L"Demander au vendeur de lancer la transaction",
	L"Demander au vendeur de r�parer les objets s�lectionn�s",
	L"Terminer l'entretien",
	L"Solde actuel",
};


//OptionScreen.c

const wchar_t* zOptionsText[] =
{
	//button Text
	L"Enregistrer",
	L"Charger partie",
	L"Quitter",
	L"OK",

	//Text above the slider bars
	L"Effets",
	L"Dialogue",
	L"Musique",

	//Confirmation pop when the user selects..
	L"Quitter la partie et revenir au menu principal ?",

	L"Activez le mode Dialogue ou Sous-titre.",
};


//SaveLoadScreen
const wchar_t* zSaveLoadText[] =
{
	L"Enregistrer",
	L"Charger partie",
	L"Annuler",
	L"Enregistrement",
	L"Chargement",

	L"Enregistrement r�ussi",
	L"ERREUR lors de la sauvegarde !",
	L"Chargement r�ussi",
	L"ERREUR lors du chargement !",

	L"La version de la sauvegarde est diff�rente de celle du jeu. D�sirez-vous continuer ?",
	L"Les fichiers de sauvegarde sont peut-�tre alt�r�s. Voulez-vous les effacer ?",

	//Translators, the next two strings are for the same thing.  The first one is for beta version releases and the second one
	//is used for the final version.  Please don't modify the "#ifdef JA2BETAVERSION" or the "#else" or the "#endif" as they are
	//used by the compiler and will cause program errors if modified/removed.  It's okay to translate the strings though.
#ifdef JA2BETAVERSION
	L"La version de la sauvegarde a chang�. D�sirez-vous continuer ?",
#else
	L"Tentative de chargement d'une sauvegarde de version pr�c�dente. Voulez-vous effectuer une mise � jour ?",
#endif

	//Translators, the next two strings are for the same thing.  The first one is for beta version releases and the second one
	//is used for the final version.  Please don't modify the "#ifdef JA2BETAVERSION" or the "#else" or the "#endif" as they are
	//used by the compiler and will cause program errors if modified/removed.  It's okay to translate the strings though.
#ifdef JA2BETAVERSION
	L"La version de la sauvegarde a chang�. D�sirez-vous continuer?",
#else
	L"Tentative de chargement d'une sauvegarde de version pr�c�dente. Voulez-vous effectuer une mise � jour ?",
#endif

	L"Etes-vous s�r de vouloir �craser la sauvegarde #%d ?",
	L"Voulez-vous charger la sauvegarde #%d ?",


	//The first %d is a number that contains the amount of free space on the users hard drive,
	//the second is the recommended amount of free space.
	L"Votre risquez de manquer d'espace disque. Il ne vous reste que %d Mo de libre alors que le jeu n�cessite %d Mo d'espace libre.",

	L"Enregistrement...",			//When saving a game, a message box with this string appears on the screen

	L"Peu d'armes",
	L"Beaucoup d'armes",
	L"Style r�aliste",
	L"Style SF",

	L"Difficult�",
};



//MapScreen
const wchar_t* zMarksMapScreenText[] =
{
	L"Niveau carte",
	L"Vous n'avez pas de milice : vous devez entra�ner les habitants de la ville.",
	L"Revenu quotidien",
	L"Assurance vie",
	L"%ls n'est pas fatigu�.",
	L"%ls est en mouvement et ne peut dormir.",
	L"%ls est trop fatigu� pour ob�ir.",
	L"%ls conduit.",
	L"L'escouade ne peut progresser si l'un de ses membres se repose.",

	// stuff for contracts
	L"Vous pouvez payer les honoraires de ce mercenaire, mais vous ne pouvez pas vous offrir son assurance.",
	L"La prime d'assurance de %ls co�te %ls pour %d jour(s) suppl�mentaire(s). Voulez-vous les payer ?",
	L"Inventaire du Secteur",
	L"Le mercenaire a un d�p�t m�dical.",

	// other items
	L"Medics", // people acting a field medics and bandaging wounded mercs
	L"Patients", // people who are being bandaged by a medic
	L"OK", // Continue on with the game after autobandage is complete
	L"Stop", // Stop autobandaging of patients by medics now
	L"D�sol�. Cette option n'est pas disponible.", // informs player this option/button has been disabled in the demo
	L"%ls n'a pas de trousse � outil.",
	L"%ls n'a pas de trousse de soins.",
	L"Il y a trop peu de volontaires pour l'entra�nement.",
	L"%ls ne peut pas former plus de miliciens.",
	L"Le mercenaire a un contrat d�termin�.",
L"Ce mercenaire n'est pas assur�.",
};


const wchar_t* pLandMarkInSectorString[] =
{
	L"L'escouade %d a remarqu� quelque chose dans le secteur %ls",
};

// confirm the player wants to pay X dollars to build a militia force in town
const wchar_t* pMilitiaConfirmStrings[] =
{
	L"L'entra�nement de la milice vous co�tera $", // telling player how much it will cost
	L"Etes-vous d'accord ?", // asking player if they wish to pay the amount requested
	L"Vous n'en avez pas les moyens.", // telling the player they can't afford to train this town
	L"Voulez-vous poursuivre l'entra�nement de la milice � %ls (%ls %d) ?", // continue training this town?
	L"Co�t $", // the cost in dollars to train militia
	L"(O/N)",   // abbreviated oui/non
	L"",	// unused
	L"L'entra�nement des milices dans %d secteurs vous co�tera %d $. %ls", // cost to train sveral sectors at once
	L"Vous ne pouvez pas payer les %d $ n�cessaires � l'entra�nement.",
	L"Vous ne pouvez poursuivre l'entra�nement de la milice � %ls que si cette ville est � niveau de loyaut� de %d pour-cent.",
	L"Vous ne pouvez plus entra�ner de milice � %ls.",
};

#ifdef JA2DEMOADS
const wchar_t* gpDemoString[] =
{
	//0-9
	L"LE MEILLEUR DE LA STRATEGIE.",
	L"LE MEILLEUR DU JEU DE ROLES.",
	L"Une intrigue �volutive",
	L"Un sc�nario complexe",
	L"Des combats dantesques",
	L"Eclairage dynamique",
	L"Interaction avec les autres personnages",
	L"Des tactiques �volu�es (attaque de nuit, furtivit�)",
	L"Cr�ez votre propre mercenaire",
	L"Des qu�tes passionnantes",
	L"Des v�hicules � conduire",
	//10-19
	L"Plus de 150 personnages",
	L"Des centaines d'objets",
	L"Plus de 250 secteurs diff�rents",
	L"Des explosions �bouriffantes",
	L"De nouvelles man�uvres tactiques",
	L"Des tonnes d'animation",
	L"Plus de 9000 lignes de dialogue",
	L"Une superbe bande originale de Kevin Manthei",
	L"Une strat�gie globale",
	L"Des raids a�riens",
	//20-29
	L"Entra�nez votre propre milice",
	L"Plusieurs types de jeu disponibles",
	L"Des champs de vision r�alistes",
	L"Des graphismes de haut niveau",
	L"Achetez, vendez et �changez",
	L"Une IA sans piti�",
	L"Le meilleur du combat tactique",
	L"Option SF",
	L"Une horloge de 24 heures",
	L"Bien plus de 60 heures de jeu",
	//30-34
	L"Plusieurs niveaux de difficult�",
	L"Des mod�les balistiques �tudi�s",
	L"Un moteur puissant",
	L"Un environnement interactif",
	L"Et tellement plus...",
	//35 on are BOLD
	L"",
	L"",
	L"\"L'un des jeux les plus prenants de cette fin de si�cle\"",
	L"PC Gamer",
	L"Commandez-le d�s maintenant sur WWW.JAGGEDALLIANCE2.COM !",
};

const wchar_t* gpDemoIntroString[] =
{
	L"Vous �tes sur le point de go�ter au meilleur de la strat�gie, du jeu de r�les et du combat tactique :",
	//Point 1 (uses one string)
	L"Contr�lez une �quipe de mercenaires poss�dant tous leur personnalit� (ici, pas d'unit�s identiques � l'infini).",
	//Point 2 (uses one string)
	L"Utilisez toutes les ficelles du combat tactique, faites-les courir, ramper, grimper et sauter, se battre au corps-�-corps et bien plus encore.",
	//Point 3 (uses one string)
	L"Jouez avec le feu ! Des gadgets cools, des armes d�vastatrices, des explosions �bouriffantes.",
	//Additional comment
	L"(et ce n'est qu'un d�but)",
	//Introduction/instructions
	L"Bienvenue � Demoville... (appuyez sur une touche)",
};
#endif

//Strings used in the popup box when withdrawing, or depositing money from the $ sign at the bottom of the single merc panel
const wchar_t* gzMoneyWithdrawMessageText[] =
{
	L"Vous ne pouvez retirer que 20 000 $ � la fois.",
	L"Etes-vous s�r de vouloir d�poser %ls sur votre compte ?",
};

const wchar_t* gzCopyrightText[] =
{
	L"Copyright (C) 1999 Sir-tech Canada Ltd. Tous droits r�serv�s.",
};

//option Text
const wchar_t* zOptionsToggleText[] =
{
	L"Dialogue",
	L"Confirmations muettes",
	L"Sous-titres",
	L"Pause des dialogues",
	L"Animation fum�e",
	L"Du sang et des tripes",
	L"Ne pas toucher � ma souris !",
	L"Ancienne m�thode de s�lection",
	L"Afficher chemin",
	L"Afficher tirs manqu�s",
	L"Confirmation temps r�el",
	L"Afficher notifications sommeil/r�veil",
	L"Syst�me m�trique",
	L"Mercenaire �clair� lors des mouvements",
	L"Figer curseur sur les mercenaires",
	L"Figer curseur sur les portes",
	L"Objets en surbrillance",
	L"Afficher cimes",
	L"Affichage fil de fer",
	L"Curseur 3D",
};

//This is the help text associated with the above toggles.
const wchar_t* zOptionsScreenHelpText[] =
{
	//speech
	L"Activez cette option pour entendre vos mercenaires lorsqu'ils parlent.",

	//Mute Confirmation
	L"Active/d�sactive les confirmations des mercenaires.",

		//Subtitles
	L"Affichage des sous-titres � l'�cran.",

	//Key to advance speech
	L"Si les sous-titres s'affichent � l'�cran, cette option vous permet de prendre le temps de les lire.",

	//Toggle smoke animation
	L"D�sactivez cette option si votre machine n'est pas suffisamment puissante.",

	//Blood n Gore
	L"D�sactivez cette option si le jeu vous para�t trop violent.",

	//Never move my mouse
	L"Activez cette option pour que le curseur ne se place pas automatiquement sur les boutons qui s'affichent.",

	//Old selection method
	L"Activez cette option pour retrouver vos automatismes de la version pr�c�dente.",

	//Show movement path
	L"Activez cette option pour afficher le chemin suivi par les mercenaires. Vous pouvez la d�sactiver et utiliser la touche MAJ en cours de jeu.",

	//show misses
	L"Activez cette option pour voir o� atterrissent tous vos tirs.",

	//Real Time Confirmation
	L"Activez cette option pour afficher une confirmation de mouvement en temps r�el.",

	//Sleep/Wake notification
  L"Activez cette option pour �tre mis au courant de l'�tat de veille de vos mercenaires.",

	//Use the metric system
	L"Activez cette option pour que le jeu utilise le syst�me m�trique.",

	//Merc Lighted movement
	L"Activez cette option pour �clairer les environs des mercenaires. D�sactivez-le si votre machine n'est pas suffisamment puissante.",

	//Smart cursor
	L"Activez cette option pour que le curseur se positionne directement sur un mercenaire quand il est � proximit�.",

	//snap cursor to the door
	L"Activez cette option pour que le curseur se positionne directement sur une porte quand il est � proximit�.",

	//glow items
	L"Activez cette option pour mettre les objets en �vidence",

	//toggle tree tops
	L"Activez cette option pour afficher le cime des arbres.",

	//toggle wireframe
	L"Activez cette option pour afficher les murs en fil de fer.",

	L"Activez cette option pour afficher le curseur en 3D. ( |Home )",

};


const wchar_t* gzGIOScreenText[] =
{
	L"CONFIGURATION DU JEU",
	L"Style de jeu",
	L"R�aliste",
	L"SF",
	L"Armes",
	L"Beaucoup",
	L"Peu",
	L"Difficult�",
	L"Novice",
	L"Exp�riment�",
	L"Expert",
	L"Ok",
	L"Annuler",
	L"En combat",
	L"Temps illimit�",
	L"Temps limit�",
	L"D�sactiv� pour la d�mo",
};

const wchar_t* pDeliveryLocationStrings[] =
{
	L"Austin",			//Austin, Texas, USA
	L"Bagdad",			//Baghdad, Iraq (Suddam Hussein's home)
	L"Drassen",			//The main place in JA2 that you can receive items.  The other towns are dummy names...
	L"Hong Kong",		//Hong Kong, Hong Kong
	L"Beyrouth",			//Beirut, Lebanon	(Middle East)
	L"Londres",			//London, England
	L"Los Angeles",	//Los Angeles, California, USA (SW corner of USA)
	L"Meduna",			//Meduna -- the other airport in JA2 that you can receive items.
	L"Metavira",		//The island of Metavira was the fictional location used by JA1
	L"Miami",				//Miami, Florida, USA (SE corner of USA)
	L"Moscou",			//Moscow, USSR
	L"New-York",		//New York, New York, USA
	L"Ottawa",			//Ottawa, Ontario, Canada -- where JA2 was made!
	L"Paris",				//Paris, France
	L"Tripoli",			//Tripoli, Libya (eastern Mediterranean)
	L"Tokyo",				//Tokyo, Japan
	L"Vancouver",		//Vancouver, British Columbia, Canada (west coast near US border)
};

const wchar_t* pSkillAtZeroWarning[] =
{ //This string is used in the IMP character generation.  It is possible to select 0 ability
	//in a skill meaning you can't use it.  This text is confirmation to the player.
	L"Etes-vous s�r de vous ? Une valeur de ZERO signifie que vous serez INCAPABLE d'utiliser cette comp�tence.",
};

const wchar_t* pIMPBeginScreenStrings[] =
{
	L"( 8 Caract�res Max )",
};

const wchar_t* pIMPFinishButtonText[ 1 ]=
{
	L"Analyse",
};

const wchar_t* pIMPFinishStrings[ ]=
{
	L"Nous vous remercions, %ls", //%ls is the name of the merc
};

// the strings for imp voices screen
const wchar_t* pIMPVoicesStrings[] =
{
	L"Voix",
};

const wchar_t* pDepartedMercPortraitStrings[ ]=
{
	L"Mort",
	L"Renvoy�",
	L"Autre",
};

// title for program
const wchar_t* pPersTitleText[] =
{
	L"Personnel",
};

// paused game strings
const wchar_t* pPausedGameText[] =
{
	L"Pause",
	L"Reprendre (|P|a|u|s|e)",
	L"Pause (|P|a|u|s|e)",
};


const wchar_t* pMessageStrings[] =
{
	L"Quitter la partie ?",
	L"OK",
	L"OUI",
	L"NON",
	L"Annuler",
	L"CONTRAT",
	L"MENT",
	L"Sans description", //Save slots that don't have a description.
	L"Partie sauvegard�e.",
	L"Partie sauvegard�e.",
	L"QuickSave", //The name of the quicksave file (filename, text reference)
	L"SaveGame",	//The name of the normal savegame file, such as SaveGame01, SaveGame02, etc.
	L"sav",				//The 3 character dos extension (represents sav)
	L"../SavedGames", //The name of the directory where games are saved.
	L"Jour",
	L"Mercs",
	L"Vide", //An empty save game slot
	L"D�mo",				//Demo of JA2
	L"Debug",				//State of development of a project (JA2) that is a debug build
	L"Version",			//Release build for JA2
	L"bpm",					//Abbreviation for Rounds per minute -- the potential # of bullets fired in a minute.
	L"min",					//Abbreviation for minute.
	L"m",						//One character abbreviation for meter (metric distance measurement unit).
	L"balles",				//Abbreviation for rounds (# of bullets)
	L"kg",					//Abbreviation for kilogram (metric weight measurement unit)
	L"lb",					//Abbreviation for pounds (Imperial weight measurement unit)
	L"Home",				//Home as in homepage on the internet.
	L"USD",					//Abbreviation to US dollars
	L"n/a",					//Lowercase acronym for not applicable.
	L"Entre-temps",		//Meanwhile
	L"%ls est arriv� dans le secteur %ls%ls", //Name/Squad has arrived in sector A9.  Order must not change without notifying
																		//SirTech
	L"Version",
	L"Emplacement de sauvegarde rapide vide",
	L"Cet emplacement est r�serv� aux sauvegardes rapides effectu�es depuis l'�cran tactique (ALT+S).",
	L"Ouverte",
	L"Ferm�e",
#ifdef JA2DEMO
	L"Il n'y a plus rien � faire ici. Allez donc voir Gabby.",
	L"Dommage de l'avoir tu�...",
#endif
	L"Espace disque insuffisant. Il ne vous reste que %ls Mo de libre et Jagged Alliance 2 n�cessite %ls Mo.",
	L"%ls embauch�(e) sur le site AIM",
	L"%ls prend %ls.",		//'Merc name' has caught 'item' -- let SirTech know if name comes after item.
	L"%ls a pris la drogue.", //'Merc name' has taken the drug
	L"%ls n'a aucune comp�tence m�dicale.",//'Merc name' has non medical skill.

	//CDRom errors (such as ejecting CD while attempting to read the CD)
	L"L'int�grit� du jeu n'est plus assur�e.",
	L"ERREUR : CD-ROM manquant",

	//When firing heavier weapons in close quarters, you may not have enough room to do so.
	L"Pas assez de place !",

	//Can't change stance due to objects in the way...
	L"Impossible de changer de position ici.",

	//Simple text indications that appear in the game, when the merc can do one of these things.
	L"L�cher",
	L"Lancer",
	L"Donner",

	L"%ls donn� � %ls.", //"Item" passed to "merc".  Please try to keep the item %ls before the merc %ls, otherwise,
											 //must notify SirTech.
	L"Impossible de donner %ls � %ls.", //pass "item" to "merc".  Same instructions as above.

	//A list of attachments appear after the items.  Ex:  Kevlar vest ( Ceramic Plate 'Attached )'
	L" combin� )",

	//Cheat modes
	L"Triche niveau 1",
	L"Triche niveau 2",

	//Toggling various stealth modes
	L"Escouade en mode furtif.",
	L"Escouade en mode normal.",
	L"%ls en mode furtif.",
	L"%ls en mode normal.",

	//Wireframes are shown through buildings to reveal doors and windows that can't otherwise be seen in
	//an isometric engine.  You can toggle this mode freely in the game.
	L"Fil de fer activ�",
	L"Fil de fer d�sactiv�",

	//These are used in the cheat modes for changing levels in the game.  Going from a basement level to
	//an upper level, etc.
	L"Impossible de remonter...",
	L"Pas de niveau inf�rieur...",
	L"Entr�e dans le sous-sol %d...",
	L"Sortie du sous-sol...",

	#ifdef JA2DEMO

	//For the demo, the sector exit interface, you'll be able to split your teams up, but the demo
	//has this feature disabled.  This string is fast help text that appears over "single" button.
	L"Impossible de s�parer l'�quipe dans la d�mo.",

	//The overhead map is a map of the entire sector, which you can go into anytime, except in the demo.
	L"Carte d�sactiv�e dans la d�mo.",

	#endif

	L"'s",		// used in the shop keeper inteface to mark the ownership of the item eg Red's gun
	L"Mode poursuite d�sactiv�.",
	L"Mode poursuite activ�.",
	L"Curseur 3D d�sactiv�.",
	L"Curseur 3D activ�.",
	L"Escouade %d active.",
	L"Vous ne pouvez pas payer le salaire de %ls qui se monte � %ls",	//first %ls is the mercs name, the seconds is a string containing the salary
	L"Passer",
	L"%ls ne peut sortir seul.",
	L"Une sauvegarde a �t� cr�e (Partie99.sav). Renommez-la (Partie01 - Partie10) pour pouvoir la charger ult�rieurement.",
	L"%ls a bu %ls",
	L"Un colis vient d'arriver � Drassen.",
 	L"%ls devrait arriver au point d'entr�e (secteur %ls) en jour %d vers %ls.",		//first %ls is mercs name, next is the sector location and name where they will be arriving in, lastely is the day an the time of arrival
	L"Historique mis � jour.",
#ifdef JA2BETAVERSION
	L"Partie enregistr�e dans l'emplacement de sauvegarde automatique.",
#endif
};


const wchar_t ItemPickupHelpPopup[][40] =
{
	L"OK",
	L"D�filement haut",
	L"Tout s�lectionner",
	L"D�filement bas",
	L"Annuler",
};

const wchar_t* pDoctorWarningString[] =
{
	L"%ls est trop loin pour �tre soign�.",
	L"Impossible de soigner tout le monde.",
};

const wchar_t* pMilitiaButtonsHelpText[] =
{
	L"Prendre (Clic droit)/poser (Clic gauche) Miliciens", // button help text informing player they can pick up or drop militia with this button
	L"Prendre (Clic droit)/poser (Clic gauche) Soldats",
	L"Prendre (Clic droit)/poser (Clic gauche) V�t�rans",
	L"R�partition automatique",
};

const wchar_t* pMapScreenJustStartedHelpText[] =
{
	L"Allez sur le site de l'AIM et engagez des mercenaires ( *Truc* allez voir dans le Poste de travail)", // to inform the player to hired some mercs to get things going
	L"Cliquez sur le bouton de Compression du temps pour faire avancer votre �quipe sur le terrain.", // to inform the player to hit time compression to get the game underway
};

const wchar_t* pAntiHackerString[] =
{
	L"Erreur. Fichier manquant ou corrompu. L'application va s'arr�ter.",
};


const wchar_t* gzLaptopHelpText[] =
{
	//Buttons:
	L"Voir messages",
	L"Consulter les sites Internet",
	L"Consulter les documents attach�s",
	L"Lire le compte-rendu",
	L"Afficher les infos de l'�quipe",
	L"Afficher les �tats financiers",
	L"Fermer le Poste de travail",

	//Bottom task bar icons (if they exist):
	L"Vous avez de nouveaux messages",
	L"Vous avez re�u de nouveaux fichiers",

	//Bookmarks:
	L"Association Internationale des Mercenaires",
	L"Bobby Ray : Petits et Gros Calibres",
	L"Institut des Mercenaires Professionnels",
	L"Mouvement pour l'Entra�nement et le Recrutement des Commandos",
	L"Morgue McGillicutty",
	L"Service des Fleuristes Associ�s",
	L"Courtiers d'Assurance des Mercenaires de l'A.I.M.",
};


const wchar_t* gzHelpScreenText[] =
{
	L"Quitter l'�cran d'aide",
};

const wchar_t* gzNonPersistantPBIText[] =
{
	L"Vous �tes en plein combat. Vous pouvez donner l'ordre de retraite depuis l'�cran tactique.",
	L"|P�n�trez dans le secteur pour reprendre le cours du combat.",
	L"|R�solution automatique du combat.",
	L"R�solution automatique impossible lorsque vous �tes l'attaquant.",
	L"R�solution automatique impossible lorsque vous �tes pris en embuscade.",
	L"R�solution automatique impossible lorsque vous combattez des cr�atures dans les mines.",
	L"R�solution automatique impossible en pr�sence de civils hostiles.",
	L"R�solution automatique impossible en pr�sence de chats sauvages.",
	L"COMBAT EN COURS",
	L"Retraite impossible.",
};

const wchar_t* gzMiscString[] =
{
	L"Votre milice continue le combat sans vos mercenaires...",
	L"Ce v�hicule n'a plus besoin de carburant pour le moment.",
	L"Le r�servoir est plein � %d%%.",
	L"L'arm�e de Deidranna a repris le contr�le de %ls.",
	L"Vous avez perdu un site de ravitaillement.",
};

const wchar_t* gzIntroScreen[] =
{
	L"Vid�o d'introduction introuvable",
};

// These strings are combined with a merc name, a volume string (from pNoiseVolStr),
// and a direction (either "above", "below", or a string from pDirectionStr) to
// report a noise.
// e.g. "Sidney hears a loud sound of MOVEMENT coming from the SOUTH."
const wchar_t* pNewNoiseStr[] =
{
	L"%ls entend un bruit de %ls %ls.",
	L"%ls entend un bruit %ls de MOUVEMENT %ls.",
	L"%ls entend un GRINCEMENT %ls %ls.",
	L"%ls entend un CLAPOTIS %ls %ls.",
	L"%ls entend un IMPACT %ls %ls.",
	L"%ls entend une EXPLOSION %ls %ls.",
	L"%ls entend un CRI %ls %ls.",
	L"%ls entend un IMPACT %ls %ls.",
	L"%ls entend un IMPACT %ls %ls.",
	L"%ls entend un BRUIT %ls %ls.",
	L"%ls entend un BRUIT %ls %ls.",
};

const wchar_t* wMapScreenSortButtonHelpText[] =
{
	L"Tri par nom (|F|1)",
	L"Tri par affectation (|F|2)",
	L"Tri par �tat de veille (|F|3)",
	L"Tri par lieu (|F|4)",
	L"Tri par destination (|F|5)",
	L"Tri par date de d�part (|F|6)",
};



const wchar_t* BrokenLinkText[] =
{
	L"Erreur 404",
	L"Site introuvable.",
};


const wchar_t* gzBobbyRShipmentText[] =
{
	L"Derniers envois",
	L"Commande #",
	L"Quantit� d'objets",
	L"Command�",
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
	L"Programmeur", 			// Chris Camfield
	L"Co-designer/Ecrivain",							// Shaun Lyng
	L"Syst�mes strat�giques & Programmeur",					//Kris Marnes
	L"Producteur/Co-designer",						// Ian Currie
	L"Co-designer/Conception des cartes",				// Linda Currie
	L"Artiste",													// Eric \"WTF\" Cheng
	L"Coordination, Assistance",				// Lynn Holowka
	L"Artiste Extraordinaire",						// Norman \"NRG\" Olsen
	L"Gourou du son",											// George Brooks
	L"Conception �crans/Artiste",					// Andrew Stacey
	L"Artiste en chef/Animateur",						// Scot Loving
	L"Programmeur en chef",									// Andrew \"Big Cheese Doddle\" Emmons
	L"Programmeur",											// Dave French
	L"Syst�mes strat�giques & Programmeur",					// Alex Meduna
	L"Portraits",								// Joey \"Joeker\" Whelan",
};

const wchar_t* gzCreditNameFunny[]=
{
	L"", 																			// Chris Camfield
	L"(ah, la ponctuation...)",					// Shaun Lyng
	L"(\"C'est bon, trois fois rien\")",	//Kris \"The Cow Rape Man\" Marnes
	L"(j'ai pass� l'�ge)",				// Ian Currie
	L"(et en plus je bosse sur Wizardry 8)",						// Linda Currie
	L"(on m'a forc� !)",			// Eric \"WTF\" Cheng
	L"(partie en cours de route...)",	// Lynn Holowka
	L"",																			// Norman \"NRG\" Olsen
	L"",																			// George Brooks
	L"(T�te de mort et fou de jazz)",						// Andrew Stacey
	L"(en fait il s'appelle Robert)",							// Scot Loving
	L"(la seule personne un peu responsable de l'�quipe)",					// Andrew \"Big Cheese Doddle\" Emmons
	L"(bon, je vais pouvoir r�parer ma moto)",	// Dave French
	L"(piqu� � l'�quipe de Wizardry 8)",							// Alex Meduna
	L"(conception des objets et des �crans de chargement !)",	// Joey \"Joeker\" Whelan",
};

const wchar_t* sRepairsDoneString[] =
{
	L"%ls a termin� la r�paration de ses objets",
	L"%ls a termin� la r�paration des armes & armures",
	L"%ls a termin� la r�paration des objets port�s",
	L"%ls a termin� la r�paration des objets transport�s",
};

const wchar_t* zGioDifConfirmText[]=
{
	L"Vous avez choisi le mode de difficult� NOVICE. Ce mode de jeu est conseill� pour les joueurs qui d�couvrent Jagged Alliance, qui n'ont pas l'habitude de jouer � des jeux de strat�gie ou qui souhaitent que les combats ne durent pas trop longtemps. Ce choix influe sur de nombreux param�tres du jeu. Etes-vous certain de vouloir jouer en mode Novice ?",
	L"Vous avez choisi le mode de difficult� EXPERIMENTE. Ce mode de jeu est conseill� pour les joueurs qui ont d�j� jou� � Jagged Alliance ou des jeux de strat�gie. Ce choix influe sur de nombreux param�tres du jeu. Etes-vous certain de vouloir jouer en mode Exp�riment� ?",
	L"Vous avez choisi le mode de difficult� EXPERT. Vous aurez �t� pr�venu. Ne venez pas vous plaindre si vos mercenaires quittent Arulco dans un cerceuil. Ce choix influe sur de nombreux param�tres du jeu. Etes-vous certain de vouloir jouer en mode Expert ?",
};

const wchar_t* gzLateLocalizedString[] =
{
	L"Donn�es de l'�cran de chargement de %ls introuvables...",

	//1-5
	L"Le robot ne peut quitter ce secteur par lui-m�me.",

	//This message comes up if you have pending bombs waiting to explode in tactical.
	L"Compression du temps impossible. C'est bient�t le feu d'artifice !",

	//'Name' refuses to move.
	L"%ls refuse d'avancer.",

	//%ls a merc name
	L"%ls n'a pas assez d'�nergie pour changer de position.",

	//A message that pops up when a vehicle runs out of gas.
	L"Le %ls n'a plus de carburant ; le v�hicule est bloqu� � %c%d.",

	//6-10

	// the following two strings are combined with the pNewNoise[] strings above to report noises
	// heard above or below the merc
	L"au-dessus",
	L"en-dessous",

	//The following strings are used in autoresolve for autobandaging related feedback.
	L"Aucun de vos mercenaires n'a de comp�tence m�dicale.",
	L"Plus de bandages !",
	L"Pas assez de bandages pour soigner tout le monde.",
	L"Aucun de vos mercenaires n'a besoin de soins.",
	L"Soins automatiques.",
	L"Tous vos mercenaires ont �t� soign�s.",

	//14
	L"Arulco",

  L"(roof)",

	L"Sant� : %d/%d",

	//In autoresolve if there were 5 mercs fighting 8 enemies the text would be "5 vs. 8"
	//"vs." is the abbreviation of versus.
	L"%d contre %d",

	L"Plus de place dans le %ls !",  //(ex "The ice cream truck is full")

  L"%ls requiert des soins bien plus importants et/ou du repos.",

	//20
	//Happens when you get shot in the legs, and you fall down.
	L"%ls a �t� touch� aux jambes ! Il ne peut plus tenir debout !",
	//Name can't speak right now.
	L"%ls ne peut pas parler pour le moment.",

	//22-24 plural versions
	L"%d miliciens ont �t� promus v�t�rans.",
	L"%d miliciens ont �t� promus soldats.",
	L"%d soldats ont �t� promus v�t�rans.",

	//25
	L"Echanger",

	//26
	//Name has gone psycho -- when the game forces the player into burstmode (certain unstable characters)
	L"%ls est devenu fou !",

	//27-28
	//Messages why a player can't time compress.
	L"Nous vous d�conseillons d'utiliser la Compression du temps ; vous avez des mercenaires dans le secteur %ls.",
	L"Nous vous d�conseillons d'utiliser la Compression du temps lorsque vos mercenaires se trouvent dans des mines infest�es de cr�atures.",

	//29-31 singular versions
	L"1 milicien a �t� promu v�t�ran.",
	L"1 milicien a �t� promu soldat.",
	L"1 soldat a �t� promu v�t�ran.",

	//32-34
	L"%ls ne dit rien.",
	L"Revenir � la surface ?",
	L"(Escouade %d)",

	//35
	//Ex: "Red has repaired Scope's MP5K".  Careful to maintain the proper order (Red before Scope, Scope before MP5K)
	L"%ls a r�par� pour %ls : %ls",//inverted order !!! Red has repaired the MP5 of Scope

	//36
	L"Chat Sauvage",

	//37-38 "Name trips and falls"
	L"%ls tr�buche et tombe",
	L"Cet objet ne peut �tre pris d'ici.",

	//39
	L"Il ne vous reste aucun mercenaire en �tat de se battre. La milice combattra les cr�atures seule.",

	//40-43
	//%ls is the name of merc.
	L"%ls n'a plus de trousse de soins !",
	L"%ls n'a aucune comp�tence m�dicale !",
	L"%ls n'a plus de trousse � outils !",
	L"%ls n'a aucune comp�tence technique !",

	//44-45
	L"Temps de r�paration",
	L"%ls ne peut pas voir cette personne.",

	//46-48
	L"Le prolongateur de %ls est tomb� !",
	L"Seuls %d instructeurs de milice peuvent travailler par secteur.",
  L"Etes-vous s�r ?",

	//49-50
	L"Compression du temps",
	L"Le r�servoir est plein.",

	//51-52 Fast help text in mapscreen.
	L"Compression du temps (|E|s|p|a|c|e)",
	L"Arr�t de la Compression du temps (|E|c|h|a|p)",

	//53-54 "Magic has unjammed the Glock 18" or "Magic has unjammed Raven's H&K G11"
	L"%ls a d�senray� le %ls",
	L"%ls a d�senray� le %ls de %ls",//inverted !!! magic has unjammed the g11 of raven

	//55
	L"Compression du temps impossible dans l'�cran d'inventaire.",

	L"Le CD Play de Jagged Alliance 2 est introuvable. L'application va se terminer.",

	L"Objets associ�s.",

	//58
	//Displayed with the version information when cheats are enabled.
	L"Actuel/Maximum : %d%%/%d%%",

	//59
	L"Escorter John et Mary ?",

  L"Interrupteur activ�.",
};

#endif //FRENCH
