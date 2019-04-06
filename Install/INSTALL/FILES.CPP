/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 */

#include <files.h>

CXoslFiles::CXoslFiles()
{
	IgnoreRPM = false;
}

CXoslFiles::~CXoslFiles()
{
}

int CXoslFiles::GetIssuedFileCount()
{
	if (!IgnoreRPM) {
		return IssuedFileCount;
	}
	return IssuedFileCount - 2;
}

int CXoslFiles::GetPartFileCount()
{
	return PartFileCount;
}

const char *CXoslFiles::GetIssuedFileName(int Index)
{
	return IssuedFileList[Index];
}

const char *CXoslFiles::GetPartFileName(int Index)
{
	return PartFileList[Index];
}



const char *CXoslFiles::GetXoslDataName()
{
	return XoslData;
}

const char *CXoslFiles::GetBootItemName()
{
	return BootItem;
}

const char *CXoslFiles::GetIplFileName(TFileIpl IplToUse)
{
	return IplFileList[IplToUse];
}

const char *CXoslFiles::GetCurrentMbrName()
{
	return CurrentMBR;
}

const char *CXoslFiles::GetOriginalMbrName()
{
	return OriginalMBR;
}

const char *CXoslFiles::GetDefaultMbrName()
{
	return "DEFAULT.MBR";
}

const char *CXoslFiles::GetSmartBmName()
{
	return "SBM_IPL0.XCF";
}


void CXoslFiles::IgnorePartManFiles()
{
	IgnoreRPM = true;
}

const char *CXoslFiles::GetXoslLoadName()
{
	return XoslLoad;
}	

const char *CXoslFiles::GetXoslImgXName()
{
	return XoslImgX;
}


const char *CXoslFiles::XoslData = "XOSLDATA.XDF";
const char *CXoslFiles::BootItem = "BOOTITEM.XDF";
const char *CXoslFiles::CurrentMBR = "CURR_MBR.XCF";
const char *CXoslFiles::OriginalMBR = "ORIG_MBR.XCF";

const char *CXoslFiles::XoslLoad = "XOSLLOAD.XCF";

const char *CXoslFiles::XoslImgX = "XOSLIMG0.XXF";

/*
const char *CXoslFiles::FileList[] = {
 These are now done individually
	"XOSLLOAD.XCF",
	"XOSLIMG0.XXF","XOSLIMG1.XXF","XOSLIMG2.XXF","XOSLIMG3.XXF","XOSLIMG4.XXF","XOSLIMG5.XXF", "XOSLIMG6.XXF",


	"XOSLLOGO.XBF","XOSLWALL.XBF","SPLASHLG.XBF",
	"DEFAULT.XFF","EXTRA.XFF",
	"ORIG_MBR.XCF","CURR_MBR.XCF","SBM_IPL0.XCF",
	"BOOTITEM.XDF","XOSLDATA.XDF",
	"XRPART00.XXF","XRPART01.XXF"

	// Issued files
	"XOSLLOGO.XBF","XOSLWALL.XBF","SPLASHLG.XBF",
	"DEFAULT.XFF","EXTRA.XFF",
	"XRPART00.XXF","XRPART01.XXF"

	// Created files
	"ORIG_MBR.XCF","CURR_MBR.XCF","SBM_IPL0.XCF",
	"BOOTITEM.XDF","XOSLDATA.XDF",



};
*/
const char *CXoslFiles::IssuedFileList[] = {
	// Issued files 
	"XOSLLOGO.XBF","XOSLWALL.XBF","SPLASHLG.XBF",
	"DEFAULT.XFF","EXTRA.XFF",
	"XRPART00.XXF","XRPART01.XXF"
};

const int CXoslFiles::IssuedFileCount = sizeof (IssuedFileList) / sizeof (char *);

const char *CXoslFiles::PartFileList[] = {
	// Partition specific files
	"SBM_IPL0.XCF",
	"BOOTITEM.XDF","XOSLDATA.XDF",
};

const int CXoslFiles::PartFileCount = sizeof (PartFileList) / sizeof (char *);



const char *CXoslFiles::IplFileList[] = {
	"IPL06.BIN","IPL0B.BIN","IPL06LBA.BIN","IPL0BLBA.BIN",
	"IPLS.BIN","IPLSLBA.BIN"
};
