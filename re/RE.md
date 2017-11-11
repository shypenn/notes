links
==============
* http://www.imf-conference.org/imf2006/program.html
* http://volatilityfoundation.org/  
	https://github.com/volatilityfoundation/
* http://www.nirsoft.net/  
	http://www.nirsoft.net/kernel_struct/vista/index.html
* Shiva anti–reverse engineering tool
	????

QT reversing
===================
* <https://www.codeproject.com/articles/31330/qt-internals-reversing>
* `private`: +4
* `QTextDocumentPrivate::text` --> +50
* string in `QTextItemInt(QTextItem): 4*9`
* string in QString: `poi(poi(addr)+c): [0]Data -> [3*4]data`
* QImage: `[08] QImageData: [18] char *`
* `QRasterPaintEngine::alphaPenBlt`: bits from CreateGDISection is translated to colors in QRasterBuffer: 0xffffffff -> color;
* `HB_Glyph`: index to font(the repository is in: `QTextEngine.layoutData/70->logClustersPtr`); 

delphi  
==========
* System::AnsiString:

		-8: refNumber
		-4: length
		00: data

general
============
* dos exe relocation entry is four bytes, the first word is offset, the second is segment.  the address it points is a segment address, which should be added by the "load module" address.

* go to far address to execute:
	retf

* important numbers:
		10		-0Ah
		100		-64h
		1000	-3e8h
		10000	-2710h

		0xAAAAAAAA	-10101010101010101010101010101010
		0x55555555	-01010101010101010101010101010101

		0x100			-256	-2^8	-4*2
		0x200			-512
		0x400			-1k		-2^10	-4*2+2
		0x1000			-4k
		0x10000			-64k
		0x100000		-1M		-2^20	-4*5
		0x4000,0000		-1G		-2^30	-4*7+2
		0x4,0000,0000	-4G

* basic block:
	* only one entry and one exit;
	* each instruction will be executed;
	* bp is only needed to set on the entry;

* in x64 calling convention, the reserved registers are R12-R15 besides the four ones in x32 (RBX, RBP, RSI, RDI);

* the prefix bytes placed before 90 (nop/`xchg ax, ax`) acts as padding;

* DEBUG:
	* the first 100h bytes for user stores important information;
	* `-w`: start from 100h if no address specified;

* "process monitor" may show incorrect base symbol in stack; however RetAddr is correct;

* encoding name - page mapping:

		gb2312  - cp20936
		gbk     - cp936
		utf8    - cp65001
		utf16le - 1200
		utf16be - 1201

* double-byte char set (DBCS) is actually ascii string;

* multiple bytes code encoding, read from low to high:

		81 ０ １ ２ ３ ４ ５ ６ ７ ８ ９ Ａ Ｂ Ｃ Ｄ Ｅ Ｆ
		４ 丂 丄 丅 丆 丏 丒 丗 丟 丠 両 丣 並 丩 丮 丯 丱
		５ 丳 丵 丷 丼 乀 乁 乂 乄 乆 乊 乑 乕 乗 乚 乛 乢
		６ 乣 乤 乥 乧 乨 乪 乫 乬 乭 乮 乯 乲 乴 乵 乶 乷

	丢：8147;	in memory as word: 4781;

* borland fastcall aka 'register calling convention':
	* pass parameters from left to right 
	* parameters passed in sequentially `EAX, EDX, ECX, stack`
	* stack is cleaned by callee;

* in 'thiscall'/method, 'ecx' is actually `*this`, e.g. what 'this' points is stored in 'ecx' (this object memory address);

* when a function returns a structure, usually it's created by the caller, and its address is passed as a parameter; so the parameters are more than it's prototype; 

* qword may be aligned at address starting from '0' or '8', leave space unused between members;

* processor breakpoint;
	* address is virtual;
	* part of processor context is based on thread (debugger replicates it to all threads within the same process);
	* only the arithmetic equalty of address is checked (won't catch if the memory is accessed through another address number);
	* won't catch if processer context is changed to another which doesn't have this debugging setting (another process; kernel mode);

* compiler may add another argument for returned object, which is allocated in the caller;

* c++ object has its first/last member pointing to a 'virtual function table' if it has; in derived object initialization, this table is duplicated and fixed for those overridden virtual methods;

* multiply substitutions - `imul eax, 5`:
	* `lea ecx, eax+eax*4`
	* `shl eax, 2`  
	  `add eax, eax`

* fool disassembler:
	* call target is also the return address:  
	  return address is disassembled first, then real flow is hidden;
	* jump to middle of itself; disassembler stops, manually processing has to be initiated;
	* wait until another thread modified codes;
	* modify codes/ip in SEH handler;  
	  **when caught in debugger, let go with unhandled; if not SEH handler is not invoked. (windbg: gn)**  
	  **windbg: g/gh: execute instruction following `int 3`**

