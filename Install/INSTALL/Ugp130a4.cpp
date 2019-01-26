
#include <items.h>
#include <mem.h>
//#include <Memory_x.h>
#include <pre130a4.h>

CUpgrade::CUpgrade()
{
}

CUpgrade::~CUpgrade()
{
}

CBootItemFile* CUpgrade::upgradeBootItems(pre130a4CBootItemFile *oldBootItemData, unsigned char MbrHDSector0){

	int i;
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
	for(i = 0; i < limit ; i++){
		newPartDesc = &BootItemData->PartList[i];
		oldPartDesc = &oldBootItemData->PartList[i];
		newPartDesc->Drive = oldPartDesc->Drive;
		newPartDesc->StartSector = oldPartDesc->StartSector;
	}
	// update non arry items
	BootItemData->BootItemCount = oldBootItemData->BootItemCount;
	BootItemData->DefaultItem = oldBootItemData->DefaultItem;
	BootItemData->PartCount = oldBootItemData->PartCount;
	BootItemData->Timeout = oldBootItemData->Timeout;
	BootItemData->MbrHDSector0 = MbrHDSector0;
	delete oldBootItemData;
	return BootItemData;
}
