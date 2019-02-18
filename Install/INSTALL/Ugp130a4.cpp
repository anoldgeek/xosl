
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
	unsigned char PartIndex;

	CBootItemFile *BootItemData;
	BootItemData = new CBootItemFile;
	memset(BootItemData,0,sizeof(CBootItemFile));

	// update array of BootItem
	limit = oldBootItemData->BootItemCount;
	CBootItem *newBootItem;
	pre130a4CBootItem *oldBootItem;
	for(i = 0; i < limit ; i++){
		newBootItem = &BootItemData->BootItems[i];
		oldBootItem = &oldBootItemData->BootItems[i];
		memcpy(newBootItem->ItemName,oldBootItem->ItemName,sizeof(oldBootItem->ItemName));

		// Adjust PartIndex to allow for addition of PART_MBR for HD0 at index 1
		PartIndex = oldBootItem->PartIndex;
		newBootItem->PartIndex = (PartIndex < 1) ? PartIndex : PartIndex +1;

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
	limit = oldBootItemData->PartCount;
	CPartDesc *newPartDesc;
	pre130a4CPartDesc *oldPartDesc;
	for(i = 0, j = 0; i < limit ; i++,j++){
		oldPartDesc = &oldBootItemData->PartList[i];
		newPartDesc = &BootItemData->PartList[j];
		newPartDesc->Drive = oldPartDesc->Drive;
		switch( i ){
			case 0:{
				// First entry is always PART_OMBR
				newPartDesc->StartSector = PART_OMBR;
				break;
			}
			case 1:{
				// Second entry should be PART_MBR
				// Add the missing PART_MBR
				newPartDesc->StartSector = 0;
				// next new PartDesc
				newPartDesc = &BootItemData->PartList[++j];
				newPartDesc->Drive = oldPartDesc->Drive;

				// Third Entry is always PART_SBM
				newPartDesc->StartSector = PART_SBM;
				break;
			}
			default:{
				if (oldPartDesc->Drive < 16){
					// Assume max of 16 floppy drives
					newPartDesc->StartSector = PART_FLOPPY;
				}
				else{
					newPartDesc->StartSector = oldPartDesc->StartSector;
				}
				break;
			}
		}
	}
	// update non array items
	BootItemData->BootItemCount = oldBootItemData->BootItemCount;
	BootItemData->DefaultItem = oldBootItemData->DefaultItem;
	BootItemData->PartCount = oldBootItemData->PartCount + 1; // We've add PART_MBR for HD0
	BootItemData->Timeout = oldBootItemData->Timeout;
	BootItemData->MbrHDSector0 = MbrHDSector0;
	BootItemData->BootItemVersion = CURRENT_BOOTITEM_VERSION;
	delete oldBootItemData;
	return BootItemData;
}
