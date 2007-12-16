#ifndef __FLORIST_CARDS_H
#define __FLORIST_CARDS_H


#define		FLOR_CARD_TEXT_FILE							"BINARYDATA/FlowerCard.edt"
#define FLOR_CARD_TEXT_TITLE_SIZE 5 * 80


BOOLEAN EnterFloristCards(void);
void ExitFloristCards(void);
void HandleFloristCards(void);
void RenderFloristCards(void);


extern INT8			gbCurrentlySelectedCard;

#endif
