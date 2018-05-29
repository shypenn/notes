read teb in x64?

##	.ALIAS

- alias to string;

- check if exist: ${/d:}, ${/f:}

- name and equivalent/value are case sensitive;

- replace only when enter a new block;

- won't replace if the block starts with alias-related commands(as/aS/ad...); a leading space can overcome this;

- when interpreting, name can only be adjascent with ';' and '{}'
 * r@$t10= $arg1;
 * r@$t10=${$arg1};

- options should be placed before name, right after command;
	as /ma name equal

- in ad/as/aS, ${/v:Name} always is replaced with Name(prevent expansion);

- /ma can interpreret DBCS string;

- fixed-name alias ($u0-9) is set by 'r', and with '.' prefix to 'u':
	r $.u0 = afafaf
 * every thing after equal sign except of leading and trailing spaces is treated as equivalent, even ';';
 * surround it in block to end the equivalent;

- aS does't accept semi-colon, except included in quots; quite fit in script file;
 * 'as' treats the whole line as equivalent, if not included in a block.

- ${/f/d:} once is replaced to 0 or empty string, has nothing to do with its original alias name; 
 * only one time replacement is allowed;
 * prevent from replacing in the block outerer than the block in which value is set:
	ad /q *
	.while(1){ad /q DLL					-- if no this line, ${/f:DLL} was already replaced by "", then value set to DLL can't affect value of "";
		aS DLL dll_name
		.block{
			.echo ${/f:DLL}
		}
	}
 	
- all aliases in a block (including sub-blocks) are replaced when enter each time; ( aliases in sub-blocks are replaced multiple times)
 * script file is a block;
 * command box is a block;
 * .block{}
   **exception: if the block starts with al/as/ad, replacing is cancelled;**
   .) no space space before 'al/...';
   .) no error;
 * conditional bp with alias(no space at the start):
```
al; bp USER32!DrawTextA "aS ${/v:str} str; db poi(esp+8) l10; aS /ma ${/v:str} poi(esp+8); .block{.echo str; j $spat(\"${str}\", \"未找到或*\") ; gc};"
```

- aliases preserved between sessions;
- will not be replaced when combined with whatever other chars; (surround in interpreter or use fixed-name alias)
- recursive replacing only occurs when as/aS is prefixed with ';' (or fixed-name alias)
- after an alias is set and replaced, it doesn't work to replace again, since it is not the alias already;
- ${/f,d} will always be replaced the first time; 
- alias surrounded by escaped quotes should be seperated by ${};
	\"CLS\"	->	\"${CLS}\"
 * this is not needed:
 	\"CLS \"

	
##	.NET

- .NET Framework runtime dll:
 * before 4.0: mscorwks.dll
 * after 4.0: clr.dll

- sos.dll has to be the corresponding version with runtime dll:
 .loadby sos mscorwks/clr
 20802033 be, 63, 84

- type name is case sensitive;


- the process to find a method:
 * list modules with address;
 	:!dumpDomain
 * list all types with methodTableAddr in a module(mt - managed types):
 	:!dumpModule -mt moduleAddr
 * list all methods in the class and its address(MT -method table; md -methods):
 	:!dumpMT -md methodTableAddr
 	
- to list breakpoints:
 :!bpmd -list
 * in old version, this doesn't work, instead:
 	:bl
	!ip2md addr	(instruction pointer to method)

- get info from name: !name2ee
 * get module or class info from a method (the method name can be whatever):
	 :!name2ee moduleName/* methodName
- no extension and quotes
	:sxe ld mscorjit

- to load sos.dll at the very first, after Common Language Runtime is loaded:
	sxe ld mscorjit
	.loadby sos.dll mscorwks/clr
 * load after 'mscorjit' rather than 'mscorwks/clr', to set bp on Main successfully;
 * 'dll' in 'sos.dll' can prevent the bug that the first command fail to run;

## extension

- Starting in Windows Driver Kit (WDK) 8, MSBuild replaced the Windows Build Utility (Build.exe), extension has to be built in vs.

- wdk (which requires sdk) is required: driver dll.

- DbgEng framework supersedes WdbgExts, EngExtCpp is based on DbgEng;

- Upper case letter in command name can not be recognized in kernel mode;

- IDebugControl.Output:
%s	--in debugger address space.
%p, %ma	--in debuggee address space.

-Output reads incorrect data sometimes when more than one items mixed in a call. such as
	pCtrl->Output(DEBUG_OUTPUT_NORMAL, "----len: %d--- FrameOffset: 0x%x---duration: 0x%x\n" , len, pFrm[i].FrameOffset, duration);
the items placed after FrameOffset gets wrong data.

- Output sends data to engine, then which sends to registered clients who are interested.

- SetEventCallbacks shouldn't be called on the primary client.

- 

## HEAP

 * 'Heap-' is based on 'Virtual-'(page granularity)
 * PEB.ProcessHeaps is a list of _HEAP pointers; 
 * _HEAP(HeapCreate) contains list(max=64) of segments(_HEAP_SEGMENT);
 * a heap segment contains list of block(`_HEAP_ENTRY`)(HeapAlloc/RtlAllocateHeap/GetProcessHeap !substract its size);
 * segments may not be contiguous and ascending;
 * HeapHandle/HeapStartAddr is address of _HEAP;
 * memory size unit(_HEAP_ENTRY size) is 8/16 bytes in 32/64 bit, to get the bytes from value applying left shift 3 bit;
 * large blocks(>0x7fff0) are not allocated from segment, instead directly from heap, the list or blocks is kept in _HEAP.VirtualAllocdBlocks;
 * all debug features can be set by gflags;
 * page heap;
 	.) divided into normal & full;
 	.) can be enabled by gflags only;
	.) able to backtrace alloc/free;
	.) full page heap appends a non-writable page at the end/start to trigger error when overrun/underrun;
	.) _DPH_HEAP_ROOT = HeapHandle + 0x1000
	.) entry(each contains allocation stack trace address):
		#) _DPH_HEAP_BLOCK: resides in dedicated segment;
		#) _DPH_BLOCK_INFORMATION: precedes data(_HEAP_ENTRY equivalent)

-!heap
 * -stat has latency;
 * specify address or index;
 	.) 0 for all
	.) index is for handle;
 * HeapHandle list [with segments](sort with details ascendingly) ;
 	.) [no switch]
 	.) -stat
 	.) -s
 	.) -m [handle]
 	
 * entry list:
 	.) -h handle;
 	.) -stat -h handle: 20 items;

 * locate the heap entry for address(allocating stack trace if enabled);
	.) normal heap: 
		!heap -x addr
	.) page and normal heap;
		!heap -p -a addr

 * entry info: -i
	.) call twice with handle first, then _HEAP_ENTRY address;
	.) 'Block CRC: OK' if handle set correctly;

 * search reference to any address in the range which the entry contains; 'addr' could be any address in the range;
	 !heap -x -v addr

 * page heap can be viewed only by '-p' switch(normal heap also included);

##	GENERAL

- history search: shift+up arrow;
- suspend_ui 1: prevent command window rendering;

- x64 debugger to debug x86 app may output x64 data, such as '!heap -stat'; 
 * better to debug with the bitness matched debugger;

- extension:
 * .load/.loadby
 * list: .chain
 * .unload: when name is omited, the last one is assumed to be;

- kernel debug on virtual machine:
 * guest: msconfig /debug+com1
 * virtual machine: serial port -> com1 + host pipe + \\.\pipe\NAME
 * windbg: kernel debug -> com -> port:\\.\pipe\NAME

- always call '.reload /user' when debug user mode process in kernel mode debugger, then
 * 'lm' can load user mode modules;

- ntdll contains stubs of kernel mode services;
 * the symbols of functions exist both at 'ntdll' and 'nt';
 * don't use that in ntdll, correct correspondence is not guuarranteed;

- step into 'sysenter' doesn't work, set bp on the same name in 'nt';

- breakpoint command;
 * should be enclosed in double quotes("");
 * bs[c]: update [conditional]  breakpoint command;
 * no commands should follow those controling target(t/p/g), or error occurs: 'function is incorrect';

- j expr cmd1; cmd2
 * ';' is necessary even if no 'cmd2';
 * enclose cmd in '', if more than one;
 * no commands is allowed following this, except place 'j' in block;
 * single quotes in string in commands are not allowed; take '.if' instead;

- commands that follow those controling target(t/p/g) are ignored in some circumstances, such as exception command;
 * g target, 'target' is ignored;

- in WOW64, processor mode in debugger determines the output size of certain command;
 * to switch mode(effective machine): .effmach ./x86/amd64
 * cmds affected: !heap, 
 * !peb fail if in x86;
 * there are two PEB/TEB for each; psuedo register $peb/$teb in windbg32/64 points to the corresponding one;



- dump memory layout:
	:!address

- address <-> symbol
x	- symbol to address
dt	- address to symbol 

- dt except: variable of type;
 * arguments order is not important;
 * address is to pointer of an instance;

- list local variables:
 * x
 * dv

- '!dlls' can get more details than 'lm' for module;

- user-defined pseudo-register starts from 0 to 19;
	$t0-19;
 * only store integer (with any type info);

- DATA breakpoint is hardware breakpoint; 'read' access implies 'write';

- CTRL+ALT+D
display the communication with remote debuggee.

- dot represents current address.

- count backward:	L-10
	ub .
- uf	-disassemble function.

- disassembly displays symbol info according to the codes that are before optimized. this is caused by symbol file can't reflect the optimized code layout.

- after a user mode process is switched to, a kernel mode system thread with high priority is inserted to that process(not belongs to it), to gain control when the process is scheduled to run. 

- switch process in kernel mode:
 * .process /i EPROCESS
 * g
 
- to break in to a user process before it runs in kernel mode debugging, for examining its symbols or unavailable:
	a) find out EPROCESS address in nt!PspCreateProcess
	b) switch context to it forcibly(.process /i EPROCESS);
	c)

- gflags:
 * tabs:
	 .) 'system registry': all processes/permanent/reboot needed;
	 .) 'kernel flags': all process/temporary/immdediatly;
	 .) 'image file': process specific/permanent/rerun;
 * 'soe'(stop on exception): make kernel debugger accept user mode exceptions as well;
 	.) catch kernel mode exceptions which can't be caught by user debugger;
 	.) general exceptions are passed to kernel debugger if no user mode debugger exists;
	.) exceptions controlling debugger always are passed, not affected by this flag;
	.) won't work to set in target on the fly; '!gflag' in host debugger works;
 * set a debugger for a process to attatch to it automatically when it's started;

- if stack frame is corrupted, 'k' can't go up any further, instead use 'dps';

- pfn(Page Frame Number): the state of each page in physical memory;

- vpn(Virtual Page Number): virtual address divides page size;


- !vad vad_node_address
- !vad va 1:	first get vad_node_address from va;

- _MMVAD_FLAGS.NoChange controls if 'Protect' can be modified(can be modified in kernel debugger);

- 'kf' displays frame lengthes;

- to break in on certain important times, like module loaded, using 'sx'; (only User-mode)

- when an exception is fixed, set it to be handled before go:
 * gh
 * sxe -h exception

- windbg can work in session 0 as a invisible server with '-server', debugging those process working in session 0, and outputing to visible clients.

-
step out:	gu(go up);
run to 'ret':	pt
run to 'call':	pc
run to 'ret' or 'call':	ptc
step in: t(trace)


 **gu works incorrectly if stack trace is incorrect or ebp is not used, call 'pt' instead;**

- to set bp in user-mode code when kernel debuging, it should be done after the context is switched to that process, by .process /i 0xaaaaa

- when process exits, the field 'SignalState' will be set in `_EPROCESS`.

- to redirect output to a file, use .logopen/.logclose 

- when function name includes special chars and spaces, enclose the whole string:
 * @!"..."
 * ()

- turn on symbol process information display:
	!sym noisy

- bu can't accept ambiguous symbol;
 * use source line to distinguish overloads;
 * bm

- resolve virtual address and load user mode symbols in kernel mode: 
  * .process /r /p
  * .reload /user

- cancel processing:
ctrl+break

- restart communicating with target and toggle printing connecting info:
 * ctrl+alt+d
 * ctrl+r	->	enter	-> ctrl+c



- symbol server url:
`SRV*http://msdl.microsoft.com/download/symbols;CACHE*dir`
 * `cache*path` accept the first one and ignores others;
 * cumulative between workspaces;

- commands contrast(only extension are case sensitive)(windbg!ProcessEngineCommands, dbgeng!Execute, dbgeng!ProcessCommands):
 * Regular commands: debug the target process.
 * Dot commands: control the debugger(windbg!DirectCommand).
 * Extension commands: both of them.


- get help info:
.hh					-open help file
? 					-list all built-in commands
.help 				-list meta commands
!help CommandName	-info of extension command
 * in sos.dll, '!help' list all help info;

-
? exp	- evaluate expression
?? exp	- evaluate c++ expression


-enable kernel mode debug:
1) bcdedit /debug on
2) reboot.
3) enter windbg directory, run:
    KDbgCtrl  -db
    KDbgCtrl  -e

- a leading underscore is present for kernel object symbol names.

- @@() treat the expression as another one against the default between c++ and MASM.
 * @@c++()/@@masm() can be embeded in a long command to force treat this subpart as an expression.
 * masm doesn't  have logical operators
	.) switch to c++(nested masm) to use them
		? @@(@@(poi(eax))||@@(poi(eax)))
	.) use bitwise operator(and/or better than &/|) and comparison operators which results 1 or 0;
 * c++ doesn't recognize hex number if no '0x';
 * '@' is mandatory for registers in c++: *(char *@eax; 

- in masm, dereference is achieved by 'poi()';

- in c++ expr, a symbol can't be resolved if its data type is not one of c++ primitive data types(switch to masm).

- exit scripts when break in:
.if(@$ip==ntdll!DbgBreakPoint){.break}

- dereference with c++ and embeded it in a normal command:
	dt nt!_object_type @@(*((nt!_object_type *)*@$t0))
		<==>
	?? *((nt!_object_type *)*@$t0)

- !alpc /lpc ADDR
list all processes connected to this ALPC port.

- to attach to a program with source accessible, infinitely loop on a global variable(work around the drawback that JIT launches debugger with the target privileges).

- ListEntry field points to another field of the same type of structure; to get the address of the structure, substracts the offset of this field from the address it points; $extret with !list command contains the field address instead of structure's.  

- object type list stored in: nt!obTypeIndexTable

- DEBUG_STACK_FRAME
    InstructionOffset	-- same to ReturnOffset;
    ReturnOffset;
    FrameOffset;		-- where the last ebp stored
    StackOffset;		-- the position below the return address of next call(the top of this stack frame).
So, stack is composed of this recursive group of "StackOffset+FrameOffset+ReturnOffset". 

- examine symbol in any modules, "*!" is necessary:
x *!*sleep*

- x
 * module name is necessary; if not clear, use wildcard.
 * no extension should appear in module name;
 * forwarded symbol can't be found;

- force loading deferred symbols:
 * .reload /f mfc120ud.dll
 * ld module.dll

- break on entry point:
bp $exentry

- control thread:
			user	kernel		
 list		~		!process id 2
 current 	~1		!thread	
 set		~2s		.thead id
 
- processor
 * change: ~1s
 * .echocpunum: switch to show/hide in command label;
 * !running

- when breaks in a user mode process, the current thread is a leased system thread, which will not be included in the target process by '!process -1 2'

- number radix:
 * default is hex;
 * the command to set is: n 16
 * hex: 0x
 * decimal: 0n
 * octal: 0t
 * binary: 0y
 
- dg @cs:	segment descriptor;

- gdtr/gdtl: base/limit;

- .echo accepts semi-colon in quotation marks;

- application verifier can detect memory leaks occured in dlls only;

- in km, system dlls are mapped to each process;

- fs:0 contains _TEB, which contains `_NT_TIB` as its first member;
 * address represented by 'fs:0' is stored at offset of 0x18 in the memory of that address;
 * commands (exept 'd') accepting an virtual address evaluate "fs:0" to 0; however poi(fs:0) can read correctly; then read the address by "poi(fs:18)" which is a member of `_NT_TIB` points to itself;

- LastErrorValue locates in _TEB:34;

- application verifier(appverif.exe) in system32 can control more details for an app than gflags.exe;

- break in when system boots: ctrl+alt+k (cycle initial break);

- 'sxe ld' works when system boots;
 * only work for modules loaded in kernel space when in kernel debugger, unless to set '!gflag +ksl';
 * alternative is 'BreakOnDllLoad' under:
 "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\Foo.dll"

- code breakpoints set by kernel debugger may be missed for race condition; adopt hardware breakpoint instead;
 * to use hardware breakpoint by default:
 	.allow_bp_ba_convert 1

- !process process flag imageName:
 * imageName should include extension;
 * if process is 0, imageName will be checked, if none, all are shown;
 * !process -1				-- current process(-1 can be omited);
 * !process 0				-- all active processes;

- dump stack of certain process:
 * !process EPROCESS 2
	 .) !thread ETHREAD
	 .) .thread ETHREAD; k

- 'break' can force connect to kernel mode debugee;

- bp /c|C	address
	only break when $csp is less/greater than the address;

- when entering ntdll!LdrpInitializeProcess, the main image and statically linked dlls are loaded but not initialized.

- find which function is currently in:
	ln

-set exception:
sxe	-	enable first chance break
sxd	-	disable first chance break(enable second chance)
sxn	-	only log exceptions
sxi	-	ignore this exception totally
 * commands: -c"" -c2"" 
 * those work for both user and kernel debuggers, only take effect in their specific space;
 * user space exceptions can be passed to kernel debugger when 'soe' in gflag is turned on;

- load extension module and execute a command:
	!logext.logm
 * after the module is loaded, the module name can be omited:
	!logm

 * usually an extension has command 'help', which will list all the commands;
 * .extmatch search ext commands;

- view image name:
 * |
 * !peb

- find symbol & data types
x mod!name*
dt mod!name*


- dt: to display a subfield content, append a dot to the field name;

- function offset maybe not correct in optimized code; instead to check RetAddr;

- automatic pseudo-registers take important role in script program, which store the most valuable memory addresses.
 * $exentry	- OEP
 * $ra			- current return address
 * $proc		- EPROCESS (equal to $peb in user mode)
 * $peb, $teb
 * some extensions define their own automatic pseudo-register such as 'extret' in '!list';

- c++ expression can recognize pseudo-registers' type;
 * assign type to pseudo-register: r?

- symbols in MASM are treated as adddress always; whereas in c++ treated as types;

- '?' can only evaluate the address of c++;
 ? @@(@$peb->ProcessParameters)
 ? @@(&@$peb->ProcessParameters->ImagePathName)		-- ImagePathName is an embeded member, instead of pointer, so '&' is mandatory;

- prepend exclamation point (!) to a symbol if it can be confused to be a hex number;

- symbol of local variable can be distinguished by a module name as '$';

- in 32bit app running in x64, ds/S and 'as /msa' take the 32-bit structure;
 * we can specify ntdll!_unicode_string explicitly to 'dt':
	dt ntdll!_unicode_string Buffer @@(&@$peb->ProcessParameters->ImagePathName)
 * evaluate expression to get the correct address, then pass to d/au:
 	? @@(@$peb->ProcessParameters->ImagePathName.Buffer)
 	du @@(@$peb->ProcessParameters->ImagePathName.Buffer)

- arguments are: $arg0 - $arg9
 * is aliases, which will be deleted by:
	ad *
 * $arg0 is command string;
  

- the execution of pending script is paralelle with the command logging; so the logging output may be after the output of script when breakin;

- in masm expression, 'poi()' is used to dereference;

- comment:
$$	- to the semicolon(even in quotes);
	* ';' is required between cmds and it;
*	- everything after it in this block;

- certain commands can't be ended by semicolon, which can only be separted by '.block{}';
 *as
 *r$.u0=abc

- Variable in .foreach
 * is not an alias, will not store value after iterating;
 * but if already defined in alias, error is thrown;

- one more cmd will be execute after 'g';
 the next loop will cause 'The debugee already running error':
	while(1){g}	
 add another dummy cmd after 'g': r$t0=$t0;

- escape
 * \ 
	bp USER32!CreateWindowExW "ad /q CLS; aS /mu CLS poi(esp+8); .if($spat(\"${CLS}\",\"Chrome_WidgetWin_?\")) {} .else {gc}"
 * @""
 * @!""	-symbol
 * script file

- $spat:
 * quots are neccessary
	as PATH abcd;
	$spat("${PATH}", "*b*");
 * escape with @"" in case of special chars to trigger error;

- if string following .echo starts with ", it should ends with " as well, i.e. no char should appear between the ending quote and semicolon;

- undocumented commands:
!showimports module
!showexports module

- force to load symbols:
 * ld modumeName /f symbol_filename
 * .reload /f m

- force to load a mismatched symbol file:
 

- load module again after unload manually by '.reload /u';
	.reload m=address

- g:
 * startAddress should be less than end address;
 * go to address to begin execute: g =addr
 * execute to stop at address: g addr

- in x64, poi(fs:0) and poi(gs:0) both can't be accessed; 
 * get teb address from @$teb pseudo register;

- .formats num
 -- output number in different formats;

- .dvalloc <=> .call malloc

- '.call' only works when private symbol pdb exists;

- search memory:
 * any address: s
 * in address region: !address -f:MEM_COMMIT,heap... -c:"s %1..."
 * binary in heap(only work on entry with size not greater than 0x10000): 
	 !heap -srch pattern
 * !search: physical memory;
 * !findstack: symbol in stack;

- s:
 * enclose flags in [];	s -[s]
 * precedes flags and type together with '-';	s-[s]a

- create stack trace history:
	!gflag +ust

- only execute current thread: ~. g;

- memory with usage of 'unknown'(!address /f:var) is allocated via 'VirtualAlloc':
 * direct VirtualAlloc() calls in your code
 * .NET (because it has its own heap manager)
 * C++ HeapAlloc() calls that are larger than some limit
 * MSXML
 * GDI Bitmaps 

- symbol file may supply different name for export/import, then which are invisible;

- if a function jump to another one, instead of 'call', it's lost from stack trace;

- recursive calling function or group of functions;
 * skip new: set conditional bp stopping only at larger or equal stack, or with MaxCallStackDepth(lines appear in 'k');
 * go out from deep nested inside: check how many times one specific  return address left, set bp with this pass time;
 * achieve both above: bp at return address, count times when esp larger than specific one, break when times meet left;
 	r@$t2=0; bp reta "j esp<stackAddr gc; 'r@$t2=@$t2+1; j @$t2<5 gc'"

- to accelerate the translation of addresses between windbg and IDA, store the base address difference in pseudo-register at module load event;
	sxe ld -c"r @$t1=module-base" module

- control ui outputs:
 * non-command windows: .suspend_ui
 * command window: .outmask 
  	.) turn off all output except log file:
		.outmask- /l ffff
	.) restore
		.outmask /d

- flags:
of(Overflow)	nv/ov
df(Direction)	up/dn
if(Interrupt)	di/ei
sf(Sign)		pl/ng
zf(Zero)		nz/zr
af(Auxiliary)	na/ac
pf(Parity)		pe/po
cf(Carry)		nc/cy

- data access breakpoint may cause 'SetContext failed, 0x80070005': E_ACCESSDENIED;
 * disable after used;
 * specify thread;

- when executible resources are loaded by LoadResource/ReadFile/..., bp on virtual address is not hit; ???????????? read in kernel mode? 
 * search the memory from 'LockResource'/'SizeofResource'

- processor breakpoints set in kernel debugger are stored in PRCB(processor control block), which will be duplicated to all threads in all processes;
 * specify /p flag(EPROCESS)
 * user mode debugger can catch it, but not vice versa;

- rM20 won't work in user debugger(debug registers can be accessed only in kernel mode);

- kernel & user mode debugger side by side, catch processor breakpoints absolutely; 
 * set processor breakpoints in kernel debugger;
 * both can catch those triggered in their specific space; 
 * it's shown as 'single step exception' in user debugger;
 * when catched in kernel, the API called is after 'ntdll!KiFastSystemCallRet';

- invalid address cause access violation, so processor breakpoint will never catch it;

- processor breakpoints are very limited to width(4 bytes at most); take page fault to mimic in unit of page; 
 * MEMORY_BASIC_INFORMATION.Protect: PAGE_NOACCESS(0x01); PAGE_READONLY(0x02); PAGE_READWRITE(0x04); PAGE_GUARD(0x100)
 * !vprot/!vadump: display MEMORY_BASIC_INFORMATION;
 * PAGE_GUARD: one shot

- !vprot works in u, !vm in k;

- !error int:	show error number info;
 * !gle

- user mode extension 'uext.dll' is added automatically when in user mode debugging, whereas not when kernel mode debugging;

- fail to get vad is a matter of windbg version;
 * '!vad va 1' failed also;
