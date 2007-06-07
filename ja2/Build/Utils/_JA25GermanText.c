#include "Language_Defines.h"
#ifdef ENGLISH
#	include "Text.h"
#endif
#include <wchar.h>


#ifdef GERMAN

// VERY TRUNCATED FILE COPIED FROM JA2.5 FOR ITS FEATURES FOR JA2 GOLD

const wchar_t *zNewTacticalMessages[]=
{
	L"Entfernung zum Ziel: %d Felder",
	L"Verbinden Sie den Transmitter mit Ihrem Laptop-Computer.",
	L"Sie haben nicht genug Geld, um %s anzuheuern",
	L"Das obenstehende Honorar deckt f�r einen begrenzten Zeitraum die Kosten der Gesamtmission, und schlie�t untenstehendes Equipment mit ein.",
	L"Engagieren Sie %s jetzt und nutzen Sie den Vorteil unseres beispiellosen 'Ein Betrag f�r alles'-Honorars. Das pers�nliche Equipment des S�ldners ist gratis in diesem Preis mit inbegriffen.",
	L"Honorar",
	L"Da ist noch jemand im Sektor...",
	L"Waffen-Rchwt.: %d Felder, Entf. zum Ziel: %d Felder",
	L"Deckung anzeigen",
	L"Sichtfeld",
	L"Neue Rekruten k�nnen dort nicht hinkommen.",
	L"Da Ihr Laptop keinen Transmitter besitzt, k�nnen Sie keine neuen Teammitglieder anheuern. Vielleicht ist dies eine guter Zeitpunkt, ein gespeichertes  Spiel zu laden oder ein neues zu starten!",
	L"%s h�rt das Ger�usch knirschenden Metalls unter Jerry hervordringen. Es klingt gr�sslich - die Antenne ihres Laptop-Computers ist  zerst�rt.",  //the %s is the name of a merc.  @@@  Modified
	L"Nach Ansehen des Hinweises, den Commander Morris hinterlie�, erkennt %s eine einmalige Gelegenheit. Der Hinweis enth�lt Koordinaten f�r den Start von Raketen gegen verschiedene St�dte in Arulco. Aber er enth�lt auch die Koordinaten des Startpunktes - der Raketenanlage.",
	L"Das Kontroll-Board studierend, entdeckt %s, dass die Zahlen umgedreht werden k�nnten, so dass die Raketen diese Anlage selbst zerst�ren. %s muss nun einen Fluchtweg finden. Der Aufzug scheint die schnellstm�gliche Route zu bieten...",         //!!! The original reads:	L"Noticing the control panel %s, figures the numbers can be reversed..." That sounds odd for me, but I think the comma is placed one word too late... (correct?)
L"Dies ist ein IRON MAN-Spiel, und es kann nicht gespeichert werden, wenn sich Gegner in der N�he befinden.",
	L"(Kann w�hrend Kampf nicht speichern)",
	L"Der Name der aktuellen Kampagne enth�lt mehr als 30 Buchstaben.",
	L"Die aktuelle Kampagne kann nicht gefunden werden.",
	L"Kampagne: Standard ( %ls )",
	L"Kampagne: %ls",
	L"Sie haben die Kampagne %ls gew�hlt. Diese ist eine vom Spieler modifizierte Version der Originalkampagne von JA2UB. M�chten Sie die Kampagne %ls spielen?",
	L"Um den Editor zu benutzen, m�ssen Sie eine andere als die Standardkampgane ausw�hlen.",
};

//@@@:  New string as of March 3, 2000.
const wchar_t *gzIronManModeWarningText[]=
{
	L"You have chosen IRON MAN mode. This setting makes the game considerably more challenging as you will not be able to save your game when in a sector occupied by enemies. This setting will affect the entire course of the game.  Are you sure want to play in IRON MAN mode?",
};


#endif
