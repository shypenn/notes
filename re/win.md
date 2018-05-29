* [windows xp source codes](https://doxygen.reactos.org/)
	
* `WM_COMMAND`

	| `Message_Source` | `wParam` (high word)              | `wParam`(low word)               | `lParam`                     |
	|------------------|-----------------------------------|----------------------------------|------------------------------|
	| Menu             | 0                                 | Menu identifier (`IDM_*)`        | 0                            |
	| Accelerator      | 1                                 | Accelerator identifier (`IDM_*`) | 0                            |
	| Control          | Control-defined-notification-code | Control identifier (`*1`)        | Handle to the control window |

	**Controls send information to dialog box procedure through the `WM_COMMAND` message, the id of which is in wParam**

* ways to find user defined event handler:
	- Through API (RegisterClass, DialogBoxParam...);
	- olly: Pause -> Run to user code -> Trigger an event;
	- set event breakpoint (usually `WM_COMMAND`/111h) on builtin ClassProc, then trigger the event and step into manually;

* VB api for comparing:

	_vbaVarTstEq, _vbaVarTstNe, _vbaVarCmpEq, _vbaStrCmp, _vbaStrComp, _vbaStrCompVar, 

* exception handling:

	CommonDispatchException(create ExceptionRecord, ExceptionFrame) -> KiDispatchException

* kernel mode api prefix:

	| Prefix | Component                  | Example                   |
	|--------|----------------------------|---------------------------|
	| Ex     | Executive                  | ExAllocatePool            |
	| Hal    | Hardware abstraction layer | HalGetBusData             |
	| Io     | I/O manager                | IoAllocateIrp             |
	| Ke     | Kernel                     | KeBugCheck                |
	| Ks     | Kernel streaming           | KsAcquireControl          |
	| Mm     | Memory manager             | MmGetPhysicalAddress      |
	| Ob     | Object manager             | ObReferenceObjectByHandle |
	| Po     | Power management           | PoSetSystemState          |
	| Se     | Security                   | SeAccessCheck             |
	| Tdi    | Transport driver interface | TdiProviderReady          |
	| Zw     | nothing                    | ZwCancelTimer             |

* call system service:
	- from user mode(nt/zw totally same):

		```
		API (store service index) 
		-> SystemCallStub (store esp, sysenter) 
		-> KiFastCallEntry (trap frame, enable interrupt) 
		-> KiSystemService (set previous mode, copy params from stack, get index) 
		-> KiServiceTable[index]
		```

	- from kernel mode:
		+ nt: KiServiceTable[index] (the last step of in user mode)	-- **not safe**
		+ zw: KiSystemService (the second to the last step of in user mode)

* system service will validate parameters if called from user mode (check the "previous mode").

* there are two types of handle table, one is specific to a certain process, another is global.

* in DOS, the "int 21" functions routine is located in `CS:[functionIndex*2+base]`

* x64 calling convention:

	**32 bits shadow memory is reserved even if no parameters**

* the data structure for process includes three parts:

		_KPROCES  - kernel
		_EPROCESS - executive
		_PEB      - subsystem
 
	- `_KPROCES` is the first member of `_EPROCES` (embeded);		
	- `_EPROCES` has a member of type of pointer to `_PEB`; 	
	- `_EPROCES` resides in system address space;

* address space layout:
	- kernel mode memory is mapped to each process address space;
	- x86:  
		user space occupies the starting 2G(0 - 7fff`0000) (or 3G if the characteristics of PE header includes `IMAGE_FILE_LARGE_ADDRESS_AWARE`;
	- x64:  
		currently only 48-bit is implemented in CPU, and 44-bit in windows 64; (**sign extended**)
		+ user space starts from zero up (``0 - 000007ff`ffffffff``)
		+ system space starts from top down (``ffffffff`ffffffff - fffff800`00000000``)

* when wow64 executable runs, the 64-bit ntdll runs first, then followed by 32-bit ntdll running;

* the first 0x38 bytes of `_TEB` is `_NT_TIB`;
	```
	[ntdll!_NT_TIB64]       
	00 ExceptionList        : Ptr64 _EXCEPTION_REGISTRATION_RECORD
	08 StackBase            : Ptr64 Void
	10 StackLimit           : Ptr64 Void
	18 SubSystemTib         : Ptr64 Void
	20 FiberData            : Ptr64 Void
	20 Version              : Uint4B
	28 ArbitraryUserPointer : Ptr64 Void
	30 Self                 : Ptr64 _NT_TIB
	```

* to determine if a process is running in wow64: 
	- IsWow64Process 
	- NtQueryInformationProcess(,ProcessWow64Information,...)

	along with GetSystemInfo() to determine the process bitness;

* the type of loaded module list in `_PEB_LDR_DATA` is `_LDR_DATA_TABLE_ENTRY`; 

* `_LIST_ENTRY` points to the field itself, instead of the beginning of the structure;

* at the start of thread entry, the address of which is contained in `EDX` set by `ntdll!RtlUserThreadStart`; 

* teb is pointed by fs in x86 and gs in x64; the other is invalid to use;
	- get PEB:
		
		fs:0 -> teb -> peb


* paste from app UI requires the app running;

* when string is encoded in codepage other than unicode, call function with suffix `A`; which will convert it to unicode string then call `W`;
	- gdi32!GetCodePage(NtGdiGetCharSet) returns codepage from hdc;

* usually in flat mode;
	- data & code segments each contains the whole address space;
	- fs is an exception;

* GetThreadSelectorEntry -> GDT/LDT entry;

* synchronizable object starts with `_DISPATCHER_OBJECT`, such as `_EPROCESS/_KPROCESS`, `_ETHREAD`;

* PoolTag is the last member of `_POOL_HEADER`, which preceding the allocated pool; 
	- can only contain ascii values;
	- the tags allocated by kernel has its highest bit set, in case of overriden by third party components;
	- used to identify kernel specific data;

* VirtualAlloc -> VAD -> PTE;
	- VAD tree root in: `_EPROCESS.VadRoot`
	- requirements set in VirtualAlloc are stored in VAD, which is consulted when allocate PTE;

* deriving: `_MMVAD_SHORT -> MMVAD -> _MMVAD_LONG`

* `u` mmeber of `_MMVAD` is of type `_MMVAD_FLAGS`, which includes `Protection`, `NoChange` and others; 

* vadRoot/vadNode has different symbol name in different os;

	| OS         | VadRoot       | VadNode                         |
	|------------|---------------|---------------------------------|
	| XP/03      | ptr32         | _MMVAD_LONG/_MMVAD/_MMVAD_SHORT |
	| Vista/7/08 | _MM_AVL_TABLE | _MMADDRESS_NODE                 |
	| 8/12       | _MM_AVL_TABLE | _MM_AVL_NODE                    |
	| 8.1/12r2   | _MM_AVL_TREE  | _MM_BALANCED_NODE               |

* when execution is transfered to application, `ebx` points to `PEB`;

* winsock
- `wsock32.lib`: v1
- `ws2_32.lib`: v2

* int has length of 32bits both in x86 & x64.
