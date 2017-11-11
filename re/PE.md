PE FILE FORMAT:
========================
#### DOS_HEADER
```
{
	00 WORD   e_magic;                     // Magic number (4d 5a -MZ)				----					
	02 WORD   e_cblp;                      // Bytes on last page of file				|
	04 WORD   e_cp;                        // Pages in file								|
	06 WORD   e_crlc;                      // Number of Relocation entries				|
	08 WORD   e_cparhdr;                   // Size of header in paragraphs (4)			|
	0a WORD   e_minalloc;                  // Minimum extra paragraphs needed			|
	0c WORD   e_maxalloc;                  // Maximum extra paragraphs needed			|-- DOS exe header
	0e WORD   e_ss;                        // Initial (relative) SS value				|
	10 WORD   e_sp;                        // Initial SP value							|
	12 WORD   e_csum;                      // Checksum									|
	14 WORD   e_ip;                        // Initial IP value							|
	16 WORD   e_cs;                        // Initial (relative) CS value				|
	18 WORD   e_lfarlc;                    // File address of relocation table (0x40)	|
	1a WORD   e_ovno;                      // Overlay number						----

	1c WORD   e_res[4];                    // Reserved words						------- Unused
	24 WORD   e_oemid;                     // OEM identifier (for e_oeminfo)
	26 WORD   e_oeminfo;                   // OEM information; e_oemid specific
	28 WORD   e_res2[10];                  // Reserved words						------- Unused
	3c LONG   e_lfanew;                    // File address of PE header
}	

* fileSize = e_cp*512 (e_cblp==0)	or	(e_cp-1)*512 + e_cblp
* headerSize = e_cparhdr * 16
* imageSize = @fileSize - @headerSize	(load to memory starting after the header)IMAGE_FILE_HEADER 
* uninitialized = e_minalloc * 16
* extra = e_maxalloc * 16
* memorySize = between (@imageSize + @uninitialized) and (@imageSize + @uninitialized + @extra)
```

#### IMAGE_NT_HEADERS (PE header)
```
{
	00	LONG	PE_SIGNATURE (50 45 00 00 - PE..)
	04	14		IMAGE_FILE_HEADER/COFF_HEADER
	18	e0/f0	OPTIONAL_HEADER 
}
```


#### COFF_HEADER
```
{
	00	WORD	Target Machine type
	02	WORD	Number Of Sections
	04	LONG	Time Date Stamp (higher bytes are omitted)
	08	LONG	File offset to COFF symbol table (PE doesn't use this to debug anymore)
	0c	LONG	Number of symbols
	10	WORD	Size of optional header (PE only)
	12	WORD	characteristics
}

* the characteristics of IMAGE_FILE_32BIT_MACHINE(0x100) is also set on x64;
```

#### OPTIONAL_HEADER (image only)
```
{
	00		WORD	Magic (0x10b - PE32; 0x107 - ROM; 0x20b - PE32+(x64))	----
	02		BYTE	Linker Major Version									|
	03		BYTE	Linker Minor Version									|
	04		LONG	sum file size of all CODE sections						|
	08		LONG	sum file size of all INITIALIZED_DATA sections			|- standard
	0c		LONG	sum file size of all UNINITIALIZED_DATA sections		|	? maybe always zero;
	10		LONG	OEP (RVA)												|
	14		LONG	Base of Code (headers before)							|
	18		LONG	Base of Data (PE32 only)(codes before)				----	* VC set it as the same as that of code.

	1c/18 	LONG/LL	Image Base (stack and heap before)									------------   * maybe always 40,0000;
	20		LONG	section alignment in memory (default to architecture's page size)				|
	24		LONG	section alignment in file (512(default) - 64k)(no more than memory alignment)	|
	28		WORD	Major OS version																|
	2a		WORD	Minor OS version																|
	2c		WORD	Major Image version																|
	2e		WORD	Minor Image version																|
	30		WORD	Major Subsystem version  (* is the same with OS if windows app)					|
	32		WORD	Minor Subsystem version															|
	34		LONG	Win32 version (reserved)														|- windows speicific
	38		LONG	Size of Image (memory alignment)(including headers)								|
	3c		LONG	Size of Header (file alignment)(loaded into the first section)					|
	40		LONG	Checksum (only checked for drivers and other critical images)					|
	44		WORD	subsystem																		|
	46		WORD	dll characteristics																|
	48		LONG/LL	stack reserved																	|
	4c/50	LONG/LL	stack committed																	|
	50/58	LONG/LL	heap reserved																	|
	54/60	LONG/LL	heap committed																	|
	58/*	LONG	Loader flags (reserved)			[*: 16 added from this now on]					|
	5c		LONG	number of directory entries (16)									------------


																 ---- 0		LONG	RVA
	60		Export Directory Table 	- (IMAGE_DATA_DIRECTORY) ---|
																 ---- 4		LONG	Size (* valid to reserve space)
	68		Import Directory Table
	70		Resource Table
	78		Exception Table
	80		Certificate Table
	88		Base Relocation Table
	90		Debug
	98		Architecture (reserved)
	a0		Global pointer register (size:0)
	a8		TLS table
	b0		Load Config table
	b8		Bound import 
	c0		import address table (IAT)	-useless
	c8		Delay-load import table 
	d0		CLR header
	d8		reserved
}
```

* dll characteristics:
	* `IMAGE_DLL_CHARACTERISTICS_DYNAMIC_BASE(40h)`            - ASLR;
	* `IMAGE_DLL_CHARACTERISTICS_NX_COMPAT(100h)`              - DEP
	* `IMAGE_DLL_CHARACTERISTICS_TERMINAL_SERVER_AWARE(8000h)` - machine wide registry or directory access will not be redirected to a virtual one to isolate from real system data;



#### SECTION_HEADER
```
{
	00		LL		Name
	08		LONG	size in memory (not aligned) (image only)
	0c		LONG	memory Address (image only)
	10		LONG	Size in file (fileAlignmemt) (zero if uninitialized data)
	14		LONG	file offset
	18		LONG	file offset to Relocations (object only)
	1c		LONG	file offset to line numbers (object only)
	20		WORD	number of Relocations (object only)
	22		WORD	number of line numbers (object only)
	24		LONG	characteristics
}
```
* size in memory may be less than in file, since the former is not rounded (the actual size of data) but the latter does. 


COFF FILE FORMAT:
========================

	COFF_HEADER + SECTION_HEADERs + SECTIONs

 * no image address (RVA) and size;

### Export section:
```
		functions						strings [ExportNameTable]
			|								|
list of function address 		list of name string address (ordered alphabetically)
 [ExportAddressTable]				[NamePointerTable] --------------------------
						\		/										  		 | - indices are correspondent 
	list of index into function address list for name pointers					 |
					[ExportOrdinalTable] [WORD] ----------------------------------
```
* the pos of function name address in NamePointerTable is the same with the pos of the function address index in ExportOrdinalTable.

	i = search_ExportNamePointerTable (ExportName);

	i2 = ExportOrdinalTable [i];

	SymbolRVA = ExportAddressTable [i2];

* the Ordinal appearing in spec is used only in ImportLookupTable, which is index into ExportAddressTable plus OrdinalBase; 


#### [ExportDirectoryTable] - describes the info of other tables (single row):
```
 00		4		flags (reverved)
 04		4		stamp
 08		2		major version
 0a		2		minor version
 0c		4		address of ExportNameTable
 10		4		Ordinal Base (substracted from Ordinal to get real index into ExportAddressTable)
 14		4		number of ExportAddressTable
 18		4		number of NamePointerTable
 1c		4		address of ExportAddressTable
 20		4		address of NamePointerTable
 24		4		address of ExportOrdinalTable
```

### Import Section:

#### [ImportDirectoryTable] - each row describes one dll (ends with a NULL row):
```
 00		4		import lookup table RVA (usually empty, contained in IAT)
 04		4		stamp
 08		4		forwarder chain
 0c		4		dll name RVA
 10		4		import address table (IAT) RVA (the same with lookup talbe before bound)
```

#### [ImportLookupTable] - ends with null entry;
```
 31b		- Type of Ordinal (selects one of the next two formats: true for Ordinal)
 30-0b		- Hint/Name RVA
 15-0b		- Ordinal (substract OrdinalBase, to get the index to ExportAddressTable)
```

#### [Hint/NameTable] (no start RVA specified);
```
 00		2		Hint			- index into export NamePointerTable/OrdinalTable (check if strings match, if not search binarily) (not Ordinal, no subtraction required);
 02		*		Name			- ASCII string (case sensitive and null ended)
 *		0/1		PAD				- pad to be even if not
```

### (Image) Base Relocation Section:
			 --	00		4		Page RVA
			|	04		4		Size (including Page RVA and Size)
			|								--4 bit		-TYPE 
	BLOCK --|	08		2		entry	---|
			|								--12 bit	-OFFSET 
			 __	...

	BLOCK
	...

* one block contains relocations for 4k (12 bits); 
* so, offset is relative to Page RVA.
* order is important;
* no empty entry allowed between blocks;
* only required when Image Base is not loaded in specified address.
* the number of block entry should be even, e.g. Block starts on a 4-byte boundary; padding entry has type of IMAGE_REL_BASED_ABSOLUTE(0). 
* the accurate total size is in Relocation `IMAGE_DATA_DIRECTORY` in optional header. (relocation section header shouldn't be trusted).

NOTES
===============
* secion name is not meaningful to os, but for human;

* in vc++ `#pragma data_seg( "MY_DATA" )` sets data section name as "MY_DATA" instead of the default ".data";

* each section starts with a multiple of "alignment value" specified in the header of the PE, there are two of that value, one of which for memory and the other for file;

* RVA	- relative virtual address (relative to base image address (VA));

* e_lfanew in _IMAGE_DOS_HEADER is at offset 0x3c(60B) which is the offset value of IMAGE_NT_HEADERS;

* imported function address is specified in IAT when load, so their address is unknown when compile, rather its pionter address is fixed; then the way to call imported function is indirect.

		-> call [funPointerAddrInIAT];

	* however, if not specified, the compiler has no way to know which function is in external image; assuming the function is within the same image, the direct way is used:

			-> call funAddr;

	* then the job to fix that problem is left to linker, which insert an instruction to indirectly jump to that function; in turn, this instrcution is called by all those calls calling the imported function:

			-> call jmpAddr;
			-> jmpAddr: jmp [funPointerAddrInIAT];

	* the way to notify compiler a function is imported:

			-> declspec(dllimport) void funName(); (or after void).

* specific purpose sections include a Directory Table, which may be not from its beginning, rather specified by IMAGE_DATA_DIRECTORY in optional header; take '.idata' section for example, the Import Directory Table may be not starts from the beginning of that section.  

* several specific purpose sections may be combined in a single one.

