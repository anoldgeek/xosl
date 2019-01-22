#ifndef _GPTTAB_
#define _GPTTAB_

typedef unsigned char __u8;

#define UUID_SIZE 16

/* 512 << 7 = 65536 byte sectors.  */
#define MAX_SECTOR_LOG 7

typedef unsigned char uint8_t;
typedef unsigned short uint16_t ;
typedef unsigned long uint32_t ;
typedef unsigned long long uint64_t ;

typedef struct {
        __u8 b[UUID_SIZE];
} uuid_t;

typedef struct s_gpt_header
{
  uint8_t magic[8];
  uint32_t version;
  uint32_t headersize;
  uint32_t crc32;
  uint32_t unused1;
  uint64_t primary;
  uint64_t backup;
  uint64_t start;
  uint64_t end;
  uuid_t guid;
  uint64_t partitions;
  uint32_t maxpart;
  uint32_t partentry_size;
  uint32_t partentry_crc32;
  unsigned char reserved[0x200 - 0x5c];
} gpt_header_t ;

typedef struct 
{
  uuid_t type;		// 16
  uint8_t guid[16];	// 16
  uint64_t start;	// 8
  uint64_t end;		// 8
  uint64_t attrib;	// 8
  char name[72];	// 72
} gpt_partentry_t;

typedef struct {
	gpt_partentry_t gptEntries[4];
} TGPTTable;


#if FALSE
#define UUID_INIT(a, b, c, d0, d1, d2, d3, d4, d5, d6, d7)                      \
                                                               \
{ ((a) >> 24) & 0xff, ((a) >> 16) & 0xff, ((a) >> 8) & 0xff, (a) & 0xff, \
   ((b) >> 8) & 0xff, (b) & 0xff,                                       \
   ((c) >> 8) & 0xff, (c) & 0xff,                                       \
   (d0), (d1), (d2), (d3), (d4), (d5), (d6), (d7) }
#else
#define UUID_INIT(a, b, c, d0, d1, d2, d3, d4, d5, d6, d7)                      \
{ (a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff,((a) >> 24) & 0xff,   \
   (b) & 0xff, ((b) >> 8) & 0xff,                                       \
   (c) & 0xff, ((c) >> 8) & 0xff,                                       \
   (d0), (d1), (d2), (d3), (d4), (d5), (d6), (d7) }

#endif
/*
 * The length of a UUID string ("aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee")
 * not including trailing NUL.
 */
#define UUID_STRING_LEN         36

typedef struct S_gpt_fstype
{
	uint16_t MBRType;
	uuid_t GUIDype;
	char Name[80];
} gpt_fstype_t ;

#endif

