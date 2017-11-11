#pragma pack(push, 1)           // IDA uses 1 byte alignments!
#if defined CL
	#include <stdio.h>
#endif
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef          char   int8;
typedef signed   char   sint8;
typedef unsigned char   uint8;
typedef          short  int16;
typedef unsigned short  uint16;
typedef int	int32;
typedef unsigned int	uint32;
typedef long long int64;
typedef unsigned long long uint64;

#define OBSOLETE(n)	n
#define idaapi 
#ifdef X64
	typedef uint64 ea_t;      // effective address
	typedef uint64 sel_t;     // segment selector
	typedef uint64 asize_t;   // memory chunk size
	typedef int64 adiff_t;    // address difference
#else
	typedef uint32 ea_t;      // effective address
	typedef uint32 sel_t;     // segment selector
	typedef uint32 asize_t;   // memory chunk size
	typedef int32 adiff_t;    // address difference
#endif
typedef asize_t uval_t;   // unsigned value used by the processor
typedef adiff_t sval_t;   // signed value used by the processor
typedef ptrdiff_t ssize_t;
typedef uint32 flags_t;   // 32-bit flags for each address
	
//dummy data
#define DUMMY int
typedef DUMMY AbstractRegister; 
typedef DUMMY rginfo;
typedef DUMMY WorkReg; 
typedef DUMMY bytes_t;
typedef DUMMY instruc_t;
typedef DUMMY switch_info_ex_t;
typedef DUMMY func_t;
typedef DUMMY member_t;
typedef DUMMY set_options_t;
typedef DUMMY mvm_t;
typedef DUMMY help_t;
//dummy data


class op_t
{
public:
// Number of operand. Initialized once at the start of work.
// You have no right to change its value.

  uchar         n;              // number of operand (0,1,2)


// Type of operand. See above for explanations

enum optype_t: uchar     // Description                          Data field
{
  o_void     =  0, // No Operand                           ----------
  o_reg      =  1, // General Register (al,ax,es,ds...)    reg
  o_mem      =  2, // Direct Memory Reference  (DATA)      addr
  o_phrase   =  3, // Memory Ref [Base Reg + Index Reg]    phrase
  o_displ    =  4, // Memory Reg [Base Reg + Index Reg + Displacement] phrase+addr
  o_imm      =  5, // Immediate Value                      value
  o_far      =  6, // Immediate Far Address  (CODE)        addr
  o_near     =  7, // Immediate Near Address (CODE)        addr
  o_idpspec0 =  8, // IDP specific type
  o_idpspec1 =  9, // IDP specific type
  o_idpspec2 = 10, // IDP specific type
  o_idpspec3 = 11, // IDP specific type
  o_idpspec4 = 12, // IDP specific type
  o_idpspec5 = 13, // IDP specific type
  o_last     = 14 // first unused type
};
  optype_t      type;           // type of operand


// Offset of operand value from the instruction start.
// Of course this field is meaningful only for certain types of operands.
// Leave it equal to zero if the operand has no offset.
// This offset should point to the 'interesting' part of operand.
// For example, it may point to the address of a function in
//      call func
// or it may point to bytes holding '5' in
//      mov  ax, [bx+5]
// Usually bytes pointed to this offset are relocated (have fixup information)

  char          offb;           // offset of operand relative to instruction start
                                // 0 - unknown


// The same as above. Some operands have 2 numeric values used to
// form operand. 'offo' is used for the second part of operand if it exists.
// Currently this field is used only for outer offsets of Motorla processors.
// Leave it equal to zero if the operand has no offset

  char          offo;           // offset of operand relative to instruction start
                                // 0 - unknown


// Some characteristics of operand

enum op_flags: uchar
{
OF_NO_BASE_DISP= 0x80,    // o_displ: base displacement doesn't exist
                                // meaningful only for o_displ type
                                // if set, base displacement (x.addr)
                                // doesn't exist.
OF_OUTER_DISP=   0x40,    // o_displ: outer displacement exists
                                // meaningful only for o_displ type
                                // if set, outer displacement (x.value) exists.
PACK_FORM_DEF=   0x20,    // !o_reg + dt_packreal: packed factor defined
OF_NUMBER=       0x10,    // can be output as number only
                                // if set, the operand can be converted to a
                                // number only
OF_SHOW=         0x08    // should the operand be displayed?
                                // if clear, the operand is hidden and should
                                // not be displayed
};
  op_flags         flags;



// Convenience functions:

//  void set_showed()     { flags |=  OF_SHOW; }
//  void clr_showed()     { flags &= ~OF_SHOW; }
//  bool showed() const   { return (flags & OF_SHOW) != 0; }


// Type of operand value. Usually first 9 types are used.
// This is the type of the operand itself, not the size of the addressing mode.
// for example, byte ptr [epb+32_bit_offset]  will have dt_byte type.

  char          dtyp;
enum op_dtyp
{
	dt_byte        =0 ,     // 8 bit
	dt_word        =1 ,     // 16 bit
	dt_dword       =2 ,     // 32 bit
	dt_float       =3 ,     // 4 byte
	dt_double      =4 ,     // 8 byte
	dt_tbyte       =5 ,     // variable size (ph.tbyte_size)
	dt_packreal    =6 ,     // packed real format for mc68040
	dt_qword       =7 ,     // 64 bit
	dt_byte16      =8 ,     // 128 bit
	dt_code        =9 ,     // ptr to code (not used?)
	dt_void        =10,     // none
	dt_fword       =11,     // 48 bit
	dt_bitfild     =12,     // bit field (mc680x0)
	dt_string      =13,     // pointer to asciiz string
	dt_unicode     =14,     // pointer to unicode string
	dt_3byte       =15,     // 3-byte data
	dt_ldbl        =16,     // long double (which may be different from tbyte)
	dt_byte32      =17,     // 256 bit
	dt_byte64      =18      // 512 bit
};


// The following unions keep other information about the operand

  union reg_t{
    uint16 reg;                 // number of register (o_reg)
    uint16 phrase;              // number of register phrase (o_phrase,o_displ)
                                // you yourself define numbers of phrases
                                // as you like
  } reg;

//  bool is_reg(int r) const { return type == o_reg && reg == r; }

//  Next 12 bytes are used by mc68k for some float types


// VALUE

  union  value_t
  {
    uval_t value;               // 1) operand value (o_imm)
                                // 2) outer displacement (o_displ+OF_OUTER_DISP)
                                // integer values should be in IDA's (little-endian) order
                                // when using ieee_realcvt, floating point values should be in the processor's native byte order
                                // dt_double values take up 8 bytes (value and addr fields for 32-bit modules)
                                // NB: in case a dt_dword/dt_qword immediate is forced to float by user,
                                // the kernel converts it to processor's native order before calling FP conversion routines

    struct value_shorts{                    // this structure is defined for
        uint16 low;             // your convenience only
        uint16 high;
    };
  } value;

//  bool is_imm(uval_t v) const { return type == o_imm && value == v; }


// VIRTUAL ADDRESS (OFFSET WITHIN THE SEGMENT)

  union addr_t
  {
    ea_t addr;                  // virtual address pointed or used by the operand
                                // (o_mem,o_displ,o_far,o_near)

    struct addr_shorts{                    // this structure is defined for
        uint16 low;             // your convenience only
        uint16 high;
    };
  } addr;


// IDP SPECIFIC INFORMATION

  union specval_t
  {
    ea_t specval;               // This field may be used as you want.
    struct specval_shorts{                    // this structure is defined for your convenience only
        uint16 low;             // IBM PC: segment register number (o_mem,o_far,o_near)
        uint16 high;            // IBM PC: segment selector value  (o_mem,o_far,o_near)
    };
  } specval;

// The following fields are used only in idp modules
// You may use them as you want to store additional information about
// the operand

  char          specflag1;
  char          specflag2;
  char          specflag3;
  char          specflag4;

};
op_t op;

class insn_t
{
// Current segment base paragraph. Initialized by the kernel.

  ea_t cs;                      // segment base (in paragraphs)


// Virtual address of the instruction (address within the segment)
// Initialized by the kernel.

  ea_t ip;                      // offset in the segment


// Linear address of the instruction.
// Initialized by the kernel.

  ea_t ea;                      // instruction start addresses


// Internal code of instruction. IDP should define its own instruction
// codes. These codes are usually defined in ins.hpp. The array of instruction
// names and features (ins.cpp) is accessed using this code.

  uint16 itype;                 // instruction code (see ins.hpp)
                                // only for canonical insns (not user defined!):
// Size of instruction in bytes.
// The analyzer should put here the actual size of the instruction.

  uint16 size;                  // instruction size in bytes


// Additinal information about the instruction.
// You may use these field as you want.

 union auxpref_t
  {
    uint16 auxpref;             // processor dependent field
    struct auxpref_chars
    {
      uchar low;
      uchar high;
    };
  } auxpref;
  char segpref;                 // processor dependent field
  char insnpref;                // processor dependent field

// Information about instruction operands.

//#define UA_MAXOP        6
//  op_t Operands[UA_MAXOP];

  op_t Op1;
  op_t Op2;
  op_t Op3;
  op_t Op4;
  op_t Op5;
  op_t Op6;

enum insn_flag: char
{
	INSN_MACRO =0x01,        // macro instruction
	INSN_MODMAC=0x02        // macros: may modify the database
};
  insn_flag flags;                   // instruction flags
                                // to make room for the macro insn
};
insn_t insn;


class asm_t
{
	enum asm_flag: uint32
{
AS_OFFST      =0x00000001L,       // offsets are 'offset xxx' ?
AS_COLON      =0x00000002L,       // create colons after data names ?
AS_UDATA      =0x00000004L,       // can use '?' in data directives

AS_2CHRE      =0x00000008L,       // double char constants are: "xy
AS_NCHRE      =0x00000010L,       // char constants are: 'x
AS_N2CHR      =0x00000020L,       // can't have 2 byte char consts

                                  // ASCII directives:
AS_1TEXT      =0x00000040L,       //   1 text per line, no bytes
AS_NHIAS      =0x00000080L,       //   no characters with high bit
AS_NCMAS      =0x00000100L,       //   no commas in ascii directives

AS_HEXFM      =0x00000E00L,       // format of hex numbers:
ASH_HEXF0     =0x00000000L,       //   34h
ASH_HEXF1     =0x00000200L,       //   h'34
ASH_HEXF2     =0x00000400L,       //   34
ASH_HEXF3     =0x00000600L,       //   0x34
ASH_HEXF4     =0x00000800L,       //   $34
ASH_HEXF5     =0x00000A00L,       //   <^R   > (radix)
AS_DECFM      =0x00003000L,       // format of dec numbers:
ASD_DECF0     =0x00000000L,       //   34
ASD_DECF1     =0x00001000L,       //   #34
ASD_DECF2     =0x00002000L,       //   34.
ASD_DECF3     =0x00003000L,       //   .34
AS_OCTFM      =0x0001C000L,       // format of octal numbers:
ASO_OCTF0     =0x00000000L,       //   123o
ASO_OCTF1     =0x00004000L,       //   0123
ASO_OCTF2     =0x00008000L,       //   123
ASO_OCTF3     =0x0000C000L,       //   @123
ASO_OCTF4     =0x00010000L,       //   o'123
ASO_OCTF5     =0x00014000L,       //   123q
ASO_OCTF6     =0x00018000L,       //   ~123
ASO_OCTF7     =0x0001C000L,       //   q'123
AS_BINFM      =0x000E0000L,       // format of binary numbers:
ASB_BINF0     =0x00000000L,       //   010101b
ASB_BINF1     =0x00020000L,       //   ^B010101
ASB_BINF2     =0x00040000L,       //   %010101
ASB_BINF3     =0x00060000L,       //   0b1010101
ASB_BINF4     =0x00080000L,       //   b'1010101
ASB_BINF5     =0x000A0000L,       //   b'1010101'

AS_UNEQU      =0x00100000L,       // replace undefined data items
                                  // with EQU (for ANTA's A80)
AS_ONEDUP     =0x00200000L,       // One array definition per line
AS_NOXRF      =0x00400000L,       // Disable xrefs during the output file generation
AS_XTRNTYPE   =0x00800000L,       // Assembler understands type of extrn
                                  // symbols as ":type" suffix
AS_RELSUP     =0x01000000L,       // Checkarg: 'and','or','xor' operations
                                  // with addresses are possible
AS_LALIGN     =0x02000000L,       // Labels at "align" keyword
                                  // are supported.
AS_NOCODECLN  =0x04000000L,       // don't create colons after code names
AS_NOTAB      =0x08000000L,       // Disable tabulation symbols during the output file generation
AS_NOSPACE    =0x10000000L,       // No spaces in expressions
AS_ALIGN2     =0x20000000L,       // .align directive expects an exponent rather than a power of 2
                                  // (.align 5 means to align at 32byte boundary)
AS_ASCIIC     =0x40000000L,       // ascii directive accepts C-like
                                  // escape sequences (\n,\x01 and similar)
AS_ASCIIZ     =0x80000000L        // ascii directive inserts implicit
                                        // zero byte at the end
};
  asm_flag flag;                           // Assembler features:

  uint16 uflag;                         // user defined flags (local only for IDP)
                                        // you may define and use your own bits
  const char *name;                     // Assembler name (displayed in menus)
  help_t help;                          // Help screen number, 0 - no help
  const char *const *header;            // array of automatically generated header lines
                                        // they appear at the start of disassembled text
  const uint16 *badworks;               // array of unsupported instructions
                                        // (array of cmd.itype, zero terminated)
  const char *origin;                   // org directive
  const char *end;                      // end directive
  const char *cmnt;                     // comment string (see also cmnt2)
  char ascsep;                          // ASCII string delimiter
  char accsep;                          // ASCII char constant delimiter
  const char *esccodes;                 // ASCII special chars
                                        // (they can't appear in character and
                                        // ascii constants)
//
//      Data representation (db,dw,...):
//
  const char *a_ascii;                  // ASCII string directive
  const char *a_byte;                   // byte directive
  const char *a_word;                   // word directive
  const char *a_dword;                  // NULL if not allowed
  const char *a_qword;                  // NULL if not allowed
  const char *a_oword;                  // NULL if not allowed
  const char *a_float;                  // float;  4bytes; NULL if not allowed
  const char *a_double;                 // double; 8bytes; NULL if not allowed
  const char *a_tbyte;                  // long double;    NULL if not allowed
  const char *a_packreal;               // packed decimal real NULL if not allowed
  const char *a_dups;                   // array keyword. the following
                                        // sequences may appear:
                                        //      #h - header
                                        //      #d - size
                                        //      #v - value
                                        //      #s(b,w,l,q,f,d,o) - size specifiers
                                        //                        for byte,word,
                                        //                            dword,qword,
                                        //                            float,double,oword
  const char *a_bss;                    // uninitialized data directive
                                        // should include '%s' for the
                                        // size of data
  const char *a_equ;                    // 'equ' Used if AS_UNEQU is set
  const char *a_seg;                    // 'seg ' prefix (example: push seg seg001)

//
//  Pointer to checkarg_dispatch() function. If NULL, checkarg won't be called.
//
  bool (idaapi* checkarg_dispatch)(void *a1, void *a2, uchar cmd);
  void *_UNUSED1_was_atomprefix;
  void *_UNUSED2_was_checkarg_operations;

//
// translation to use in character and string constants.
// usually 1:1, i.e. trivial translation (may specify NULL)
//
  const uchar *XlatAsciiOutput;         // If specified, must be 256 chars long
  const char *a_curip;                  // current IP (instruction pointer)
                                        // symbol in assembler
  void (idaapi *func_header)(func_t *); // generate function header lines
                                        // if NULL, then function headers
                                        // are displayed as normal lines
  void (idaapi *func_footer)(func_t *); // generate function footer lines
                                        // if NULL, then a comment line
                                        // is displayed
  const char *a_public;                 // "public" name keyword. NULL-gen default, ""-do not generate
  const char *a_weak;                   // "weak"   name keyword. NULL-gen default, ""-do not generate
  const char *a_extrn;                  // "extrn"  name keyword
  const char *a_comdef;                 // "comm" (communal variable)
//
// Get name of type of item at ea or id.
// (i.e. one of: byte,word,dword,near,far,etc...)
//
  ssize_t (idaapi *get_type_name)(flags_t flag,
                                  ea_t ea_or_id,
                                  char *buf,
                                  size_t bufsize);

  const char *a_align;                  // "align" keyword

// Left and right braces used in complex expressions

  char lbrace;
  char rbrace;

  const char *a_mod;    // %  mod     assembler time operation
  const char *a_band;   // &  bit and assembler time operation
  const char *a_bor;    // |  bit or  assembler time operation
  const char *a_xor;    // ^  bit xor assembler time operation
  const char *a_bnot;   // ~  bit not assembler time operation
  const char *a_shl;    // << shift left assembler time operation
  const char *a_shr;    // >> shift right assembler time operation
  const char *a_sizeof_fmt; // size of type (format string)

enum asm_flag2: uint32
{
AS2_BRACE     =0x00000001,        // Use braces for all expressions
AS2_STRINV    =0x00000002,        // For processors with bytes bigger than 8 bits:
                                  //  invert the meaning of inf.wide_high_byte_first
                                  //  for text strings
AS2_BYTE1CHAR =0x00000004,        // One symbol per processor byte
                                  // Meaningful only for wide byte processors
AS2_IDEALDSCR =0x00000008,        // Description of struc/union is in
                                  // the 'reverse' form (keyword before name)
                                  // the same as in borland tasm ideal
AS2_TERSESTR  =0x00000010,        // 'terse' structure initialization form
                                  // NAME<fld,fld,...> is supported
AS2_COLONSUF  =0x00000020,        // addresses may have ":xx" suffix
                                 // this suffix must be ignored when extracting
                                 // the address under the cursor
AS2_YWORD     =0x00000040        // a_yword field is present and valid
};
  asm_flag2 flag2;

  const char *cmnt2;                    // comment close string (usually NULL)
                                        // this is used to denote a string which
                                        // closes comments, for example, if the
                                        // comments are represented with (* ... *)
                                        // then cmnt = "(*" and cmnt2 = "*)"
  const char *low8;     // low8 operation, should contain %s for the operand
  const char *high8;    // high8
  const char *low16;    // low16
  const char *high16;   // high16
  const char *a_include_fmt;            // the include directive (format string)
  const char *a_vstruc_fmt;             // if a named item is a structure and displayed
                                        // in the verbose (multiline) form then display the name
                                        // as printf(a_strucname_fmt, typename)
                                        // (for asms with type checking, e.g. tasm ideal)
  const char *a_3byte;                  // 3-byte data
  const char *a_rva;                    // 'rva' keyword for image based offsets
                                        // (see nalt.hpp, REFINFO_RVA)
  const char *a_yword;                  // 32-byte (256-bit) data; NULL if not allowed
};
asm_t assembler;


struct processor_t
{
  int version;                  // Expected kernel version,
                                //   should be IDP_INTERFACE_VERSION

enum id_t
{
PLFM_386        =0 ,      // Intel 80x86
PLFM_Z80        =1 ,      // 8085, Z80
PLFM_I860       =2 ,      // Intel 860
PLFM_8051       =3 ,      // 8051
PLFM_TMS        =4 ,      // Texas Instruments TMS320C5x
PLFM_6502       =5 ,      // 6502
PLFM_PDP        =6 ,      // PDP11
PLFM_68K        =7 ,      // Motoroal 680x0
PLFM_JAVA       =8 ,      // Java
PLFM_6800       =9 ,      // Motorola 68xx
PLFM_ST7        =10,      // SGS-Thomson ST7
PLFM_MC6812     =11,      // Motorola 68HC12
PLFM_MIPS       =12,      // MIPS
PLFM_ARM        =13,      // Advanced RISC Machines
PLFM_TMSC6      =14,      // Texas Instruments TMS320C6x
PLFM_PPC        =15,      // PowerPC
PLFM_80196      =16,      // Intel 80196
PLFM_Z8         =17,      // Z8
PLFM_SH         =18,      // Renesas (formerly Hitachi) SuperH
PLFM_NET        =19,      // Microsoft Visual Studio.Net
PLFM_AVR        =20,      // Atmel 8-bit RISC processor(s)
PLFM_H8         =21,      // Hitachi H8/300, H8/2000
PLFM_PIC        =22,      // Microchip's PIC
PLFM_SPARC      =23,      // SPARC
PLFM_ALPHA      =24,      // DEC Alpha
PLFM_HPPA       =25,      // Hewlett-Packard PA-RISC
PLFM_H8500      =26,      // Hitachi H8/500
PLFM_TRICORE    =27,      // Tasking Tricore
PLFM_DSP56K     =28,      // Motorola DSP5600x
PLFM_C166       =29,      // Siemens C166 family
PLFM_ST20       =30,      // SGS-Thomson ST20
PLFM_IA64       =31,      // Intel Itanium IA64
PLFM_I960       =32,      // Intel 960
PLFM_F2MC       =33,      // Fujistu F2MC-16
PLFM_TMS320C54  =34,      // Texas Instruments TMS320C54xx
PLFM_TMS320C55  =35,      // Texas Instruments TMS320C55xx
PLFM_TRIMEDIA   =36,      // Trimedia
PLFM_M32R       =37,      // Mitsubishi 32bit RISC
PLFM_NEC_78K0   =38,      // NEC 78K0
PLFM_NEC_78K0S  =39,      // NEC 78K0S
PLFM_M740       =40,      // Mitsubishi 8bit
PLFM_M7700      =41,      // Mitsubishi 16bit
PLFM_ST9        =42,      // ST9+
PLFM_FR         =43,      // Fujitsu FR Family
PLFM_MC6816     =44,      // Motorola 68HC16
PLFM_M7900      =45,      // Mitsubishi 7900
PLFM_TMS320C3   =46,      // Texas Instruments TMS320C3
PLFM_KR1878     =47,      // Angstrem KR1878
PLFM_AD218X     =48,      // Analog Devices ADSP 218X
PLFM_OAKDSP     =49,      // Atmel OAK DSP
PLFM_TLCS900    =50,      // Toshiba TLCS-900
PLFM_C39        =51,      // Rockwell C39
PLFM_CR16       =52,      // NSC CR16
PLFM_MN102L00   =53,      // Panasonic MN10200
PLFM_TMS320C1X  =54,      // Texas Instruments TMS320C1x
PLFM_NEC_V850X  =55,      // NEC V850 and V850ES/E1/E2
PLFM_SCR_ADPT   =56,      // Processor module adapter for processor modules written in scripting languages
PLFM_EBC        =57,      // EFI Bytecode
PLFM_MSP430     =58,      // Texas Instruments MSP430
PLFM_SPU        =59,      // Cell Broadband Engine Synergistic Processor Unit
PLFM_DALVIK     =60,      // Android Dalvik Virtual Machine
PLFM_65C816     =61,      // 65802/65816
PLFM_M16C       =62,      // Renesas M16C
PLFM_ARC        =63,      // Argonaut RISC Core
PLFM_UNSP       =64,      // SunPlus unSP
PLFM_TMS320C28  =65      // Texas Instruments TMS320C28x
};


  id_t id;                       // IDP id
                                // Numbers above 0x8000 are reserved
                                // for the third-party modules

  uint32 flag;                  // Processor features
#define PR_SEGS       0x000001  // has segment registers?
#define PR_USE32      0x000002  // supports 32-bit addressing?
#define PR_DEFSEG32   0x000004  // segments are 32-bit by default
#define PR_RNAMESOK   0x000008  // allow to user register names for
                                // location names
//#define PR_DB2CSEG    0x0010  // .byte directive in code segments
//                              // should define even number of bytes
//                              // (used by AVR processor)
#define PR_ADJSEGS    0x000020  // IDA may adjust segments moving
                                // their starting/ending addresses.
#define PR_DEFNUM     0x0000C0  // default number representation:
#define PRN_HEX       0x000000  //      hex
#define PRN_OCT       0x000040  //      octal
#define PRN_DEC       0x000080  //      decimal
#define PRN_BIN       0x0000C0  //      binary
#define PR_WORD_INS   0x000100  // instruction codes are grouped
                                // 2bytes in binrary line prefix
#define PR_NOCHANGE   0x000200  // The user can't change segments
                                // and code/data attributes
                                // (display only)
#define PR_ASSEMBLE   0x000400  // Module has a built-in assembler
                                // and understands IDP_ASSEMBLE
#define PR_ALIGN      0x000800  // All data items should be aligned
                                // properly
#define PR_TYPEINFO   0x001000  // the processor module supports
                                // type information callbacks
                                // ALL OF THEM SHOULD BE IMPLEMENTED!
#define PR_USE64      0x002000  // supports 64-bit addressing?
#define PR_SGROTHER   0x004000  // the segment registers don't contain
                                // the segment selectors, something else
#define PR_STACK_UP   0x008000  // the stack grows up
#define PR_BINMEM     0x010000  // the processor module provides correct
                                // segmentation for binary files
                                // (i.e. it creates additional segments)
                                // The kernel will not ask the user
                                // to specify the RAM/ROM sizes
#define PR_SEGTRANS   0x020000  // the processor module supports
                                // the segment translation feature
                                // (it means it calculates the code
                                // addresses using the codeSeg() function)
#define PR_CHK_XREF   0x040000  // don't allow near xrefs between segments
                                // with different bases
#define PR_NO_SEGMOVE 0x080000  // the processor module doesn't support move_segm()
                                // (i.e. the user can't move segments)
#define PR_FULL_HIFXP 0x100000  // REF_VHIGH operand value contains full operand
                                // not only the high bits. Meaningful if ph.high_fixup_bits
#define PR_USE_ARG_TYPES 0x200000 // use ph.use_arg_types callback
#define PR_SCALE_STKVARS 0x400000 // use ph.get_stkvar_scale callback
#define PR_DELAYED    0x800000  // has delayed jumps and calls
                                // if this flag is set, ph.is_basic_block_end should be implemented
#define PR_ALIGN_INSN 0x1000000 // allow ida to create alignment instructions
                                // arbirtrarily. Since these instructions
                                // might lead to other wrong instructions
                                // and spoil the listing, IDA does not create
                                // them by default anymore
#define PR_PURGING    0x2000000 // there are calling conventions which may
                                // purge bytes from the stack
#define PR_CNDINSNS   0x4000000 // has conditional instructions
#define PR_USE_TBYTE  0x8000000 // BTMT_SPECFLT means _TBYTE type
#define PR_DEFSEG64  0x10000000 // segments are 64-bit by default
#define PR_TINFO     0x20000000 // has support for tinfo_t
  int cnbits;                           // Number of bits in a byte
                                        // for code segments (usually 8)
                                        // IDA supports values up to 32 bits
  int dnbits;                           // Number of bits in a byte
                                        // for non-code segments (usually 8)
                                        // IDA supports values up to 32 bits
//
// Number of 8bit bytes required to hold one byte of the target processor
//

                                        // IDP module may support several compatible
                                        // processors. The following arrays define
                                        // processor names:
  const char *const *psnames;           // short processor names (NULL terminated)
                                        // Each name should be shorter than 9 characters
  const char *const *plnames;           // long processor names (NULL terminated)
                                        // No restriction on name lengths.
  const asm_t *const *assemblers;       // pointer to array of target
                                        // assembler definitions. You may
                                        // change this array when current
                                        // processor is changed.
                                        // (NULL terminated)

// Custom instruction codes defined by processor extension plugins
// must be greater than or equal to this:

#define CUSTOM_CMD_ITYPE 0x8000


// use_regarg_type (see below) uses this bit in the return value
// to indicate that the register value has been spoiled

#define REG_SPOIL 0x80000000L

//
// Callback function. IDP module can take appropriate
// actions when some events occurs in the kernel.
//
  enum idp_notify
  {
        init,                   // The IDP module is just loaded
                                // arg - const char *idp_modname
                                //       processor module name
                                // Processor modules should return zero to indicate failure
        term,                   // The IDP module is being unloaded
        newprc,                 // Before changing proccesor type
                                // arg - int processor number in the
                                //       array of processor names
                                // return 1-ok,0-prohibit
        newasm,                 // Before setting a new assembler
                                // arg = int asmnum
        newfile,                // A new file is loaded (already)
                                // arg - char * input file name
        oldfile,                // An old file is loaded (already)
                                // arg - char * input file name
        newbinary,              // Before loading a binary file
                                // args:
                                //  char *filename - binary file name
                                //  uint32 fileoff  - offset in the file
                                //  ea_t basepara  - base loading paragraph
                                //  ea_t binoff    - loader offset
                                //  uint32 nbytes   - number of bytes to load
        endbinary,              // After loading a binary file
                                //  bool ok        - file loaded successfully?
        newseg,                 // A new segment is about to be created
                                // arg = segment_t *
                                // return 1-ok, 0-segment should not be created
        assemble,               // Assemble an instruction
                                // (display a warning if an error is found)
                                // args:
                                //  ea_t ea -  linear address of instruction
                                //  ea_t cs -  cs of instruction
                                //  ea_t ip -  ip of instruction
                                //  bool use32 - is 32bit segment?
                                //  const char *line - line to assemble
                                //  uchar *bin - pointer to output opcode buffer
                                // returns size of the instruction in bytes
        obsolete_makemicro,     // Generate microcode for the instruction
                                // in 'cmd' structure.
                                // arg - mblock_t *
                                // returns MICRO_... error codes
        outlabel,               // The kernel is going to generate an instruction
                                // label line or a function header
                                // args:
                                //   ea_t ea -
                                //   const char *colored_name -
                                // If returns value <=0, then the kernel should
                                // not generate the label
        rename,                 // The kernel is going to rename a byte
                                // args:
                                //   ea_t ea
                                //   const char *new_name
                                //   int flags (SN_...)
                                // If returns value <=0, then the kernel should
                                // not rename it. See also the 'renamed' event
        may_show_sreg,          // The kernel wants to display the segment registers
                                // in the messages window.
                                // arg - ea_t current_ea
                                // if this function returns 0
                                // then the kernel will not show
                                // the segment registers.
                                // (assuming that the module have done it)
        closebase,              // The database will be closed now
        load_idasgn,            // FLIRT signature have been loaded
                                // for normal processing (not for
                                // recognition of startup sequences)
                                // arg - const char *short_sig_name
                                // returns: nothing
        coagulate,              // Try to define some unexplored bytes
                                // This notification will be called if the
                                // kernel tried all possibilities and could
                                // not find anything more useful than to
                                // convert to array of bytes.
                                // The module can help the kernel and convert
                                // the bytes into something more useful.
                                // arg:
                                //      ea_t start_ea
                                // returns: number of converted bytes + 1
        auto_empty,             // Info: all analysis queues are empty
                                // args: none
                                // returns: none
                                // This callback is called once when the
                                // initial analysis is finished. If the queue is
                                // not empty upon the return from this callback,
                                // it will be called later again.
                                // See also auto_empty_finally.
        auto_queue_empty,       // One analysis queue is empty
                                // args: atype_t type
                                // returns: 1-yes, keep the queue empty
                                //        <=0-no, the queue is not empty anymore
                                // This callback can be called many times, so
                                // only the autoMark() functions can be used from it
                                // (other functions may work but it is not tested)
        func_bounds,            // find_func_bounds() finished its work
                                // The module may fine tune the function bounds
                                // args: int *possible_return_code
                                //       func_t *pfn
                                //       ea_t max_func_end_ea (from the kernel's point of view)
                                // returns: none
        may_be_func,            // can a function start here?
                                // the instruction is in 'cmd'
                                // arg: int state -- autoanalysis phase
                                //   state == 0: creating functions
                                //         == 1: creating chunks
                                // returns: probability 0..100
                                // the idp module is allowed to modify 'cmd'
        is_sane_insn,           // is the instruction sane for the current file type?
                                // arg:  int no_crefs
                                // 1: the instruction has no code refs to it.
                                //    ida just tries to convert unexplored bytes
                                //    to an instruction (but there is no other
                                //    reason to convert them into an instruction)
                                // 0: the instruction is created because
                                //    of some coderef, user request or another
                                //    weighty reason.
                                // The instruction is in 'cmd'
                                // returns: 1-ok, <=0-no, the instruction isn't
                                // likely to appear in the program
        is_jump_func,           // is the function a trivial "jump" function?
                                // args:  func_t *pfn
                                //        ea_t *jump_target
                                //        ea_t *func_pointer
                                // returns: 0-no, 1-don't know, 2-yes, see jump_target
                                // and func_pointer
        gen_regvar_def,         // generate register variable definition line
                                // args:  regvar_t *v
                                // returns: 0-ok
        setsgr,                 // The kernel has changed a segment register value
                                // args:  ea_t startEA
                                //        ea_t endEA
                                //        int regnum
                                //        sel_t value
                                //        sel_t old_value
                                //        uchar tag (SR_... values)
                                // returns: 1-ok, 0-error
        set_compiler,           // The kernel has changed the compiler information
                                // (inf.cc structure)
        is_basic_block_end,     // Is the current instruction end of a basic block?
                                // This function should be defined for processors
                                // with delayed jump slots. The current instruction
                                // is stored in 'cmd'
                                // args:  bool call_insn_stops_block
                                // returns: 1-unknown, 0-no, 2-yes
        reglink,                // IBM PC only, ignore it
        get_vxd_name,           // IBM PC only, ignore it
                                // Get Vxd function name
                                // args: int vxdnum
                                //       int funcnum
                                //       char *outbuf
                                // returns: nothing

                                // PROCESSOR EXTENSION NOTIFICATIONS
                                // They are used to add support of new instructions
                                // to the existing processor modules.
                                // They should be processed only in notification callbacks
                                // set by hook_to_notification_point(HK_IDP,...)
        custom_ana,             // args: none, the address to analyze is in cmd.ea
                                //   cmd.ip and cmd.cs are initialized too
                                //   cmd.itype must be set >= 0x8000
                                //   cmd.size must be set to the instruction length
                                //   (good plugin would fill the whole 'cmd' including the operand fields)
                                //   in the case of error the cmd structure should be kept intact
                                // returns: 1+cmd.size
        custom_out,             // args: none (cmd structure contains information about the instruction)
                                //   optional notification
                                //   (depends on the processor module)
                                //   generates the instruction text using
                                //   the printf_line() function
                                // returns: 2
        custom_emu,             // args: none (cmd structure contains information about the instruction)
                                //   optional notification. if absent,
                                //   the instruction is supposed to be an regular one
                                //   the kernel will proceed to the analysis of the next instruction
                                // returns: 2
        custom_outop,           // args: op_t *op
                                //   optional notification to generate operand text. if absent,
                                //   the standard operand output function will be called.
                                //   the output buffer is inited with init_output_buffer()
                                //   and this notification may use out_...() functions from ua.hpp
                                //   to form the operand text
                                // returns: 2
        custom_mnem,            // args: char *outbuffer, size_t bufsize (cmd structure contains information about the instruction)
                                //   optional notification. if absent,
                                //   the IDC function GetMnem() won't work
                                // returns: 2
                                // At least one of custom_out or custom_mnem
                                // should be implemented. custom_ana should always be
                                // implemented. These custom_... callbacks will be
                                // called for all instructions. It is the responsability
                                // of the plugin to ignore the undesired callbacks
                                // END OF PROCESSOR EXTENSION NOTIFICATIONS

        undefine,               // An item in the database (insn or data) is being deleted
                                // args: ea_t ea
                                // returns: >0-ok, <=0-the kernel should stop
                                // if the return value is positive:
                                //   bit0 - ignored
                                //   bit1 - do not delete srareas at the item end
        make_code,              // An instruction is being created
                                // args: ea_t ea, asize_t size
                                // returns: 1-ok, <=0-the kernel should stop
        make_data,              // A data item is being created
                                // args: ea_t ea, flags_t flags, tid_t tid, asize_t len
                                // returns: 1-ok, <=0-the kernel should stop

        moving_segm,            // May the kernel move the segment?
                                // args: segment_t - segment to move
                                //       ea_t to   - new segment start address
                                //       int flags - combination of MSF_... bits
                                // returns: 1-yes, <=0-the kernel should stop
        move_segm,              // A segment is moved
                                // Fix processor dependent address sensitive information
                                // args: ea_t from  - old segment address
                                //       segment_t* - moved segment
                                // returns: nothing

        is_call_insn,           // Is the instruction a "call"?
                                // ea_t ea  - instruction address
                                // returns: 1-unknown, 0-no, 2-yes

        is_ret_insn,            // Is the instruction a "return"?
                                // ea_t ea  - instruction address
                                // bool strict - 1: report only ret instructions
                                //               0: include instructions like "leave"
                                //                  which begins the function epilog
                                // returns: 1-unknown, 0-no, 2-yes

        get_stkvar_scale_factor,// Should stack variable references be multiplied by
                                // a coefficient before being used in the stack frame?
                                // Currently used by TMS320C55 because the references into
                                // the stack should be multiplied by 2
                                // Returns: scaling factor
                                // Note: PR_SCALE_STKVARS should be set to use this callback

        create_flat_group,      // Create special segment representing the flat group
                                // (to use for PC mainly)
                                // args - ea_t image_base, int bitness, sel_t dataseg_sel

        kernel_config_loaded,   // This callback is called when ida.cfg is parsed
                                // args - none, returns - nothing

        might_change_sp,        // Does the instruction at 'ea' modify the stack pointer?
                                // args: ea_t ea
                                // returns: 1-yes, 0-false
                                // (not used yet)

        is_alloca_probe,        // Does the function at 'ea' behave as __alloca_probe?
                                // args: ea_t ea
                                // returns: 2-yes, 1-false

        out_3byte,              // Generate text representation of 3byte data
                                // init_out_buffer() is called before this function
                                // and all Out... function can be used.
                                // uFlag contains the flags.
                                // This callback might be implemented by the processor
                                // module to generate custom representation of 3byte data.
                                // args:
                                // ea_t dataea - address of the data item
                                // uint32 value - value to output
                                // bool analyze_only - only create xrefs if necessary
                                //              do not generate text representation
                                // returns: 2-yes, 1-false

        get_reg_name,           // Generate text representation of a register
                                // int reg        - internal register number as defined in the processor module
                                // size_t width   - register width in bytes
                                // char *buf      - output buffer
                                // size_t bufsize - size of output buffer
                                // int reghi      - if not -1 then this function will return the register pair
                                // returns: -1 if error, strlen(buf)+2 otherwise
                                // Most processor modules do not need to implement this callback
                                // It is useful only if ph.regNames[reg] does not provide
                                // the correct register names
        savebase,               // The database is being saved. Processor module should
                                // save its local data
        gen_asm_or_lst,         // Callback: generating asm or lst file
                                // The kernel calls this callback twice, at the beginning
                                // and at the end of listing generation. Processor
                                // module can intercept this event and adjust its output
                                // bool starting - beginning listing generation
                                // FILE *fp      - output file
                                // bool is_asm   - true:assembler, false:listing
                                // int flags     - flags passed to gen_file()
                                // gen_outline_t **outline - ptr to ptr to outline callback
                                // the outline callback, if defined by gen_asm_or_lst,
                                // will be used by the kernel to output the generated lines
                                // returns: nothing
        out_src_file_lnnum,     // Callback: generate analog of
                                //   #line "file.c" 123
                                // directive.
                                // const char *file - source file (may be NULL)
                                // size_t lnnum     - line number
                                // returns: 2-directive has been generated
        get_autocmt,            // Callback: get dynamic auto comment
                                // Will be called if the autocomments are enabled
                                // and the comment retrieved from ida.int starts with
                                // '$!'. 'cmd' contains valid info.
                                // char *buf  - output buffer
                                // size_t bufsize - output buffer size
                                // returns: 2-new comment has been generated
                                //          1-callback has not been handled
                                //            the buffer must not be changed in this case
        is_insn_table_jump,     // Callback: determine if instruction is a table jump or call
                                // If CF_JUMP bit can not describe all kinds of table
                                // jumps, please define this callback.
                                // It will be called for insns with CF_JUMP bit set.
                                // input: cmd structure contains the current instruction
                                // returns: 1-yes, 0-no
        auto_empty_finally,     // Info: all analysis queues are empty definitively
                                // args: none
                                // returns: none
                                // This callback is called only once.
                                // See also auto_empty.
        loader_finished,        // Event: external file loader finished its work
                                // linput_t *li
                                // uint16 neflags
                                // const char *filetypename
                                // Use this event to augment the existing loader functionality
        loader_elf_machine,     // Event: ELF loader machine type checkpoint
                                //   linput_t *li
                                //   int machine_type
                                //   const char **p_procname
                                //   proc_def **p_pd (see ldr\elf.h)
                                //   set_elf_reloc_t *set_reloc
                                // A plugin check of the machine_type. If it is the desired one,
                                // the the plugin fills p_procname with the processor name.
                                // (one of the names present in ph.psnames)
                                // p_pd is used to handle relocations, otherwise can be left untouched
                                // set_reloc can be later used by the plugin to specify relocations
                                // returns: e_machine value (if it is different from the
                                // original e_machine value, procname and p_pd will be ignored
                                // and the new value will be used)
                                // This event occurs for each newly loaded ELF file
        is_indirect_jump,       // Callback: determine if instruction is an indrect jump
                                // If CF_JUMP bit can not describe all jump types
                                // jumps, please define this callback.
                                // input: cmd structure contains the current instruction
                                // returns: 1-use CF_JUMP, 2-no, 3-yes
        verify_noreturn,        // The kernel wants to set 'noreturn' flags for a function
                                // func_t *pfn
                                // Returns: 1-ok, any other value-do not set 'noreturn' flag
        verify_sp,              // All function instructions have been analyzed
                                // Now the processor module can analyze the stack pointer
                                // for the whole function
                                // input: func_t *pfn
                                // Returns: 1-ok, 0-bad stack pointer
        renamed,                // The kernel has renamed a byte
                                // args:
                                //   ea_t ea
                                //   const char *new_name
                                //   bool local_name
                                // Returns: nothing. See also the 'rename' event
        add_func,               // The kernel has added a function
                                // args: func_t *pfn
                                // Returns: nothing
        del_func,               // The kernel is about to delete a function
                                // args: func_t *pfn
                                // Returns: 1-ok,<=0-do not delete
        set_func_start,         // Function chunk start address will be changed
                                // args: func_t *pfn
                                //       ea_t new_start
                                // Returns: 1-ok,<=0-do not change
        set_func_end,           // Function chunk end address will be changed
                                // args: func_t *pfn
                                //       ea_t new_end
                                // Returns: 1-ok,<=0-do not change
        treat_hindering_item,   // An item hinders creation of another item
                                // args: ea_t hindering_item_ea
                                //       flags_t new_item_flags (0 for code)
                                //       ea_t new_item_ea
                                //       asize_t new_item_length
                                // Returns: 1-no reaction, <=0-the kernel may delete the hindering item
        str2reg,                // Convert a register name to a register number
                                // args: const char *regname
                                // Returns: register number + 2
                                // The register number is the register index in the regNames array
                                // Most processor modules do not need to implement this callback
                                // It is useful only if ph.regNames[reg] does not provide
                                // the correct register names
        create_switch_xrefs,    // Create xrefs for a custom jump table
                                // in: ea_t jumpea;        - address of the jump insn
                                //     switch_info_ex_t *; - switch information
                                // returns: must return 2
        calc_switch_cases,      // Calculate case values and targets for a custom jump table
                                // in:  ea_t insn_ea - address of the 'indirect jump' instruction
                                //      switch_info_ex_t *si      - switch information
                                //      casevec_t *casevec - vector of case values...
                                //      evec_t *targets - ...and corresponding target addresses
                                // casevec and targets may be NULL
                                // returns: 2-ok, 1-failed
        determined_main,        // The main() function has been determined
                                // in:  ea_t main - address of the main() function
                                // returns: none
        preprocess_chart,       // gui has retrieved a function flow chart
                                // in: qflow_chart_t *fc
                                // returns: none
                                // Plugins may modify the flow chart in this callback
        get_bg_color,           // Get item background color
                                // in: ea_t ea, bgcolor_t *color
                                // Returns: 1-not implemented, 2-color set
                                // Plugins can hook this callback to color disassembly lines
                                // dynamically
        validate_flirt_func,    // flirt has recognized a library function
                                // this callback can be used by a plugin or proc module
                                // to intercept it and validate such a function
                                // args: ea_t start_ea
                                //       const char *funcname
                                // returns: -1-do not create a function,
                                //           1-function is validated
                                // the idp module is allowed to modify 'cmd'
        get_operand_string,     // Request text string for operand (cli, java, ...)
                                // args: int opnum
                                //       char *buf
                                //       size_t buflen
                                // (cmd structure must contain info for the desired insn)
                                // opnum is the operand number; -1 means any string operand
                                // returns: 1 - no string (or empty string)
                                //         >1 - original string length with terminating zero

                                // the following 5 events are very low level
                                // take care of possible recursion
        add_cref,               // a code reference is being created
                                // args: ea_t from, ea_t to, cref_t type
                                // returns: <0 - cancel cref creation
        add_dref,               // a data reference is being created
                                // args: ea_t from, ea_t to, dref_t type
                                // returns: <0 - cancel dref creation
        del_cref,               // a code reference is being deleted
                                // args: ea_t from, ea_t to, bool expand
                                // returns: <0 - cancel cref deletion
        del_dref,               // a data reference is being deleted
                                // args: ea_t from, ea_t to
                                // returns: <0 - cancel dref deletion
        coagulate_dref,         // data reference is being analyzed
                                // args: ea_t from, ea_t to, bool may_define, ea_t *code_ea
                                // plugin may correct code_ea (e.g. for thumb mode refs, we clear the last bit)
                                // returns: <0 - cancel dref analysis
        register_custom_fixup,  // request to register a new custom fixup type
                                // args: const char *name
                                // returns: fixup id + 1
        custom_refinfo,         // called from get_offset_expr, when refinfo_t
                                // contains flag REFINFO_CUSTOM. Normally this
                                // notification used in a combination with custom_fixup
                                // args: ea_t ea, int numop, ea_t* opval, const refinfo_t* ri,
                                //       char* buf, size_t bufsize, ea_t* target,
                                // ea_t* fullvalue, ea_t from, int getn_flags
                                // returns: 2 - buf filled as simple expression
                                //          3 - buf filled as complex expression
                                //          4 - apply standard processing (with - possible - changed values)
                                //     others - can't convert to offset expression

        set_proc_options,       // called if the user specified an option string in the command line:
                                //  -p<processor name>:<options>
                                // can be used for e.g. setting a processor subtype
                                // also called if option string is passed to set_processor_type()
                                // and IDC's SetProcessorType()
                                // args: const char * options
                                // returns: <0 - bad option string

        adjust_libfunc_ea,      // called when a signature module has been matched against
                                // bytes in the database. This is used to compute the
                                // offset at which a particular module's libfunc should
                                // be applied.
                                // args: const idasgn_t *sig, const libfunc_t *libfun, ea_t *ea
                                // returns: 2 - the ea_t pointed to by the third argument was modified.
                                //     others - not modified. Use default algorithm.
                                //
                                // Note: "*ea" initially contains the ea_t of the start
                                //       of the pattern match.

        extlang_changed,        // the list of extlangs or the default extlang was changed
                                // int kind
                                //    0 - extlang installed
                                //    1 - extlang removed
                                //    2 - default extlang changed
                                // const extlang_t *el - pointer to the extlang affected
                                // Returns: nothing

        last_cb_before_debugger,
        // START OF DEBUGGER CALLBACKS
        obsolete_get_operand_info = 100, // Get operand information
                                // same as get_operand_info (below), but uses
                                // idd_opinfo_old_t* as the last argument (less info)

        OBSOLETE(get_reg_info), // the same as get_reg_info2 but the
                                // position and size of the subvalue is defined
                                // by uint64 (not bitrange_t) and
                                // the 3d argument is uint64 *mask (0-no mask)
                                // example: "ah" returns main_regname="eax" and mask=0xFF00
#ifdef NO_OBSOLETE_FUNCS        //
        next_exec_insn,         // Get next address to be executed
#else                           //
        get_jump_target,        // Get jump target (old name)
#endif                          //
                                // ea_t ea                   - instruction address
                                // int tid                   - current therad id
                                // const regval_t &(*idaapi getreg)(const char *name,
                                //                                  const regval_t *regvalues))
                                //                           - function to get register values
                                // const regval_t *regvalues - register values array
                                // ea_t *target              - pointer to the answer
                                // This function must return the next address to be executed.
                                // If the instruction following the current one is executed, then it must return BADADDR
                                // Usually the instructions to consider are: jumps, branches, calls, returns
                                // This function is essential if the 'single step' is not supported in hardware
                                // returns: 1-unimplemented, 0-implemented

        calc_step_over,         // Calculate the address of the instruction which will be
                                // executed after "step over". The kernel will put a breakpoint there.
                                // If the step over is equal to step into or we can not calculate
                                // the address, return BADADDR.
                                // ea_t ip - instruction address
                                // ea_t *target - pointer to the answer
                                // returns: 1-unimplemented, 0-implemented

        get_macro_insn_head,    // Calculate the start of a macro instruction
                                // This notification is called if IP points to the middle of an instruction
                                // ea_t ip - instruction address
                                // ea_t *head - answer, BADADDR means normal instruction
                                // returns: 1-unimplemented, 0-implemented

        get_dbr_opnum,          // Get the number of the operand to be displayed in the
                                // debugger reference view (text mode)
                                // ea_t ea - instruction address
                                // int *opnum - operand number (out, -1 means no such operand)
                                // returns: 1-unimplemented, 0-implemented

        insn_reads_tbit,        // Check if insn will read the TF bit
                                // ea_t ea - instruction address
                                // const regval_t &(*idaapi getreg)(const char *name,
                                //                                  const regval_t *regvalues))
                                //                           - function to get register values
                                // const regval_t *regvalues - register values array
                                // returns: 3-yes, will generate 'step' exception
                                //          2-yes, will store the TF bit in memory
                                //          1-no

        get_operand_info,       // Get operand information.
                                // This callback is used to calculate the operand
                                // value for double clicking on it, hints, etc
                                // ea_t ea  - instruction address
                                // int n    - operand number
                                // int thread_id - current thread id
                                // const regval_t &(*idaapi getreg)(const char *name,
                                //                                  const regval_t *regvalues))
                                //                           - function to get register values
                                // const regval_t *regvalues - register values array
                                // idd_opinfo_t *opinf       - the output buffer
                                // returns: 0-ok, otherwise failed

        calc_next_eas,          // Calculate list of addresses the instruction in cmd
                                // may pass control to.
                                // bool over - calculate for step over (ignore call targets)
                                // ea_t *res - array for the results.
                                //             This array has NEXTEAS_ANSWER_SIZE elements.
                                // int *nsubcalls - number of addresses of called functions
                                //                  in the above array. they must be put
                                //                  at the beginning of the array.
                                //                  if over=true, this answer will be zero.
                                // returns: number of calculated addresses+1
                                // If there are too many addresses or they are
                                // incalculable (indirect jumps, for example), return -1.
                                // This callback is required for source level debugging.

        clean_tbit,             // Clear the TF bit after an insn like pushf stored it in memory
                                // ea_t ea - instruction address
                                // const regval_t &(*idaapi getreg)(const char *name,
                                //                                  const regval_t *regvalues))
                                //                           - function to get register values
                                // const regval_t *regvalues - register values array
                                // returns: 2-ok, 1-failed

        get_reg_info2,          // Get register information by its name
                                // const char *regname
                                // const char **main_regname (NULL-failed)
                                // bitrange_t *bitrange - position and size of the value within 'main_regname' (empty bitrange == whole register)
                                // returns: 1-unimplemented, 2-implemented
                                // example: "ah" returns:
                                //   main_regname="eax"
                                //   bitrange_t = { offset==8, nbits==8 }
                                // this callback may be unimplemented if the register
                                // names are all present in ph.regNames and they all have
                                // the same size

        // END OF DEBUGGER CALLBACKS

        // START OF TYPEINFO CALLBACKS
                                // The codes below will be called only if PR_TYPEINFO is set.
                                // The codes based_ptr, max_ptr_size, get_default_enum_size MUST be implemented.
                                // (other codes are optional but still require for normal
                                // operation of the type system. without calc_arglocs3,
                                // for example, ida will not know about the argument
                                // locations for function calls.

        OBSOLETE(decorate_name)=500,
                                // Decorate/undecorate a C symbol name
                                // const til_t *ti    - pointer to til
                                // const char *name   - name of symbol
                                // const type_t *type - type of symbol. If NULL then it will try to guess.
                                // char *outbuf       - output buffer
                                // size_t bufsize     - size of the output buffer
                                // bool mangle        - true-mangle, false-unmangle
                                // cm_t cc            - real calling convention for VOIDARG functions
                                // returns: true if success

        setup_til,              // Setup default type libraries (called after loading
                                // a new file into the database)
                                // The processor module may load tils, setup memory
                                // model and perform other actions required to set up
                                // the type system
                                // args:    none
                                // returns: nothing
                                // Optional callback

        based_ptr,              // get prefix and size of 'segment based' ptr
                                // type (something like char _ss *ptr)
                                // see description in typeinf.hpp
                                // args:  unsigned ptrt
                                //        const char **ptrname (output arg)
                                // returns: size of type

        max_ptr_size,           // get maximal size of a pointer in bytes
                                // args:  none
                                // returns: max possible size of a pointer plus 1

        get_default_enum_size,  // get default enum size
                                // args:  cm_t cm
                                // returns: sizeof(enum)

        OBSOLETE(calc_arglocs),
        OBSOLETE(use_stkarg_type),
        OBSOLETE(use_regarg_type),
        OBSOLETE(use_arg_types),
        OBSOLETE(get_fastcall_regs),
        OBSOLETE(get_thiscall_regs),
        OBSOLETE(calc_cdecl_purged_bytes),
        OBSOLETE(get_stkarg_offset),
        OBSOLETE(calc_purged_bytes),
        OBSOLETE(calc_arglocs2),
        OBSOLETE(calc_retloc),
        OBSOLETE(calc_varglocs),
        OBSOLETE(get_varcall_regs),
        OBSOLETE(use_regarg_type2),
        OBSOLETE(use_arg_types2),
        OBSOLETE(get_fastcall_regs2),
        OBSOLETE(get_thiscall_regs2),
        OBSOLETE(get_varcall_regs2),

        calc_cdecl_purged_bytes2,// calculate number of purged bytes after call
                                // args: ea_t - address of the call instruction
                                // returns: number of purged bytes+2 (usually add sp, N)

        get_stkarg_offset2,     // get offset from SP to the first stack argument
                                // args: none
                                // returns: the offset + 2
                                // for example: pc: 0, hppa: -0x34, ppc: 0x38

        til_for_file,           // internal notification, do not use

        equal_reglocs,          // are 2 register arglocs the same?
                                // args:    argloc_t *a1
                                //          argloc_t *a2
                                // we need this callback for the pc module
                                // returns: 1-not implemented, 2-yes, -1-no

        decorate_name3,         // Decorate/undecorate a C symbol name
                                // qstring *outbuf    - output buffer
                                // const char *name   - name of symbol
                                // bool mangle        - true-mangle, false-unmangle
                                // cm_t cc            - calling convention
                                // returns: 2 if success
                                // the kernel uses this callback only if PR_TINFO is set

        calc_retloc3,           // calculate return value location
                                // args:
                                //   const tinfo_t *rettype
                                //   cm_t cc
                                //   argloc_t *retloc - the result
                                // returns: 1-not implemented, 2-ok, -1-error
                                // the kernel uses this callback only if PR_TINFO is set

        calc_varglocs3,         // calculate locations of the arguments that correspond to '...'
                                // args:
                                //   const func_type_data_t *ftd - info about all arguments (including varargs)
                                //   regobjs_t *regs             - buffer for register values
                                //   relobj_t *stkargs           - stack arguments
                                //   int nfixed                  - number of fixed arguments
                                // returns: 1-not implemented, 2-ok, -1-error
                                // the kernel uses this callback only if PR_TINFO is set

        calc_arglocs3,          // calculate function argument locations
                                // args:    func_type_data_t *fti - points to the func type info
                                // the callback should fill retloc, all arglocs, and stkargs
                                // This callback supersedes calc_argloc2.
                                // returns: 1-not implemented, 2-ok, -1-error
                                // this callback is never called for CM_CC_SPECIAL functions
                                // the kernel uses this callback only if PR_TINFO is set

        use_stkarg_type3,       // use information about a stack argument
                                // args:
                                //   ea_t ea            - address of the push instruction which
                                //                        pushes the function argument into the stack
                                //   const funcarg_t *arg - argument info
                                // returns: true - ok, false - failed, the kernel will create
                                //          a comment with the argument name or type for the instruction
                                // the kernel uses this callback only if PR_TINFO is set

        use_regarg_type3,       // use information about register argument
                                // args:
                                //   int *idx             - pointer to the returned value
                                //   ea_t ea              - address of the instruction
                                //   const funcargvec_t *rargs - vector of register arguments
                                //                               (including regs extracted from scattered arguments)
                                // at the end, *idx may contain the following values:
                                //   idx of the used argument - if the argument is defined in the current instruction
                                //                              a comment will be applied by the kernel
                                //   idx|REG_SPOIL            - argument is spoiled by the instruction
                                //   -1                       - if the instruction doesn't change any registers
                                //   -2                       - if the instruction spoils all registers
                                // returns: 2
                                // the kernel uses this callback only if PR_TINFO is set

        use_arg_types3,         // use information about callee arguments
                                // args:
                                //   ea_t ea              - address of the call instruction
                                //   func_type_data *fti  - info about function type
                                //   funcargvec_t *rargs  - array of register arguments
                                // returns: 2 (and removes handled arguments from fti and rargs)
                                // the kernel uses this callback only if PR_TINFO and PR_USE_ARG_TYPES are set

        calc_purged_bytes3,     // calculate number of purged bytes by the given function type
                                // args:
                                //   int *p_purged_bytes - ptr to output
                                //   const func_type_data_t *fti - func type details
                                // returns: 2
                                // the kernel uses this callback only if PR_TINFO is set

        shadow_args_size,       // get size of shadow args in bytes
                                // args:
                                //   int *shadow_args_size
                                //   func_t *pfn (may be NULL)
                                // returns: 2 if filled *shadow_args_size

        get_varcall_regs3,      // get register allocation convention used in the ellipsis (...) calling convention
                                // args: callregs_t * - see typeinf.hpp
                                // returns: max_possible_number_of_varcall_regs+2

        get_fastcall_regs3,     // get register allocation convention used in the fastcall calling convention
                                // args: callregs_t * - see typeinf.hpp
                                // returns: max_possible_number_of_fastcall_regs+2

        get_thiscall_regs3,     // get register allocation convention used in the thiscall calling convention
                                // args: callregs_t * - see typeinf.hpp
                                // returns: max_possible_number_of_thiscall_regs+2
        // END OF TYPEINFO CALLBACKS

        loader=1000,            // this code and higher ones are reserved
                                // for the loaders.
                                // the arguments and the return values are
                                // defined by the loaders
  };

  int (idaapi *notify)(idp_notify msgid, ...); // Various notifications for the idp

// Get the stack variable scaling factor
// Useful for processors who refer to the stack with implicit scaling factor.
// TMS320C55 for example: SP(#1) really refers to (SP+2)

//
// The following functions generate portions of the disassembled text.
//
  void  (idaapi *header)(void);                // function to produce start of disassembled text
  void  (idaapi *footer)(void);                // function to produce end of disassembled text

  void  (idaapi *segstart)(ea_t ea);          // function to produce start of segment
  void  (idaapi *segend)  (ea_t ea);          // function to produce end of segment

  void  (idaapi *assumes) (ea_t ea);          // function to produce assume directives
                                        // when segment register value changes
                                        // if your processor has no segment
                                        // registers, you may define it as NULL

// Analyze one instruction and fill 'cmd' structure.
// cmd.ea contains address of instruction to analyze.
// Return length of the instruction in bytes, 0 if instruction can't be decoded.
// This function shouldn't change the database, flags or anything else.
// All these actions should be performed only by u_emu() function.

  int   (idaapi *u_ana)   (void);

//
// Emulate instruction, create cross-references, plan to analyze
// subsequent instructions, modify flags etc. Upon entrance to this function
// all information about the instruction is in 'cmd' structure.
// If zero is returned, the kernel will delete the instruction.

  int   (idaapi *u_emu)   (void);

// Generate text representation of an instruction in 'cmd' structure.
// This function shouldn't change the database, flags or anything else.
// All these actions should be performed only by u_emu() function.

  void  (idaapi *u_out)   (void);

// Generate text representation of an instructon operand.
// This function shouldn't change the database, flags or anything else.
// All these actions should be performed only by u_emu() function.
// The output text is placed in the output buffer initialized with init_output_buffer()
// This function uses out_...() functions from ua.hpp to generate the operand text
// Returns: 1-ok, 0-operand is hidden.

  bool  (idaapi *u_outop) (op_t &op);


// Generate text represenation of data items
// This function MAY change the database and create cross-references, etc.

  void  (idaapi *d_out)   (ea_t ea);          // disassemble data

// Compare instruction operands.
// Returns 1-equal,0-not equal operands.
// This pointer may be NULL.

  bool  (idaapi *cmp_opnd)(const op_t &op1, const op_t &op2);

// Can the operand have a type as offset, segment, decimal, etc.
// (for example, a register AX can't have a type, meaning that the user can't
// change its representation. see bytes.hpp for information about types and flags)
// This pointer may be NULL.

  bool  (idaapi *can_have_type)(op_t &op);

//
//      Processor register information:
//

  int   regsNum;                        // number of registers
  const char *const *regNames;          // their names

// The following pointers should be NULL:

  const AbstractRegister *(idaapi *getreg)(int regnum);
                                        // Get register value.
                                        // If specified, will be
                                        // used in the determining predefined
                                        // comment based on the register value

  int   rFiles;                         // number of register files
  const char *const *rFnames;           // register names for files
  const rginfo *rFdescs;                // description of registers
  const WorkReg *CPUregs;               // pointer to CPU registers

// Segment register information (use virtual CS and DS registers if your
// processor doesn't have segment registers):

  int   regFirstSreg;                   // number of first segment register
  int   regLastSreg;                    // number of last segment register
  int   segreg_size;                    // size of a segment register in bytes

// You should define 2 virtual segment registers for CS and DS.
// Let's call them rVcs and rVds.

  int   regCodeSreg;                    // number of CS register
  int   regDataSreg;                    // number of DS register

//
//      Empirics
//

  const bytes_t *codestart;             // Array of typical code start sequences
                                        // This array is used when a new file
                                        // is loaded to find the beginnings of code
                                        // sequences.
                                        // This array is terminated with
                                        // a zero length item.
  const bytes_t *retcodes;              // Array of 'return' instruction opcodes
                                        // This array is used to determine
                                        // form of autogenerated locret_...
                                        // labels.
                                        // The last item of it should be { 0, NULL }
                                        // This array may be NULL
                                        // Better way of handling return instructions
                                        // is to define the is_ret_insn callback in
                                        // the notify() function

//
//      Instruction set
//

  int   instruc_start;                  // icode of the first instruction
  int   instruc_end;                    // icode of the last instruction + 1

  const instruc_t *instruc;             // Array of instructions

// is indirect far jump or call instruction?
// meaningful only if the processor has 'near' and 'far' reference types

  int   (idaapi *is_far_jump)(int icode);


//      Translation function for offsets
//      Currently used in the offset display functions
//      to calculate the referenced address
//
  ea_t (idaapi *translate)(ea_t base, adiff_t offset);

//
//      Size of long double (tbyte) for this processor
//      (meaningful only if ash.a_tbyte != NULL)
//
  size_t tbyte_size;

//
//      Floating point -> IEEE conversion function
// error codes returned by this function (load/store):
#define REAL_ERROR_FORMAT  -1 // not supported format for current .idp
#define REAL_ERROR_RANGE   -2 // number too big (small) for store (mem NOT modifyed)
#define REAL_ERROR_BADDATA -3 // illegal real data for load (IEEE data not filled)
//
  int (idaapi *realcvt)(void *m, uint16 *e, uint16 swt);

//
// Number of digits in floating numbers after the decimal point.
// If an element of this array equals 0, then the corresponding
// floating point data is not used for the processor.
// This array is used to align numbers in the output.
//      real_width[0] - number of digits for short floats (only PDP-11 has them)
//      real_width[1] - number of digits for "float"
//      real_width[2] - number of digits for "double"
//      real_width[3] - number of digits for "long double"
// Example: IBM PC module has { 0,7,15,19 }
//
  char real_width[4];

//
//  Find 'switch' idiom
//      fills 'si' structure with information and returns 1
//      returns 0 if switch is not found.
//      input: 'cmd' structure is correct.
//      this function may use and modify 'cmd' structure
//      it will be called for instructions marked with CF_JUMP
//
  bool (idaapi *is_switch)(switch_info_ex_t *si);

//
//  Generate map file. If this pointer is NULL, the kernel itself
//  will create the map file.
//  This function returns number of lines in output file.
//  0 - empty file, -1 - write error
//
  int32 (idaapi *gen_map_file)(FILE *fp);

//
//  Extract address from a string. Returns BADADDR if can't extract.
//  Returns BADADDR-1 if kernel should use standard algorithm.
//
  ea_t (idaapi *extract_address)(ea_t ea,const char *string,int x);

//
//  Check whether the operand is relative to stack pointer or frame pointer.
//  This function is used to determine how to output a stack variable
//  This function may be absent. If it is absent, then all operands
//  are sp based by default.
//  Define this function only if some stack references use frame pointer
//  instead of stack pointer.
//  returns flags:
#define OP_FP_BASED  0x00000000 // operand is FP based
#define OP_SP_BASED  0x00000001 // operand is SP based
#define OP_SP_ADD    0x00000000 // operand value is added to the pointer
#define OP_SP_SUB    0x00000002 // operand value is substracted from the pointer
//
   int (idaapi *is_sp_based)(const op_t &x);

//
//  Create a function frame for a newly created function.
//  Set up frame size, its attributes etc.
//  This function may be absent.
//
   bool (idaapi *create_func_frame)(func_t *pfn);


// Get size of function return address in bytes
//      pfn - pointer to function structure, can't be NULL
// If this function is absent, the kernel will assume
//      4 bytes for 32-bit function
//      2 bytes otherwise

   int (idaapi *get_frame_retsize)(func_t *pfn);


//  Generate stack variable definition line
//  If this function is NULL, then the kernel will create this line itself.
//  Default line is
//              varname = type ptr value
//  where 'type' is one of byte,word,dword,qword,tbyte
//
   void (idaapi *gen_stkvar_def)(char *buf,
                                 size_t bufsize,
                                 const member_t *mptr,
                                 sval_t v);


// Generate text representation of an item in a special segment
// i.e. absolute symbols, externs, communal definitions etc.
// returns: 1-overflow, 0-ok

   bool (idaapi *u_outspec)(ea_t ea,uchar segtype);


// Icode of return instruction. It is ok to give any of possible return
// instructions

   int icode_return;


// Set IDP-specific option (see below)
  set_options_t *set_idp_options;

//      Is the instruction created only for alignment purposes?
//      returns: number of bytes in the instruction
//      Do not directly call this function, use ::is_align_insn()

  int (idaapi *is_align_insn)(ea_t ea);

//      Micro virtual machine description
//      If NULL, IDP doesn't support microcodes.

  mvm_t *mvm;

//      If the FIXUP_VHIGH and FIXUP_VLOW fixup types are supported
//      then the number of bits in the HIGH part. For example,
//      SPARC will have here 22 because it has HIGH22 and LOW10 relocations.
//      See also: the description of PR_FULL_HIFXP bit

  int high_fixup_bits;

};
processor_t processor;

// Events marked as '*' should be used as a parameter to callui()
// See convenience functions below (like get_screen_ea())
// Events marked as 'cb' are designed to be callbacks and should not
// be used in callui(). The user may hook to HT_UI events to catch them

enum ui_notification_t
{
  ui_null = 0,

  ui_range,             // cb: the disassembly range have been changed (inf.minEA..inf.maxEA)
                        // UI should redraw the scrollbars
                        // See also: lock_range_refresh
                        // Parameters: none
                        // Returns:    none

  ui_list,              // cb: the list (chooser) window contents have been changed
                        // (names, signatures, etc) UI should redraw them
                        // Parameters: none
                        // Returns:    none
                        // Please consider request_refresh() instead

  ui_idcstart,          // cb: Start of IDC engine work
                        // Parameters: none
                        // Returns:    none

  ui_idcstop,           // cb: Stop of IDC engine work
                        // Parameters: none
                        // Returns:    none

  ui_suspend,           // cb: Suspend graphical interface.
                        // Only the text version
                        // interface should response to it
                        // Parameters: none
                        // Returns:    none

  ui_resume,            // cb: Resume the suspended graphical interface.
                        // Only the text version
                        // interface should response to it
                        // Parameters: none
                        // Returns:    none

  ui_old_jumpto,        // * Jump to the specified address
                        // Parameters:
                        //      ea_t ea
                        //      int operand_num (-1: don't change x coord)
                        // Returns: bool success

  ui_readsel,           // * Get the selected area boundaries
                        // Parameters:
                        //      ea_t *startea
                        //      ea_t *endea
                        // Returns: bool
                        //          0 - no area is selected
                        //          1 - ok, startea and endea are filled
                        // See also: ui_readsel2

  ui_unmarksel,         // * Unmark selection
                        // Parameters: none
                        // Returns:    none

  ui_screenea,          // * Return the address at the screen cursor
                        // Parameters: ea_t *result
                        // Returns:    none

  ui_saving,            // cb: The kernel is flushing its buffers to the disk.
                        // The user interface should save its state.
                        // Parameters: none
                        // Returns:    none

  ui_saved,             // cb: The kernel has saved the database.
                        // This callback just informs the interface.
                        // Parameters: none
                        // Returns:    none

  ui_refreshmarked,     // * Refresh marked windows
                        // Parameters: none
                        // Returns:    none

  ui_refresh,           // * Refresh all disassembly views
                        // Parameters: none
                        // Returns:    none
                        // Forces an immediate refresh.
                        // Please consider request_refresh() instead

  ui_choose,            // * Allow the user to choose an object
                        // Parameters:
                        //      choose_type_t type
                        //      ...
                        // other parameters depend on the 'type'
                        // see below for inline functions using this
                        // notification code.
                        // Always use the helper inline functions below.
                        // Returns: depends on the 'type'

  ui_close_chooser,     // * Close a non-modal chooser
                        // Parameters:
                        //      const char *title
                        // Returns: bool success

  ui_banner,            // * Show a banner dialog box
                        // Parameters:
                        //      int wait
                        // Returns: bool 1-ok, 0-esc was pressed

  ui_setidle,           // * Set a function to call at idle times
                        // Parameters:
                        //      int (*func)(void);
                        // Returns: none

  ui_noabort,           // * Disable 'abort' menu item - the database was not
                        // compressed
                        // Parameters: none
                        // Returns:    none

  ui_term,              // cb: IDA is terminated
                        // The database is already closed.
                        // The UI may close its windows in this callback.
                        // Parameters: none
                        // Returns:    none

  ui_mbox,              // * Show a message box
                        // Parameters:
                        //      mbox_kind_t kind
                        //      const char *format
                        //      va_list va
                        // Returns: none

  ui_beep,              // * Beep
                        // Parameters:
                        //      beep_t beep_type
                        // Returns:    none

  ui_msg,               // * Show a message in the message window
                        // Parameters:
                        //      const char *format
                        //      va_list va
                        // Returns: number of bytes output

  ui_askyn,             // * Ask the user and get his yes/no response
                        // Parameters:
                        //      const char *yes_button
                        //      const char *no_button
                        //      const char *cancel_button
                        //      int default_answer (ASKBTN_XXX)
                        //      const char *format
                        //      va_list va
                        // Returns: ASKBTN_XXX value

  ui_askfile,           // * Ask the user a file name
                        // Parameters:
                        //      int savefile
                        //      const char *default_answer
                        //      const char *format
                        //      va_list va
                        // Returns: file name

  ui_form,              // * Show a modal dialog form
                        // Parameters:
                        //      const char *format
                        //      va_list va
                        // Returns: bool 0-esc, 1-ok

  ui_close_form,        // * Close the form
                        // This function may be called from pushbutton
                        // callbacks in ui_form
                        //      TView *fields[]
                        //      int is_ok
                        // Returns: none

  ui_clearbreak,        // * clear ctrl-break flag
                        // Parameters: none
                        // Returns: none
                        // NB: this call is also used to get ida version

  ui_wasbreak,          // * test the ctrl-break flag
                        // Parameters: none
                        // Returns: 1 - Ctrl-Break is detected, a message is displayed
                        //          2 - Ctrl-Break is detected again, a message is not displayed
                        //          0 - Ctrl-Break is not detected

  ui_asktext,           // * Ask text
                        // Parameters:
                        //      size_t size
                        //      char *answer
                        //      const char *default_value
                        //      const char *format
                        //      va_list va
                        // Returns: the entered text

  ui_askstr,            // * Ask a string
                        // Parameters:
                        //      int history_number
                        //      const char *default_value
                        //      const char *format
                        //      va_list va
                        // Returns: the entered string

  ui_askident,          // * Ask an identifier
                        // Parameters:
                        //      const char *default_value
                        //      const char *format
                        //      va_list va
                        // Returns: cptr the entered identifier

  ui_askaddr,           // * Ask an address
                        // Parameters:
                        //      ea_t *answer
                        //      const char *format
                        //      va_list va
                        // Returns: bool success

  ui_askseg,            // * Ask a segment
                        // Parameters:
                        //      sel_t *answer
                        //      const char *format
                        //      va_list va
                        // Returns: bool success

  ui_asklong,           // * Ask a long
                        // Parameters:
                        //      sval_t *answer
                        //      const char *format
                        //      va_list va
                        // Returns: bool success

  ui_showauto,          // * Show the autoanalysis state
                        // Parameters:
                        //      ea_t ea
                        //      int auto_t (see auto.hpp)
                        // Returns: none

  ui_setstate,          // * Show READY, BUSY, THINKING, etc
                        // Parameters:
                        //      int idastate_t (see auto.hpp)
                        // Returns: int: old ida state

  ui_add_idckey,        // * Add hotkey for IDC function
                        // After this function the UI should call the
                        // specified IDC function
                        // when the hotkey is pressed
                        // Parameters:
                        //      const char *hotkey
                        //      const char *idcfuncname
                        // Returns: int code
#define IDCHK_OK        0       // ok
#define IDCHK_ARG       -1      // bad argument(s)
#define IDCHK_KEY       -2      // bad hotkey name
#define IDCHK_MAX       -3      // too many IDC hotkeys

  ui_del_idckey,        // * Delete IDC function hotkey
                        // Parameters:
                        //      hotkey  - hotkey name
                        // Returns: bool success

  ui_old_get_marker,    // * Get pointer to function
                        // "void mark_idaview_for_refresh(ea_t ea)"
                        // This function will be called by the kernel when the
                        // database is changed
                        // Parameters: none
                        // Returns: vptr: (idaapi*marker)(ea_t ea) or NULL
                        // OBSOLETE

  ui_analyzer_options,  // * Allow the user to set analyzer options
                        // (show a dialog box)
                        // Parameters: none
                        // Returns: none

  ui_is_msg_inited,     // * Can we use msg() functions?
                        // Parameters: none
                        // Returns: bool cnd

  ui_load_file,         // Display a load file dialog and load file
                        // Parameters:
                        //      const char *filename
                        //              the name of input file as is
                        //              (if the input file is from library,
                        //               then this is the name in the library)
                        //      linput_t *li
                        //              loader input source
                        //      ushort neflags
                        //              combination of NEF_... bits
                        //              (see loader.hpp)
                        // Returns: bool cnd;

  ui_run_dbg,           // * Load a debugger plugin and run the specified program
                        // Parameters:
                        //      const char *dbgopts - value of the -r command line switch
                        //      const char *exename - name of the file to run
                        //      int argc            - number of arguments for the executable
                        //      char **argv         - argument vector
                        // Returns: bool cnd

  ui_get_cursor,        // * Get the cursor position on the screen
                        // Parameters:
                        //             int *x
                        //             int *y
                        // Returns:    bool cnd
                        //               true: x,y pointers are filled
                        //               false: no disassembly window open
                        // Note: coordinates are 0-based

  ui_get_curline,       // * Get current line from the disassemble window
                        // Parameters: none
                        // Returns:    cptr current line with the color codes
                        // (use tag_remove function to remove the color codes)

  ui_get_hwnd,          // * Get HWND of the main IDA window
                        // Parameters: none
                        // Returns:    txt version: NULL
                        //             gui version: HWND
                        //             qt version under windows: HWND
                        // HWND is returned in result.vptr

  ui_copywarn,          // * Display copyright warning
                        // Parameters: none
                        // Returns:    bool yes/no

  ui_getvcl,            // * Get VCL variables
                        // Parameters:
                        //              TApplication **app
                        //              TScreen **screen
                        //              TMouse **mouse
                        // Returns: int sizeof(TApplication)+sizeof(TScreen)+sizeof(TMouse)
                        // The text version fills the pointers with NULLs and returns 0

  ui_idp_event,         // cb: A processor module event has been generated (idp.hpp, idp_notify)
                        // Parameteres:
                        //      ph::idp_notify event_code
                        //      va_list va
                        // Returns:
                        //      int code; code==0 - process the event
                        //                otherwise return code as the result
                        // This event should not be used as a parameter to callui()
                        // The kernel uses it to notify the ui about the events

  ui_lock_range_refresh,// * Lock the ui_range refreshes. The ranges will not
                        // be refreshed until the corresponding unlock_range_refresh
                        // is issued.
                        // See also: unlock_range_refresh
                        // Parameters: none
                        // Returns:    none

  ui_unlock_range_refresh,// * Unlock the ui_range refreshes. If the number of locks
                        // is back to zero, then refresh the ranges.
                        // See also: ui_range
                        // Parameters: none
                        // Returns:    none

  ui_setbreak,          // * set ctrl-break flag
                        // Parameters: none
                        // Returns: none

  ui_genfile_callback,  // cb: handle html generation
                        // parameters: html_header_cb_t **,
                        //             html_footer_cb_t **,
                        //             html_line_cb_t **
                        // returns: nothing

  ui_open_url,          // * open url
                        // Parameters: const char *url
                        // Returns: none

  ui_hexdumpea,         // * Return the current address in a hex view
                        // Parameters: ea_t *result
                        //             int hexdump_num
                        // Returns:    none

  ui_set_xml,           // * set/update one or more XML values. The 'name' element
                        // or attribute (use @XXX for an attribute) is created in
                        // all XML elements returned by the evaluation of the
                        // 'path' XPath expression, and receives the given 'value'.
                        // If 'name' is empty, the returned elements or attributes
                        // are directly updated to contain the new 'value'.
                        // Parameters: const char *path
                        //             const char *name
                        //             const char *value
                        // Returns:    bool
                        // NOTE: obsolete, not supported anymore

  ui_get_xml,           // * return an XML value by evaluating the 'path' XPath
                        // expression.
                        // Parameters: const char *path
                        //             idc_value_t *value
                        // Returns:    bool
                        // NOTE: obsolete, not supported anymore

  ui_del_xml,           // * delete XML values corresponding to the evaluation of the
                        // 'path' XPath expression.
                        // Parameters: const char *path
                        // Returns:    bool
                        // NOTE: obsolete, not supported anymore

  ui_push_xml,          // * push an XML element on a stack whose uppermost element will be
                        // used to evaluate future relative XPath expressions.
                        // Parameters: const char *path
                        // Returns:    bool
                        // NOTE: obsolete, not supported anymore

  ui_pop_xml,           // * pop the uppermost XML element from the stack.
                        // Parameters: none
                        // Returns:    bool
                        // NOTE: obsolete, not supported anymore

  ui_get_key_code,      // * get keyboard key code by its name
                        // Parameters: const char *keyname
                        // Returns:    short code

  ui_setup_plugins_menu,// * setup plugins submenu
                        // Parameters: none
                        // Returns:    none

  ui_refresh_navband,   // * refresh navigation band if changed
                        // Parameters: bool force
                        // Returns:    none

  ui_new_custom_viewer, // * create new ida viewer based on place_t (gui)
                        // Parameters:
                        //      const char *title
                        //      TWinControl *parent
                        //      const place_t *minplace
                        //      const place_t *maxplace
                        //      const place_t *curplace
                        //      int y
                        //      void *ud
                        // returns: TCustomControl *
                        // see also: ui_new_ea_viewer

  ui_add_menu_item,     // * add a menu item
                        // Parameters: const char *menupath,
                        //             const char *name,
                        //             const char *hotkey,
                        //             int flags,
#define SETMENU_POSMASK     0x1
#define SETMENU_INS         0x0         // add menu item before the specified path (default)
#define SETMENU_APP         0x1         // add menu item after the specified path
#define SETMENU_CTXMASK     0x7C000000  // context flags for the menu item
#define SETMENU_CTXAPP      0x00000000  // global (default)
#define SETMENU_CTXIDA      0x04000000  // disassembly
#define SETMENU_CTXSTR      0x08000000  // structures
#define SETMENU_CTXENUM     0x0C000000  // enumerations
#define SETMENU_CTXEA       0x10000000  // ea views (disassembly, hex)
#define SETMENU_CTXVIEW     0x14000000  // any kind of view (disassembly, hex, structures, etc.)
                        //             menu_item_callback_t *callback,
                        //             void *ud
                        // Returns:    bool

  ui_del_menu_item,     // * del a menu item
                        // Parameters: const char *menupath
                        // Returns:    bool

  ui_debugger_menu_change, // cb: debugger menu modification detected
                        // Parameters: bool enable
                        // enable=true: debugger menu has been added
                        // enable=false: debugger menu will be removed

  ui_get_curplace,      // * Get current place in a custom viewer
                        // Parameters:
                        // TCustomControl *v
                        // bool mouse_position (otherwise keyboard position)
                        // int *x
                        // int *y
                        // returns: place_t *

  ui_create_tform,      // * create a new tform (only gui version)
                        // Parameters: const char *caption
                        //             HWND *handle
                        // If you need the handle of the new window,
                        // you can get it using the 'handle' parameter.
                        // You will need it if you do not use VCL.
                        // If a window with the specified caption exists, return
                        // a pointer to it. 'handle' will be NULL is this case.
                        // Returns: TForm * of a new or existing window
                        // The text version always returns NULL
                        // NB: Do not use 'handle' to populate the window
                        // because it can be destroyed by the user interface
                        // at any time. Also, the handle is invalid at the
                        // form creation time. It is present only for
                        // the compatibility reasons.
                        // Hook to ui_tform_visible event instead.

  ui_open_tform,        // * open tform (only gui version)
                        // Parameters: TForm *form
                        //             int options
//#define FORM_MDI      0x01 // start by default as MDI
#define FORM_TAB      0x02 // attached by default to a tab
#define FORM_RESTORE  0x04 // restore state from desktop config
#define FORM_ONTOP    0x08 // form should be "ontop"
#define FORM_MENU     0x10 // form must be listed in the windows menu
                           // (automatically set for all plugins)
#define FORM_CENTERED 0x20 // form will be centered on the screen
                        // Returns: nothing
#define FORM_PERSIST  0x40 // form will persist until explicitly closed with close_tform()
                        // Returns: nothing
#define FORM_QWIDGET  0x80 // windows: use QWidget* instead of HWND in ui_tform_visible
                           // this flag is ignored in unix because we never use HWND there.
                           // around 2011/09 we plan to get rid of HWND and always use QWidget*
                           // regardless of this flag.
#define FORM_CLOSED_BY_ESC     0x100 // override idagui.cfg:CLOSED_BY_ESC: esc will close
#define FORM_NOT_CLOSED_BY_ESC 0x200 // override idagui.cfg:CLOSED_BY_ESC: esc will not close

  ui_close_tform,       // * close tform (only gui version)
                        // Parameters: TForm *form
                        //             int options
#define FORM_SAVE           0x1 // save state in desktop config
#define FORM_NO_CONTEXT     0x2 // don't change the current context (useful for toolbars)
#define FORM_DONT_SAVE_SIZE 0x4 // don't save size of the window
#define FORM_CLOSE_LATER    0x8 // assign the deletion of the form to the UI loop
                        // Returns: nothing

  ui_switchto_tform,    // * activate tform (only gui version)
                        // Parameters: TForm *form
                        //             bool take_focus
                        // Returns: nothing

  ui_find_tform,        // * find tform with the specified caption  (only gui version)
                        // Parameters: const char *caption
                        // Returns: TForm *
                        // NB: this callback works only with the tabbed forms!

  ui_get_current_tform, // * get current tform (only gui version)
                        // Parameters: none
                        // Returns: TForm *
                        // NB: this callback works only with the tabbed forms!

  ui_get_tform_handle,  // * get tform handle
                        // Parameters: TForm *form
                        // Returns: HWND
                        // tform handles can be modified by the interface
                        // (for example, when the user switch from mdi to desktop)
                        // This function returns the current tform handle.
                        // It is better to hook to the 'ui_tform_visible'
                        // event and populate the window with controls at
                        // that time.
                        // NOTE: obsolete, not supported

  ui_tform_visible,     // tform is displayed on the screen
                        // Use this event to populate the window with controls
                        // Parameters: TForm *form
                        //             HWND hwnd or QWidget* widget
                        // In unix, always work with QWigdet*
                        // In windows, use HWND if FORM_QWIDGET was not specified in open_tform()
                        // Returns: nothing

  ui_tform_invisible,   // tform is being closed
                        // Use this event to destroy the window controls
                        // Parameters: TForm *form
                        //             HWND hwnd or QWidget* widget
                        // See comment for ui_tform_visible
                        // Returns: nothing

  ui_get_ea_hint,       // cb: ui wants to display a simple hint for an address
                        // Use this event to generate a custom hint
                        // Parameters: ea_t ea
                        //             char *buf
                        //             size_t bufsize
                        // Returns: bool: true if generated a hint
                        // See also more generic ui_get_item_hint

  ui_get_item_hint,     // cb: ui wants to display multiline hint for an item
                        // Parameters: ea_t ea (or item id like a structure or enum member)
                        //             int max_lines -- maximal number of lines
                        //             int *important_lines  -- out: number of important lines
                        //                                           if zero, output is ignored
                        //             qstring *hint  -- the output string
                        // Returns: bool: true if generated a hint
                        // See also more generic ui_get_custom_viewer_hint

  ui_set_nav_colorizer, // * setup navigation band color calculator (gui)
                        // Parameters: nav_colorizer_t *func
                        // Returns: vptr: pointer to old colorizer

  ui_refresh_custom_viewer, // * refresh custom ida viewer
                        // Parameters:
                        // TCustomControl *custom_viewer
                        // returns: nothing

  ui_destroy_custom_viewer, // * destroy custom ida viewer
                        // Parameters:
                        // TCustomControl *custom_viewer
                        // returns: nothing

  ui_jump_in_custom_viewer, // * set cursor position in custom ida viewer
                        // Parameters:
                        // TCustomControl *custom_viewer
                        // place_t *new_position
                        // int x
                        // int y
                        // returns: bool success

  ui_set_custom_viewer_popup, // * clear custom viewer popup menu
                        // TCustomControl *custom_viewer
                        // TPopupMenu *popup (NULL-clear menu)
                        // returns: nothing

  ui_add_custom_viewer_popup, // * add custom viewer popup menu item
                        // TCustomControl *custom_viewer
                        // const char *title
                        // const char *hotkey
                        // menu_item_callback_t *cb
                        // void *ud
                        // returns: nothing

  ui_set_custom_viewer_handlers,
                        // * set handlers for custom viewer events
                        // TCustomControl *custom_viewer
                        // custom_viewer_keydown_t *keyboard_handler
                        // custom_viewer_popup_t *popup_handler
                        // custom_viewer_dblclick_t *dblclick_handler
                        // custom_viewer_curpos_t *curpos_handler
                        // custom_viewer_close_t *close_handler
                        // void *user_data
                        // Any of these handlers may be NULL
                        // returns: nothing
                        // see also: ui_set_custom_viewer_handler

  ui_get_custom_viewer_curline,
                        // * get custom viewer current line
                        // TCustomControl *custom_viewer
                        // bool mouse (current for mouse pointer?)
                        // returns: cptr: const char * or NULL
                        // The returned line is with color codes

  ui_get_current_viewer,// * get current ida viewer (idaview or custom viewer)
                        // returns: TCustomControl *viewer

  ui_is_idaview,        // * is idaview viewer? (otherwise-custom viewer)
                        // TCustomControl *custom_viewer
                        // returns: bool

  ui_get_custom_viewer_hint,
                        // cb: ui wants to display a hint for a viewer (idaview or custom)
                        // TCustomControl *viewer - viewer
                        // place_t *place         - current position in it
                        // int *important_lines  -- out: number of important lines
                        //                               if zero, the result is ignored
                        // qstring *hint -- the output string
                        // Returns: bool: true if generated a hint

  ui_readsel2,          // * Get the selected area boundaries
                        // Parameters:
                        //      TCustomControl *custom_viewer
                        //      twinpos_t *start
                        //      twinpos_t *end
                        // Returns: bool
                        //          0 - no area is selected
                        //          1 - ok, start and end are filled
                        // This is more complex version of ui_readsel.
                        // If you see only the addresses, use ui_readsel.

  ui_set_custom_viewer_range,
                        // * set position range for custom viewer
                        // Parameters:
                        //      TCustomControl *custom_viewer
                        //      const place_t *minplace
                        //      const place_t *maxplace
                        // returns: nothing

  ui_database_inited,   // cb: database initialization has completed
                        // the kernel is about to run idc scripts
                        // Parameters: int is_new_database
                        //             const char *idc_script (maybe NULL)
                        // Returns:    none

  ui_ready_to_run,      // cb: all UI elements have been initialized.
                        // Automatic plugins may hook to this event to
                        // perform their tasks.
                        // Parameters: none
                        // Returns: nothing

  ui_set_custom_viewer_handler,
                        // * set a handler for a custom viewer event
                        // TCustomControl *custom_viewer
                        // custom_viewer_handler_id_t handler_id
                        // void *handler_or_data
                        // returns: old value of the handler or data
                        // see also: ui_set_custom_viewer_handlers

  ui_refresh_chooser,   // * Mark a non-modal custom chooser for a refresh
                        // Parameters:
                        //      const char *title
                        // Returns: bool success

  ui_add_chooser_cmd,   // * add a menu item to a chooser window
                        // const char *chooser_caption
                        // const char *cmd_caption
                        // chooser_cb_t *chooser_cb
                        // int menu_index
                        // int icon
                        // int flags
                        // const char *hotkey (only when CHOOSER_HOTKEY is specified)
                        // Returns: bool success

  ui_open_builtin,      // * open a window of a built-in type
                        // int window_type (one of BWN_... constants)
                        // additional params depend on the window type
                        // see below for the inline convenience functions
                        // Returns: TForm * window pointer

  ui_preprocess,        // cb: ida ui is about to handle a user command
                        // const char *name: ui command name
                        //   these names can be looked up in ida[tg]ui.cfg
                        // returns: int 0-ok, nonzero: a plugin has handled the command

  ui_postprocess,       // cb: an ida ui command has been handled

  ui_set_custom_viewer_mode,
                        // * switch between graph/text modes
                        // TCustomControl *custom_viewer
                        // bool graph_view
                        // bool silent
                        // Returns: bool success

  ui_gen_disasm_text,   // * generate disassembly text for a range
                        // ea_t ea1
                        // ea_t ea2
                        // text_t *text
                        // bool truncate_lines (on inf.margin)
                        // returns: nothing, appends lines to 'text'

  ui_gen_idanode_text,  // cb: generate disassembly text for a node
                        // qflow_chart_t *fc
                        // int node
                        // text_t *text
                        // Plugins may intercept this event and provide
                        // custom text for an IDA graph node
                        // They may use gen_disasm_text() for that.
                        // Returns: bool text_has_been_generated

  ui_install_cli,       // * install command line interpreter
                        // cli_t *cp,
                        // bool install
                        // Returns: nothing

  ui_execute_sync,      // * execute code in the main thread
                        // exec_request_t *req
                        // Returns: int code

  ui_enable_input_hotkeys,
                        // * enable or disable alphanumeric hotkeys
                        //   which can interfere with user input
                        // bool enable
                        // Returns: bool new_state
                        // NOTE: obsolete, not supported

  ui_get_chooser_obj,
                        // * get underlying object of the specified chooser
                        // const char *chooser_caption
                        // Returns: void *chooser_object

  ui_enable_chooser_item_attrs,
                        // * enable item-specific attributes for a chooser
                        // const char *chooser_caption
                        // bool enable
                        // Returns: success

  ui_get_chooser_item_attrs,
                        // cb: get item-specific attributes for a chooser
                        // void *chooser_object
                        // uint32 n
                        // chooser_item_attrs_t *attrs
                        // Returns: nothing
                        // This callback is generated only after enable_chooser_attrs()

  ui_set_dock_pos,      // * sets the docking position of a form
                        // const char *src_form
                        // const char *dest_form
                        // const int orientation (one of DP_XXXX flags)
                        // const int left, top, right, bottom
                        // Returns: boolean
#define DP_LEFT            0x0001
#define DP_TOP             0x0002
#define DP_RIGHT           0x0004
#define DP_BOTTOM          0x0008
#define DP_INSIDE          0x0010
#define DP_BEFORE          0x0020
#define DP_RAW             0x0040
#define DP_FLOATING        0x0080

  ui_get_opnum,         // * get current operand number
                        // Returns int operand number. -1 means no operand

  ui_install_custom_datatype_menu,
                        // * install/remove custom data type menu item
                        // int dtid - data type id
                        // bool install
                        // Returns: success

  ui_install_custom_optype_menu,
                        // * install/remove custom operand type menu item
                        // int fid - format id
                        // bool install
                        // Returns: success

  ui_get_range_marker,  // * Get pointer to function
                        // "void mark_range_for_refresh(ea_t ea, asize_t size)"
                        // This function will be called by the kernel when the
                        // database is changed
                        // Parameters: none
                        // Returns: vptr: (idaapi*marker)(ea_t ea, asize_t) or NULL

  ui_get_highlighted_identifier,
                        // * Returns the highlighted identifier in the current IDAView
                        // char *buf - buffer to copy identifier to
                        // size_t bufsize - buffer size
                        // int flags - currently not used (pass 0)
                        // Returns: bool (false if no identifier is highlighted)

  ui_lookup_key_code,   // * Get shortcut code previously created by get_key_code
                        // Parameters: int key
                        //             int shift
                        //             bool is_qt
                        // Returns:    short code

  ui_load_custom_icon_file,
                        // * Loads an icon and returns its id
                        // Parameters: const char *file_name
                        // Returns: int

  ui_load_custom_icon,  // * Loads an icon and returns its id
                        // Parameters: const void *ptr
                        //             uint len
                        //             const char *format
                        // Returns: int

  ui_free_custom_icon,  // * Frees an icon loaded with ui_load_custom_icon(_file)
                        // Parameters: int icon_id

  ui_process_action,    // * Processes a UI action by name
                        // Parameters: const char *name
                        //             int flags (reserved / not used)
                        //             void *param (reserved / not used)

  ui_new_code_viewer,   // * Create a code viewer
                        // Parameters: TWinControl *parent
                        //             TCustomControl *custview
                        //             int flags (combination of CDVF_* flags)
                        // returns: TCustomControl *

#define CDVF_NOLINES        0x0001    // don't show line numbers
#define CDVF_LINEICONS      0x0002    // icons can be drawn over the line control
#define CDVF_STATUSBAR      0x0004    // keep the status bar in the custom viewer

  ui_addons,            // * work with registered add-ons

  ui_execute_ui_requests,
                        // * Execute a variable number of UI requests.
                        // (the UI requests will be dispatched in the context of the main thread)
                        // Parameters: ui_request_t *req (the first request)
                        //             ... (variable arg ui_request_t *)
                        //             NULL (to terminate the var arg request list)

  ui_execute_ui_requests_list,
                        // * Execute a list of UI requests
                        // (the UI requests will be dispatched in the context of the main thread)
                        // Parameters: ui_requests_t *

  ui_register_timer,    // * Register a timer
                        // Timer functions are thread-safe and the callback is executed
                        // in the context of the main thread.
                        // Parameters: int interval (in milliseconds)
                        //             int (idaapi *callback)(void *ud)
                        //                 (the callback can return -1 to unregister the timer;
                        //                  any other value >= 0 defines the new interval for the timer)
                        //             void *ud
                        // Returns: qtimer_t (use this handle to unregister the timer)

  ui_unregister_timer,  // * Unregister a timer
                        // Parameters: qtimer_t t (handle to a registered timer)

  ui_take_database_snapshot,
                        // * Take a database snapshot
                        // Parameters: snapshot_t *ss - in/out parameter.
                        //                            - in: description, flags
                        //                            - out: filename, id
                        //             qstring *err_msg - optional error msg buffer
                        // Returns: bool

  ui_restore_database_snapshot,
                        // * Restore a database snapshot
                        // Parameters: const snapshot_t *ss - snapshot instance (see build_snapshot_tree())
                        //             ss_restore_cb_t cb - A callback that will be triggered with a NULL string
                        //             on success and an actual error message on failure.
                        //             void *ud - user data passed to be passed to the callback
                        // Note: This call is asynchronous. When it is completed, the callback will be triggered.
                        // Returns: boolean. False if restoration could not be started (snapshot file was not found).
                        //          If the returned value is True then check if the operation succeeded from the callback.

  ui_set_code_viewer_line_handlers,
                        // * Set handlers for code viewer line events
                        // Parameters: TCustomControl *code_viewer
                        //             code_viewer_lines_click_t *click_handler
                        //             code_viewer_lines_click_t *popup_handler
                        //             code_viewer_lines_click_t *dblclick_handler
                        //             code_viewer_lines_icon_t *drawicon_handler
                        //             code_viewer_lines_linenum_t *linenum_handler
                        //             Any of these handlers may be NULL
                        // Returns: nothing

  ui_refresh_custom_code_viewer,
                        // * Refresh custom code viewer
                        // Parameters: TCustomControl *code_viewer
                        // Returns: nothing

  ui_new_source_viewer, // * Create new source viewer
                        // Parameters: TWinControl *parent
                        //             TCustomControl *custview
                        //             const char *path
                        //             strvec_t *lines
                        //             int lnnum
                        //             int colnum
                        //             itn flags (SVF_... bits)
                        // Returns: source_view_t *

#define SVF_COPY_LINES  0x0000   // keep a local copy of '*lines'
#define SVF_LINES_BYPTR 0x0001   // remeber the 'lines' ptr. do not make a copy of '*lines'

  ui_get_tab_size,      // * Get the size of a tab in spaces
                        // Parameters: const char *path
                        //             (The path of the source view for which the tab size is requested.
                        //              If NULL, the default size is returned.)
                        // Returns: int

  ui_set_menu_item_icon,// * Set the icon of a menu item
                        // Parameters: const char *item_name
                        //             int icon_id
                        // Returns: bool

  ui_repaint_qwidget,   // * Repaint the widget immediately
                        // Parameters: QWidget *widget
                        // Returns: nothing

  ui_enable_menu_item,  // * Enable or disable a menu item
                        // Parameters: const char *item_name
                        //             bool enable
                        // Returns: bool

  ui_custom_viewer_set_userdata,
                        // * Change place_t user data for a custom view
                        // Parameters: TCustomControl *custom_viewer
                        //             void *user_data
                        // Returns: old user_data

  ui_obsolete_new_ea_viewer,

  ui_jumpto,            // * Jump to the specified address
                        // Parameters:
                        //      ea_t ea
                        //      int operand_num (-1: don't change x coord)
                        //      int uijmp_flags
                        // Returns: bool success

  ui_choose_info,       // * Invoke the chooser with a choose_info_t structure
                        // Parameters:
                        //      chooser_info_t *chi
                        // Returns: see the choose function

  ui_cancel_exec_request,
                        // * Cancel a queued exec request
                        // Parameters:
                        //      int req_id - request id
                        // Returns: success

  ui_show_form,         // * Show a dockable modeless dialog form
                        // Parameters:
                        //      const char *format
                        //      int flags
                        //      va_list va
                        // Returns: *TForm

  ui_unrecognized_config_directive,
                        // * Possibly handle an extra config directive,
                        //   passed through '-d' or '-D'.
                        // Parameters:
                        //      const char *directive - The config directive
                        // Returns: char *; one of the IDPOPT_OK, or IDPOPT_BAD* values.

  ui_add_chooser_menu_cb,
                        // Add callback to add new actions to the chooser popup menu
                        // Parameters:
                        //      TChooser *chooser - pointer to the chooser
                        //      prepare_popup_cmds_t *prepare_popup_cmds -
                        //        pointer to the callback function
                        // Returns: nothing

  ui_get_viewer_name,   // Returns name of the custom or ida viewer
                        // Parameters:
                        //      TCustomViewer *viewer - pointer to custom viewer
                        //      char *buf - output buffer
                        //      size_t bufsize - size of buffer
                        //  Returns: nothing

  ui_get_output_cursor, // Returns coordinates of the output window's cursor
                        // Parameters:
                        //    int *x - column
                        //    int *y - line number (global, from the start of output)
                        // Returns: nothing
                        // Note: coordinates are 0-based

  ui_get_output_curline,
                        // Get current line of output window
                        // Parameters:
                        //    char *buf - output buffer
                        //    size_t bufsize - size of buffer
                        //    bool mouse (current for mouse pointer?)
                        // Returns: bool: false if output contains no text

  ui_get_output_selected_text,
                        // Returns selected text from output window
                        // Parameters:
                        //    char *buf - output buffer
                        //    size_t bufsize - size of buffer
                        //  Returns: bool: true if there is a selection

  ui_add_output_popup,  // Adds new menu item to output popup menu.
                        // This function must be called after popup notification (msg_popup)
                        // is received, because the list of user items is cleaned every
                        // time after menu execution
                        // Parameters:
                        //    char *title - title of the menu item
                        //    menu_item_callback_t *cb - pointer to the callback function
                        //    void *ud - user data provided for the callback
                        // Returns: nothing

  ui_get_tform_idaview, // Get the TCustomControl* that is associated with this TForm*.
                        // Parameters:
                        //    TForm* - the tform
                        // Returns: the TCustomControl* that is present in this TForm*,
                        //          or NULL if that TForm doesn't host an IDA view.

  ui_get_renderer_type, // Get the type of renderer that is currently in use in the view.
                        // Parameters:
                        //    TCustomControl* - The view.
                        // Returns: tcc_renderer_type_t the renderer type.

  ui_set_renderer_type, // Set the type of renderer to use in the view.
                        // Parameters:
                        //    TCustomControl* - The view.
                        //    tcc_renderer_type_t - the renderer type.
                        // Returns: nothing

  ui_askfile2,          // * Ask the user a file name
                        // Parameters:
                        //      bool forsave
                        //      const char *default_dir
                        //      const char *filter
                        //      const char *format
                        //      va_list va
                        // Returns: file name

  ui_get_viewer_user_data,
                        // * Get the view's user data.
                        // Parameters:
                        //      TCustomControl *view
                        // Returns: void * - The user data.

  ui_get_viewer_place_type,
                        // * Get the type of place_t instances a viewer uses & creates.
                        // Parameters:
                        //      TCustomControl *view - The view that provided the place.
                        // Returns: tcc_place_type_t - The type of place

  ui_new_ea_viewer,     // * Create new ea viewer based on place_t (gui)
                        // Parameters:
                        //      const char *title
                        //      TWinControl *parent
                        //      const place_t *minplace
                        //      const place_t *maxplace
                        //      const place_t *curplace
                        //      int y
                        //      void *ud
                        //      int flags
                        //      fillloc_t *fillloc
                        //      jumploc_t *jumploc
                        // returns: TCustomControl *
                        // see also: ui_new_custom_viewer

  ui_ea_viewer_history_push_and_jump,
                        // * Push current location in the history, and jump
                        // this ea viewer (and potentially synchronized others)
                        // to the given location.
                        // Parameters:
                        //      TCustomControl *ea_viewer
                        //      ea_t ea
                        //      int x
                        //      int y
                        //      int lnnum

  ui_ea_viewer_history_info,
                        // * Get information about what's in the history
                        // Parameters:
                        //      int *out_nback - Number of available back steps
                        //      int *out_nfwd - Number of available forward steps
                        //      TCustomControl *ea_viewer

  ui_last,              // The last notification code


  // debugger callgates. should not be used directly, see dbg.hpp for details

  ui_dbg_begin = 1000,
  ui_dbg_run_requests = ui_dbg_begin,
  ui_dbg_get_running_request,
  ui_dbg_get_running_notification,
  ui_dbg_clear_requests_queue,
  ui_dbg_get_process_state,
  ui_dbg_start_process,
  ui_dbg_request_start_process,
  ui_dbg_suspend_process,
  ui_dbg_request_suspend_process,
  ui_dbg_continue_process,
  ui_dbg_request_continue_process,
  ui_dbg_exit_process,
  ui_dbg_request_exit_process,
  ui_dbg_get_thread_qty,
  ui_dbg_getn_thread,
  ui_dbg_select_thread,
  ui_dbg_request_select_thread,
  ui_dbg_step_into,
  ui_dbg_request_step_into,
  ui_dbg_step_over,
  ui_dbg_request_step_over,
  ui_dbg_run_to,
  ui_dbg_request_run_to,
  ui_dbg_step_until_ret,
  ui_dbg_request_step_until_ret,
  ui_dbg_get_oldreg_val,                // obsolete
  ui_dbg_set_oldreg_val,                // obsolete
  ui_dbg_request_set_oldreg_val,        // obsolete
  ui_dbg_get_bpt_qty,
  ui_dbg_getn_oldbpt,                   // obsolete
  ui_dbg_get_oldbpt,                    // obsolete
  ui_dbg_add_oldbpt,                    // obsolete
  ui_dbg_request_add_oldbpt,            // obsolete
  ui_dbg_del_oldbpt,                    // obsolete
  ui_dbg_request_del_oldbpt,            // obsolete
  ui_dbg_update_oldbpt,                 // obsolete
  ui_dbg_enable_oldbpt,                 // obsolete
  ui_dbg_request_enable_oldbpt,         // obsolete
  ui_dbg_set_trace_size,
  ui_dbg_clear_trace,
  ui_dbg_request_clear_trace,
  ui_dbg_is_step_trace_enabled,
  ui_dbg_enable_step_trace,
  ui_dbg_request_enable_step_trace,
  ui_dbg_get_step_trace_options,
  ui_dbg_set_step_trace_options,
  ui_dbg_request_set_step_trace_options,
  ui_dbg_is_insn_trace_enabled,
  ui_dbg_enable_insn_trace,
  ui_dbg_request_enable_insn_trace,
  ui_dbg_get_insn_trace_options,
  ui_dbg_set_insn_trace_options,
  ui_dbg_request_set_insn_trace_options,
  ui_dbg_is_func_trace_enabled,
  ui_dbg_enable_func_trace,
  ui_dbg_request_enable_func_trace,
  ui_dbg_get_func_trace_options,
  ui_dbg_set_func_trace_options,
  ui_dbg_request_set_func_trace_options,
  ui_dbg_get_tev_qty,
  ui_dbg_get_tev_info,
  ui_dbg_get_insn_tev_oldreg_val,       // obsolete
  ui_dbg_get_insn_tev_oldreg_result,    // obsolete
  ui_dbg_get_call_tev_callee,
  ui_dbg_get_ret_tev_return,
  ui_dbg_get_bpt_tev_ea,
  ui_dbg_get_reg_value_type,
  ui_dbg_get_process_qty,
  ui_dbg_get_process_info,
  ui_dbg_attach_process,
  ui_dbg_request_attach_process,
  ui_dbg_detach_process,
  ui_dbg_request_detach_process,
  ui_dbg_get_first_module,
  ui_dbg_get_next_module,
  ui_dbg_bring_to_front,
  ui_dbg_get_current_thread,
  ui_dbg_wait_for_next_event,
  ui_dbg_get_debug_event,
  ui_dbg_set_debugger_options,
  ui_dbg_set_remote_debugger,
  ui_dbg_load_debugger,
  ui_dbg_retrieve_exceptions,
  ui_dbg_store_exceptions,
  ui_dbg_define_exception,
  ui_dbg_suspend_thread,
  ui_dbg_request_suspend_thread,
  ui_dbg_resume_thread,
  ui_dbg_request_resume_thread,
  ui_dbg_get_process_options,
  ui_dbg_check_bpt,
  ui_dbg_set_process_state,
  ui_dbg_get_manual_regions,
  ui_dbg_set_manual_regions,
  ui_dbg_enable_manual_regions,
  ui_dbg_set_process_options,
  ui_dbg_is_busy,
  ui_dbg_hide_all_bpts,
  ui_dbg_edit_manual_regions,
  ui_dbg_get_reg_val,
  ui_dbg_set_reg_val,
  ui_dbg_request_set_reg_val,
  ui_dbg_get_insn_tev_reg_val,
  ui_dbg_get_insn_tev_reg_result,
  ui_dbg_register_provider,
  ui_dbg_unregister_provider,
  ui_dbg_handle_debug_event,
  ui_dbg_add_vmod,
  ui_dbg_del_vmod,
  ui_dbg_compare_bpt_locs,
  ui_dbg_save_bpts,
  ui_dbg_old_getn_bpt,              // obsolete
  ui_dbg_old_get_bpt,               // obsolete
  ui_dbg_old_update_bpt,            // obsolete
  ui_dbg_set_bptloc_string,
  ui_dbg_get_bptloc_string,
  ui_dbg_internal_appcall,
  ui_dbg_internal_cleanup_appcall,
  ui_dbg_internal_get_sreg_base,
  ui_dbg_internal_ioctl,
  ui_dbg_read_memory,
  ui_dbg_write_memory,
  ui_dbg_read_registers,
  ui_dbg_write_register,
  ui_dbg_get_memory_info,
  ui_dbg_get_event_cond,
  ui_dbg_set_event_cond,
  ui_dbg_old_find_bpt,              // obsolete
  ui_dbg_enable_bpt,
  ui_dbg_request_enable_bpt,
  ui_dbg_old_add_bpt,               // obsolete
  ui_dbg_old_request_add_bpt,       // obsolete
  ui_dbg_del_bpt,
  ui_dbg_request_del_bpt,
  ui_dbg_map_source_path,
  ui_dbg_map_source_file_path,
  ui_dbg_modify_source_paths,
  ui_dbg_is_bblk_trace_enabled,
  ui_dbg_enable_bblk_trace,
  ui_dbg_request_enable_bblk_trace,
  ui_dbg_get_bblk_trace_options,
  ui_dbg_set_bblk_trace_options,
  ui_dbg_request_set_bblk_trace_options,
  // trace management
  ui_dbg_load_trace_file,
  ui_dbg_save_trace_file,
  ui_dbg_is_valid_trace_file,
  ui_dbg_set_trace_file_desc,
  ui_dbg_get_trace_file_desc,
  ui_dbg_choose_trace_file,
  ui_dbg_diff_trace_file,
  ui_dbg_graph_trace,
  ui_dbg_get_tev_memory_info,
  ui_dbg_get_tev_event,
  ui_dbg_get_insn_tev_reg_mem,
  // breakpoint management (new codes were introduced in v6.3)
  ui_dbg_getn_bpt,
  ui_dbg_get_bpt,
  ui_dbg_find_bpt,
  ui_dbg_add_bpt,
  ui_dbg_request_add_bpt,
  ui_dbg_update_bpt,
  ui_dbg_for_all_bpts,
  ui_dbg_get_tev_ea,
  ui_dbg_get_tev_type,
  ui_dbg_get_tev_tid,
  ui_dbg_get_tev_reg_val,
  ui_dbg_get_tev_reg_mem_qty,
  ui_dbg_get_tev_reg_mem_ea,
  ui_dbg_get_trace_base_address,
  // calluis for creating traces from scratch (added in 6.4)
  ui_dbg_set_trace_base_address,
  ui_dbg_add_tev,
  ui_dbg_add_insn_tev,
  ui_dbg_add_call_tev,
  ui_dbg_add_ret_tev,
  ui_dbg_add_bpt_tev,
  ui_dbg_add_debug_event,
  ui_dbg_add_thread,
  ui_dbg_del_thread,
  ui_dbg_add_many_tevs,
  ui_dbg_map_binary_path,
  ui_dbg_modify_binary_paths,
  ui_dbg_move_bpt_to_grp,
  ui_dbg_set_highlight_trace_options,
  ui_dbg_set_trace_platform,
  ui_dbg_get_trace_platform,
  // added in 6.6
  ui_dbg_internal_get_elang,
  ui_dbg_internal_set_elang,

  ui_dbg_end

  // Debugging notifications
#ifdef _DEBUG
  ,

  // When execute_sync() is called from another thread,
  // make sure that thread is in an appropriate state
  // for a qsem_wait().
  //
  // IDAPython, for example, will listen to this notification, and assert that
  // the global interpreter lock does not belong to the currently-running
  // thread anymore. Otherwise, what might happen is that we have:
  //   main thread           -> sem_wait(interpreter_lock)
  //   python-created thread -> sem_wait(request.semaphore)
  // I.e., both threads are in deadlock, waiting on something
  // that will never change.
  debug_assert_thread_waitready = ui_dbg_end
#endif
};





