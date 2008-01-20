#ifndef ITEMS_H
#define ITEMS_H

#include "Item_Types.h"
#include "JA2Types.h"


void DamageObj(OBJECTTYPE* pObj, INT8 bAmount);

extern UINT8 SlotToPocket[7];

BOOLEAN WeaponInHand(const SOLDIERTYPE* pSoldier);

INT8 FindObj(const SOLDIERTYPE* pSoldier, UINT16 usItem);
INT8 FindAmmo( SOLDIERTYPE * pSoldier, UINT8 ubCalibre, UINT8 ubMagSize, INT8 bExcludeSlot );

INT8 FindAttachment(const OBJECTTYPE* pObj, UINT16 usItem);
INT8 FindObjClass(const SOLDIERTYPE* s, UINT32 usItemClass);
extern INT8 FindAIUsableObjClass( SOLDIERTYPE * pSoldier, 	UINT32 usItemClass );
extern INT8 FindAIUsableObjClassWithin( SOLDIERTYPE * pSoldier, 	UINT32 usItemClass, INT8 bLower, INT8 bUpper );
extern INT8 FindEmptySlotWithin( SOLDIERTYPE * pSoldier, INT8 bLower, INT8 bUpper );
extern INT8 FindExactObj( SOLDIERTYPE * pSoldier, OBJECTTYPE * pObj );
INT8 FindObjInObjRange(const SOLDIERTYPE* s, UINT16 usItem1, UINT16 usItem2);
extern INT8 FindLaunchable( SOLDIERTYPE * pSoldier, UINT16 usWeapon );
extern INT8 FindGLGrenade( SOLDIERTYPE * pSoldier );
extern INT8 FindThrowableGrenade( SOLDIERTYPE * pSoldier );
extern INT8 FindUsableObj( SOLDIERTYPE * pSoldier, UINT16 usItem );

extern void DeleteObj(OBJECTTYPE * pObj );
extern void SwapObjs( OBJECTTYPE * pObj1, OBJECTTYPE * pObj2 );

extern void RemoveObjFrom( OBJECTTYPE * pObj, UINT8 ubRemoveIndex );
//Returns true if swapped, false if added to end of stack
extern BOOLEAN PlaceObjectAtObjectIndex( OBJECTTYPE * pSourceObj, OBJECTTYPE * pTargetObj, UINT8 ubIndex );
extern void GetObjFrom( OBJECTTYPE * pObj, UINT8 ubGetIndex, OBJECTTYPE * pDest );

extern BOOLEAN AttachObject( SOLDIERTYPE * pSoldier, OBJECTTYPE * pTargetObj, OBJECTTYPE * pAttachment );
extern BOOLEAN RemoveAttachment( OBJECTTYPE * pObj, INT8 bAttachPos, OBJECTTYPE * pNewObj );

UINT8	CalculateObjectWeight(const OBJECTTYPE* pObject);
UINT32 CalculateCarriedWeight(const SOLDIERTYPE* pSoldier);

extern UINT16 TotalPoints( OBJECTTYPE * pObj );
extern UINT16 UseKitPoints( OBJECTTYPE * pObj, UINT16 usPoints, SOLDIERTYPE *pSoldier );

extern BOOLEAN EmptyWeaponMagazine( OBJECTTYPE * pWeapon, OBJECTTYPE *pAmmo );
extern BOOLEAN CreateItem( UINT16 usItem, INT8 bStatus, OBJECTTYPE * pObj );
extern BOOLEAN CreateItems( UINT16 usItem, INT8 bStatus, UINT8 ubNumber, OBJECTTYPE * pObj );
extern BOOLEAN CreateMoney( UINT32 uiMoney, OBJECTTYPE * pObj );
extern UINT16 DefaultMagazine( UINT16 usItem );
UINT16 RandomMagazine( UINT16 usItem, UINT8 ubPercentStandard );
extern BOOLEAN ReloadGun( SOLDIERTYPE * pSoldier, OBJECTTYPE * pGun, OBJECTTYPE * pAmmo );

UINT8 ItemSlotLimit( UINT16 usItem, INT8 bSlot );

// Function to put an item in a soldier profile
// It's very primitive, just finds an empty place!
BOOLEAN PlaceObjectInSoldierProfile( UINT8 ubProfile, OBJECTTYPE *pObject );
BOOLEAN RemoveObjectFromSoldierProfile( UINT8 ubProfile, UINT16 usItem );
INT8 FindObjectInSoldierProfile( UINT8 ubProfile, UINT16 usItem );

BOOLEAN ObjectExistsInSoldierProfile( UINT8 ubProfile, UINT16 usItem );
void SetMoneyInSoldierProfile( UINT8 ubProfile, UINT32 uiMoney );

void CheckEquipmentForDamage( SOLDIERTYPE *pSoldier, INT32 iDamage );
BOOLEAN ArmBomb( OBJECTTYPE * pObj, INT8 bSetting );

// NOTE TO ANDREW:
//
// The following functions expect that pObj points to the object
// "in the cursor", which should have memory allocated for it already
BOOLEAN PlaceObject( SOLDIERTYPE * pSoldier, INT8 bPos, OBJECTTYPE * pObj );

// Send fNewItem to true to set off new item glow in inv panel
BOOLEAN AutoPlaceObject( SOLDIERTYPE * pSoldier, OBJECTTYPE * pObj, BOOLEAN fNewItem );
BOOLEAN RemoveObjectFromSlot( SOLDIERTYPE * pSoldier, INT8 bPos, OBJECTTYPE * pObj );

UINT8 SwapKeysToSlot( SOLDIERTYPE * pSoldier, INT8 bKeyRingPosition, OBJECTTYPE * pObj );

// create a keyobject
BOOLEAN CreateKeyObject( OBJECTTYPE * pObj , UINT8 ubNumberOfKeys, UINT8 ubKeyIdValue );
BOOLEAN DeleteKeyObject( OBJECTTYPE * pObj );
BOOLEAN AllocateObject( OBJECTTYPE **pObj );

// removes a key from a *KEYRING* slot
BOOLEAN RemoveKeyFromSlot( SOLDIERTYPE * pSoldier, INT8 bKeyRingPosition, OBJECTTYPE * pObj );

// take several
BOOLEAN RemoveKeysFromSlot( SOLDIERTYPE * pSoldier, INT8 bKeyRingPosition, UINT8 ubNumberOfKeys ,OBJECTTYPE * pObj );

// add the keys to an inventory slot
UINT8 AddKeysToSlot( SOLDIERTYPE * pSoldier, INT8 bKeyRingPosition, OBJECTTYPE * pObj );



//Kris:  December 9, 1997
//I need a bunch of validation functions for ammo, attachments, etc., so I'll be adding them here.
//Chris, maybe you might find these useful, or add your own.  I don't really know what I'm doing yet,
//so feel free to correct me...

//Simple check to see if the item has any attachments
BOOLEAN ItemHasAttachments(const OBJECTTYPE* pObj);

//Determine if this item can receive this attachment.  This is different, in that it may
//be possible to have this attachment on this item, but may already have an attachment on
//it which doesn't work simultaneously with the new attachment (like a silencer and duckbill).
BOOLEAN ValidItemAttachment(const OBJECTTYPE* pObj, UINT16 usAttachment, BOOLEAN fAttemptingAttachment);

//Determines if it is possible to equip this weapon with this ammo.
BOOLEAN ValidAmmoType( UINT16 usItem, UINT16 usAmmoType );

//Existing functions without header def's, added them here, just incase I'll need to call
//them from the editor.
BOOLEAN ValidAttachment( UINT16 usAttachment, UINT16 usItem );

BOOLEAN ValidLaunchable( UINT16 usLaunchable, UINT16 usItem );
UINT16 GetLauncherFromLaunchable( UINT16 usLaunchable );

BOOLEAN ValidMerge( UINT16 usMerge, UINT16 usItem );


// Is the item passed a medical kit?
BOOLEAN IsMedicalKitItem(const OBJECTTYPE* pObject);

BOOLEAN AutoReload( SOLDIERTYPE * pSoldier );
INT8 FindAmmoToReload( SOLDIERTYPE * pSoldier, INT8 bWeaponIn, INT8 bExcludeSlot );

void SwapHandItems( SOLDIERTYPE * pSoldier );

INT8 FindAttachmentByClass( OBJECTTYPE * pObj, UINT32 uiItemClass );
void RemoveObjs( OBJECTTYPE * pObj, UINT8 ubNumberToRemove );

void WaterDamage( SOLDIERTYPE *pSoldier );

INT8 FindObjWithin( SOLDIERTYPE * pSoldier, UINT16 usItem, INT8 bLower, INT8 bUpper );

BOOLEAN ApplyCamo(SOLDIERTYPE* pSoldier, OBJECTTYPE* pObj, BOOLEAN* pfGoodAPs);

BOOLEAN ItemIsLegal( UINT16 usItemIndex );
BOOLEAN ExtendedGunListGun( UINT16 usGun );
UINT16 StandardGunListReplacement( UINT16 usGun );
UINT16 FindReplacementMagazineIfNecessary( UINT16 usOldGun, UINT16 usOldAmmo, UINT16 usNewGun );

BOOLEAN DamageItemOnGround(OBJECTTYPE* pObject, INT16 sGridNo, INT8 bLevel, INT32 iDamage, SOLDIERTYPE* owner);

BOOLEAN ApplyCanteen( SOLDIERTYPE * pSoldier, OBJECTTYPE * pObj, BOOLEAN *pfGoodAPs );
BOOLEAN ApplyElixir( SOLDIERTYPE * pSoldier, OBJECTTYPE * pObj, BOOLEAN *pfGoodAPs );

BOOLEAN CompatibleFaceItem( UINT16 usItem1, UINT16 usItem2 );

UINT32 MoneySlotLimit( INT8 bSlot );

void CheckEquipmentForFragileItemDamage( SOLDIERTYPE *pSoldier, INT32 iDamage );

// Range of Xray device
#define XRAY_RANGE 40
// Seconds that Xray lasts
#define XRAY_TIME 5

extern void ActivateXRayDevice( SOLDIERTYPE * pSoldier );
extern void TurnOffXRayEffects( SOLDIERTYPE * pSoldier );
extern INT8 FindLaunchableAttachment( OBJECTTYPE * pObj, UINT16 usWeapon );

BOOLEAN CanItemFitInPosition(SOLDIERTYPE* s, OBJECTTYPE* pObj, INT8 bPos, BOOLEAN fDoingPlacement);

void    SetNewItem(SOLDIERTYPE* pSoldier, UINT8 ubInvPos, BOOLEAN fNewItem);
void    CleanUpStack(OBJECTTYPE* pObj, OBJECTTYPE* pCursorObj);
void    StackObjs(OBJECTTYPE* pSourceObj, OBJECTTYPE* pTargetObj, UINT8 ubNumberToCopy);
BOOLEAN ItemIsCool(const OBJECTTYPE* pObj);

#endif
