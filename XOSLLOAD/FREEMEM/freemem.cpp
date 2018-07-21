/*
 * Extended Operating System Loader (XOSL)
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 *
 *
 */

/*
#ifdef __cplusplus
extern "C" {
#endif

void __cdecl  unsigned long FreememStart(unsigned short Status);

#ifdef __cplusplus
};
#endif 
*/

#include <freemem.h>


unsigned long far FreeMemStart(unsigned long addr)
{
	// Place this in BSS. So that it is at the end of BSS, this must be the last linked object.
//	static unsigned long addr;

	unsigned long size,page,offset;

//	size = (unsigned long)&addr;
	size = (unsigned long)addr;
	page = size & 0xffff0000;
	offset = size & 0xffff;
	page = page + ((offset & 0xfff0) << 12);
	if ( offset & 0x0f)
		page = page + 0x00010000;
	return page;
}
