#ifndef BypassH
#define BypassH

#include <defs.h>
#include <ptab.h>
 
   
typedef struct {
	unsigned char Activated;
	unsigned char StartHead;
	unsigned short StartSectCyl;
	unsigned char FSType;
	unsigned char EndHead;
	unsigned short EndSectCyl;
	unsigned long StartSector;
	unsigned long SectorCount;
} TPTabEntry;

typedef struct {
	char Reserved[446];				// IPL in MBR
	TPTabEntry Table[4];				// The partition table
	unsigned short MagicNumber;	// should be 0xaa55
} TPTabSector;

	   
class CBypass {
public:
	CBypass();
	~CBypass();

	void Execute(const char *ErrorMsg = NULL);
private:
	TPTabSector MBR;

	void ReadMBR();
	int ReadVolumeLabel(int Drive, unsigned long Sector, char *VolumeLabel);

	void RawBoot(int BootIndex);

	void CheckPassword();

};

#endif
