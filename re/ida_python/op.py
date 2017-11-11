#raw, trans, parse, restore
from idautils import *
from idc import *
from struct import *
from collections import OrderedDict

import util

_attrsRaw=OrderedDict([
    #'n',
    ('type','B'),
    #'offb',
    #'offo',
    ('flags','B'),
    ('dtyp', 'B'),		
    ('reg','H'),
    ('value','Q'), 		
    ('addr','Q'),		
    ('specval','Q'),	
    ('specflag1','B'),	
    ('specflag2','B'),
    ('specflag3','B'),
    ('specflag4','B')
])

#output number;
def raw(ea, ix=0):
	if ea is None:
		ea=ScreenEA()
	print '-------------------------------------'
	print ea, ':', 
	print(GetOpnd(ea, ix))
	op=DecodeInstruction(ea).Operands[ix]
	for key, fmt in _attrsRaw.iteritems():
		val=getattr(op, key)
	#	print 'val', val
	#	try:
	#		if val<0:
	#			fmt2=fmt.lower()
	#		else:
	#			fmt2=fmt
	#		val=unpack('@'+fmt, pack('@'+fmt2, val))[0]
	#	except struct.error:
	#		print 'errrrrrrrrr:struct.error'
		print('{0:<15}={1}'.format(key, val))


############################################################################################
_typeList=(
  'none', 		#'o_void',     #=  0, // No Operand                           ----------
  'reg', 		#'o_reg',      #=  1, // General Register (al,ax,es,ds...)    reg
  'moff',		# #'o_mem',    #=  2, // Direct Memory Reference/moffset/(bare displacement); (DATA)      addr
  'sib', 		#'o_phrase',   #=  3, // Memory Ref [Base Reg + Index Reg]    phrase
  'sib+dis',	#'o_displ',    #=  4, // Memory Reg [Base Reg + Index Reg + Displacement] phrase+addr
  'imm', 		#'o_imm',      #=  5, // Immediate Value                      value
  'far', 		#'o_far',      #=  6, // Immediate Far Address  (CODE)        addr
  'near', 		#'o_near',     #=  7, // Immediate Near Address (CODE)        addr

  'o_idpspec0', #=  8, // IDP specific type
  'o_idpspec1', #=  9, // IDP specific type
  'o_idpspec2', #= 10, // IDP specific type
  'o_idpspec3', #= 11, // IDP specific type
  'o_idpspec4', #= 12, // IDP specific type
  'o_idpspec5', #= 13, // IDP specific type
  'o_last'      #= 14; // first unused type
)

_flags={
	'OF_NO_BASE_DISP': 0x80,  # o_displ: base displacement doesn't exist; meaningful only for o_displ type, if set, base displacement (x.addr) doesn't exist.
	'OF_OUTER_DISP'  : 0x40,  # o_displ: outer displacement exists; meaningful only for o_displ type, if set, outer displacement (x.value) exists.
	'PACK_FORM_DEF'  : 0x20,  # !o_reg + dt_packreal: packed factor defined.
	'OF_NUMBER'  	 : 0x10,  # can be output as number only; if set, the operand can be converted to a number only
	'OF_SHOW'        : 0x08   # should the operand be displayed?  if clear, the operand is hidden and should not be displayed
}

def _parseFlags(op, opNew, ins):
	s, f='', op.flags
	for k, v in _flags.iteritems():
		if f&v:
			if s: s=s+'|'+k
			else: s=k
	return s

_regs=(
(
	'',
	'',
	'',
	'',
	'',
	'',
	'',
	'',

	'r8b',
	'r9b',
	'r10b',
	'r11b',
	'r12b',
	'r13b',
	'r14b',
	'r15b',

	'al',
	'cl',
	'dl',
	'bl',
	'ah',
	'ch',
	'dh',
	'bh',
	'spl',
	'bpl',
	'sil',
	'dil'
),
(
	'ax',
	'cx',
	'dx',
	'bx',
	'sp',
	'bp',
	'si',
	'di',

	'r8w',
	'r9w',
	'r10w',
	'r11w',
	'r12w',
	'r13w',
	'r14w',
	'r15w',
),
(
	'eax',
	'ecx',
	'edx',
	'ebx',
	'esp',
	'ebp',
	'esi',
	'edi',

	'r8d',
	'r9d',
	'r10d',
	'r11d',
	'r12d',
	'r13d',
	'r14d',
	'r15d',
),
(
	'rax',
	'rcx',
	'rdx',
	'rbx',
	'rsp',
	'rbp',
	'rsi',
	'rdi',

	'r8',
	'r9',
	'r10',
	'r11',
	'r12',
	'r13',
	'r14',
	'r15',
)
)


_sib16=(
('bx', 'si'),
('bx', 'di'),
('bp', 'si'),
('bp', 'di'),
('si', ''),
('di', ''),
('bp', ''),
('bx', '')
)

_dtypes=(
	'byte',         #0       // 8 bit
	'word',         #1       // 16 bit
	'dword',        #2       // 32 bit
	'dt_float',        #3       // 4 byte
	'dt_double',       #4       // 8 byte
	'dt_tbyte',        #5       // variable size (ph.tbyte_size)
	'dt_packreal',     #6       // packed real format for mc68040
# ...to here the order should not be changed, see mc68000
	'qword',        #7       // 64 bit
	'dt_byte16',       #8       // 128 bit
	'dt_code',         #9       // ptr to code (not used?)
	'dt_void',         #10      // none
	'dt_fword',        #11      // 48 bit
	'dt_bitfild',      #12      // bit field (mc680x0)
	'dt_string',       #13      // pointer to asciiz string
	'dt_unicode',      #14      // pointer to unicode string
	'dt_3byte',        #15      // 3-byte data
	'dt_ldbl',         #16      // long double (which may be different from tbyte)
	'dt_byte32',       #17      // 256 bit
	'dt_byte64'       #18      // 512 bit
)


def _parseRegs(op, opNew, ins):
	if opNew['type']=='reg':
		return _regs[op.dtyp][op.reg]

_ifIndex=(False, True)

_segs={0:'none', 0x200000: 'data', 0x1f0000: 'stack', 0x1e0000: 'code'}

def _parseSib(op, opNew, ins):
	if opNew['type']=='sib' or opNew['type']=='sib+dis':
		aux=ins.auxpref
		aux&=0x1018
		size=None
		if aux==0x1000 or aux==8:
			#16bits
			opNew['base'], opNew['index']=_sib16[op.reg]	# reg acts as base register;
		elif aux==0x1010:
			size=3	#64
		else:
			size=2	#32

		if size is not None:
			if opNew['ifIndex']:
				bs=op.specflag2&7
				if ins.insnpref&1: #REX.B
					bs|=8
				opNew['base']=_regs[size][bs]

				ix_scl=(op.specflag2&0xff)>>3
				ix=ix_scl&7
				if ix!=4: #esp can't be used as index
					opNew['scale']=2**(ix_scl>>3)
					if ins.insnpref&2: #REX.X
						ix|=8
					opNew['index']=_regs[size][ix]
			else:
				opNew['reg']='*'
				opNew['base']=_regs[size][op.reg]	# reg acts as base register;
		return '*'
	if op.specflag2==0:
		return 'none'

_attrs=OrderedDict([
    ('type', _typeList),			
    ('flags', _parseFlags),
    ('dtyp', _dtypes),			
    ('reg', _parseRegs),                                                          
    ('value',''),              # o_imm
    ('addr',''),               # displacement
    ('specval', _segs),          # 20 0000: data; 1f 0000: stack; 1e 0000: code; 
    ('specflag1', _ifIndex),       # if SIB exists.
    ('specflag2', _parseSib),	# SIB byte	-> base, index, scale
    ('specflag3',''),
    ('specflag4','')
])

_cusAttrs=dict([
    ('dtyp', 'dtype'),			
    ('value', 'imm'),            
    ('addr', 'disp'),           
    ('specval', 'seg'),
    ('specflag1', 'ifIndex')
])


#output as dict
def trans(ea=None, ix=0):
	if ea is None:
		ea=ScreenEA()
	ins=DecodeInstruction(ea)
	op=ins.Operands[ix]
	if not op.type:
		return None
	opNew=OrderedDict()
	for key, fmt in _attrs.iteritems():
		val=None
		oVal=getattr(op, key)
		if fmt:
			tp=type(fmt)
			if tp==tuple: 
				val=fmt[oVal]
			elif tp==dict:
				val=fmt.get(oVal)
			elif tp==types.FunctionType:
				val=fmt(op, opNew, ins)

		if val is None:
			#fmt=_attrsRaw[key]
			#val=unpack('@'+fmt, pack('@'+fmt.lower(), oVal))[0]
			val=oVal
		if _cusAttrs.has_key(key):
			opNew[_cusAttrs[key]]=val
		else:
			opNew[key]=val
	return opNew

#formated output of trans
def parse(ea=None, ix=0):
	if ea is None:
		ea=ScreenEA()
	print '--------------------------------'
	print ea, ':', 
	print(GetOpnd(ea, ix))
	print '--------------------------------'
	for key, val in trans(ea, ix).iteritems():
		print('{0:<15}={1}'.format(key, val))

# output assembly from ida info;
def restore(ea=None, ix=0):
	rs=''
	if ea is None:
		ea=ScreenEA()
	attrs=trans(ea, ix)
	tp=attrs['type']
	if tp== 'reg':
		rs= attrs['reg']
	elif tp== 'moff':
		rs= attrs['dtype']+' ['+str(attrs['disp'])+']'
	elif tp== 'imm':
		rs= attrs['imm']
	elif tp== 'sib' or tp== 'sib+dis':
		s= attrs['dtype']+' ['
		s+=attrs['base']
		if attrs['ifIndex']:
			s+='+'
			if attrs['scale']>1:
				s+= '%i*'%attrs['scale']
			s+=str(attrs['index'])
		if tp== 'sib+dis':
			s+='+'+str(attrs['disp'])
		rs= s+']'
	print '%s[%i]: %s' % (ea, ix, rs)
