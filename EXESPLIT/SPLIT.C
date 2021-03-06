/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 *
 * Open Watcom Migration
 * Copyright (c) 2010 by Mario Looijkens:
 * - Rename function "main" into "mymain" to get rid of
 *   linker Error! E2028: _main is an undefined reference
 * - Comment out "TExeHeader ExeHeader;" in main function to ged rid of
 *   Warning! W202: Symbol 'ExeHeader' has been defined, but not referenced
 * - Add implementation to embed ImageCount value to image file. 
 *
 */

#include <io.h>
#include <exestruc.h>

void __cdecl WriteHeader(int ifh);
void __cdecl WriteData(int ifh);
void __cdecl mymain();

static const char *SrcFile = "xosl.exe";
static char *DestFile = "xoslimg0.xxf";

static char Buffer[32768];

//void main()
void mymain()
{
	int ifh;
	//TExeHeader ExeHeader;

	ifh = open(SrcFile,O_RDWR);
	WriteHeader(ifh);
	WriteData(ifh);
	close(ifh);

}

void WriteHeader(int ifh)
{
	TExeHeader ExeHeader;
	int ofh;
	unsigned short Count;

	ofh = creat(DestFile);
	read(ifh,&ExeHeader,sizeof (ExeHeader));
	lseek(ofh,0,2);
	write(ofh,&ExeHeader,sizeof (ExeHeader));
	Count = ExeHeader.HeaderSize * 16 - sizeof (TExeHeader);
	read(ifh,Buffer,Count);
	write(ofh,Buffer,Count);
	close(ofh);
	++DestFile[7];
}

void WriteData(int ifh)
{
	unsigned short Bytes;
	int ofh;
  int imgcount;
	
	while ((Bytes = read(ifh,Buffer,32768)) != 0) {
		ofh = creat(DestFile);
		write(ofh,Buffer,Bytes);
		close(ofh);
		++DestFile[7];
	}
	
	//Put (zero based) ImageCount value in xoslimg0.xxf
	imgcount=DestFile[7]-'1';
	DestFile[7]='0';
	ofh = open(DestFile,O_RDWR);
	write(ofh,&imgcount,sizeof (imgcount));
	close(ofh);
	
}
