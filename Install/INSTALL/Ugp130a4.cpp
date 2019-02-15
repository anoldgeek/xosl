
#include <items.h>
#include <mem.h>
#include <ptab.h>
#include <pre130a4.h>

CUpgrade::CUpgrade()
{
}

CUpgrade::~CUpgrade()
{
}

CBootItemFile* CUpgrade::upgradeBootItems(pre130a4CBootItemFile *oldBootItemData, unsigned char MbrHDSector0){

	int i,j;
	int limit;

	CBootItemFile *BootItemData;
	BootItemData = new CBootItemFile;
	memset(BootItemData,0,sizeof(CBootItemFile));

	// update array of BootItem
	limit = BI_BOOTITEMS > O_BI_BOOTITEMS?O_BI_BOOTITEMS:BI_BOOTITEMS;
	CBootItem *newBootItem;
	pre130a4CBootItem *oldBootItem;
	for(i = 0; i < limit ; i++){
		newBootItem = &BootItemData->BootItems[i];
		oldBootItem = &oldBootItemData->BootItems[i];
		memcpy(newBootItem->ItemName,oldBootItem->ItemName,sizeof(oldBootItem->ItemName));
		newBootItem->PartIndex = oldBootItem->PartIndex;
		newBootItem->Hotkey = oldBootItem->Hotkey;
		newBootItem->Activate = oldBootItem->Activate;
		newBootItem->FixDriveNum = oldBootItem->FixDriveNum;
		newBootItem->Disabled = oldBootItem->Disabled;
		newBootItem->Password = oldBootItem->Password;
		memcpy(newBootItem->Keys,oldBootItem->Keys,sizeof(oldBootItem->Keys));
		memcpy(newBootItem->HideList,oldBootItem->HideList,sizeof(oldBootItem->HideList));
		newBootItem->SwapDrives = oldBootItem->SwapDrives;
	}
	// update array of PartDesc
	limit = BI_PARTS > O_BI_PARTS?O_BI_PARTS:BI_PARTS;
	CPartDesc *newPartDesc;
	pre130a4CPartDesc *oldPartDesc;
	for(i = 0, j = 0; i < limit ; i++, j++){
		oldPartDesc = &oldBootItemData->PartList[i];
		if ( i == 1 && oldPartDesc->StartSector != 0 ){
			// Add missing HD0 MBR
			newPartDesc->Drive = oldPartDesc->Drive;
			newPartDesc->StartSector = 0;
			newPartDesc->Type = PART_MBR;
			j++;
		}
		newPartDesc = &BootItemData->PartList[j];

		newPartDesc->Drive = oldPartDesc->Drive;
		newPartDesc->StartSector = oldPartDesc->StartSector;
		if (i == 0 ) {
			// First item is always ombr
			newPartDesc->Type = PART_OMBR;
		}
		else{
			if(newPartDesc->StartSector == 0){
				// Sector 0 is always mbr
				newPartDesc->Type = PART_MBR;
			}
			else{
				newPartDesc->Type = 0; // Unable to determine Type on upgrade
			}
		}
	}
	// update non array items
	BootItemData->BootItemCount = oldBootItemData->BootItemCount;
	BootItemData->DefaultItem = oldBootItemData->DefaultItem;
	BootItemData->PartCount = oldBootItemData->PartCount;
	BootItemData->Timeout = oldBootItemData->Timeout;
	BootItemData->MbrHDSector0 = MbrHDSector0;
	BootItemData->BootItemVersion = CURRENT_BOOTITEM_VERSION;
	delete oldBootItemData;
	return BootItemData;
}
