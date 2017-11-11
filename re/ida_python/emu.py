from copy import *
from idaapi import *
from idc import *
from idautils import *
from pprint import pprint
import op as Op

_REGS={
	16:(
		'ax',
		'cx',
		'dx',
		'bx',
		'sp',
		'bp',
		'si',
		'di',

		'ip'
	)
	,32:(
		'eax',
		'ecx',
		'edx',
		'ebx',
		'esp',
		'ebp',
		'esi',
		'edi',
		
		'eip'
	)
	,64:(
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

		'rip'
	)
}

_AX=0
_CX=1
_DX=2
_BX=3
_SP=4
_BP=5
_SI=6
_DI=7

_R8=8
_R9=9
_R10=10
_R11=11
_R12=12
_R13=13
_R14=14
_R15=15

_BYTEL=0xff
_BYTEH=0xff00
_WORD=0xffff
_DWORD=0xffffffff

_REGMAPS={
	16:{
		'al':[_AX, _BYTEL],
		'cl':[_CX, _BYTEL],
		'dl':[_DX, _BYTEL],
		'bl':[_BX, _BYTEL],
		'ah':[_AX, _BYTEH],
		'ch':[_CX, _BYTEH],
		'dh':[_DX, _BYTEH],
		'bh':[_BX, _BYTEH]
	}
	,32:{
		'ax':[_AX, _WORD],
		'cx':[_CX, _WORD],
		'dx':[_DX, _WORD],
		'bx':[_BX, _WORD],
		'sp':[_SP, _WORD],
		'bp':[_BP, _WORD],
		'si':[_SI, _WORD],
		'di':[_DI, _WORD]
	}
	,64:{
		'spl':[_SP, _BYTEL],
		'bpl':[_BP, _BYTEL],
		'sil':[_SI, _BYTEL],
		'dil':[_DI, _BYTEL],
		'r8b':[_R8, _BYTEL],
		'r9b':[_R9, _BYTEL],
		'r10b':[_R10, _BYTEL],
		'r11b':[_R11, _BYTEL],
		'r12b':[_R12, _BYTEL],
		'r13b':[_R13, _BYTEL],
		'r14b':[_R14, _BYTEL],
		'r15b':[_R15, _BYTEL],

		'r8w':[_R8, _WORD],
		'r9w':[_R9, _WORD],
		'r10w':[_R10, _WORD],
		'r11w':[_R11, _WORD],
		'r12w':[_R12, _WORD],
		'r13w':[_R13, _WORD],
		'r14w':[_R14, _WORD],
		'r15w':[_R15, _WORD],

		'eax':[_AX, _DWORD],
		'ecx':[_CX, _DWORD],
		'edx':[_DX, _DWORD],
		'ebx':[_BX, _DWORD],
		'esp':[_SP, _DWORD],
		'ebp':[_BP, _DWORD],
		'esi':[_SI, _DWORD],
		'edi':[_DI, _DWORD],
		'r8d':[_R8, _DWORD],
		'r9d':[_R9, _DWORD],
		'r110d':[_R10, _DWORD],
		'r11d':[_R11, _DWORD],
		'r12d':[_R12, _DWORD],
		'r13d':[_R13, _DWORD],
		'r14d':[_R14, _DWORD],
		'r15d':[_R15, _DWORD]
	}
}
_REGMAPS[32].update(deepcopy(_REGMAPS[16]))
_REGMAPS[64].update(deepcopy(_REGMAPS[32]))
for v in _REGMAPS[16].values():
	v[0]=_REGS[16][v[0]]
for v in _REGMAPS[32].values():
	v[0]=_REGS[32][v[0]]
for v in _REGMAPS[64].values():
	v[0]=_REGS[64][v[0]]


def patch(addr, val, tp):
	if tp=='byte':
		PatchByte(addr, val)
		MakeByte(addr)
	elif tp=='word':
		PatchWord(addr, val)
		MakeWord(addr)
	elif tp=='dword':
		PatchDword(addr, val)
		MakeDword(addr)
	elif tp=='qword':
		PatchDword(addr, val&0xffffffff)
		PatchDword(addr+4, (val>>16)&0xffffffff)
		MakeQword(addr)
	else:
		print 'patch: no data type:', tp

def read(addr, tp):
	if tp=='byte':
		return Byte(addr)
	elif tp=='word':
		return Word(addr)
	elif tp=='dword':
		return Dword(addr)
	elif tp=='qword':
		return Qword(addr)
	else:
		return 0
		print 'read: no data type:', tp

class Xref():
	ix=0
	ea=0
	src=None #Op
	def __init__(this, ix, ea, src):
		this.ix=ix
		this.ea=ea
		this.src=src

_SIZE_MASK={'byte':[0x80, 0xff, 1, 8], 'word':[0x8000, 0xffff, 2, 16], 'dword':[0x80000000, 0xffffffff, 4, 32], 'qword':[0x8000000000000000, 0xffffffffffffffff, 8, 64]}
_SIZE_MASK_SIGN=0
_SIZE_MASK_MAGN=1
_SIZE_MASK_BYTES=2
_SIZE_MASK_BITS=3

class _Line():
	regs=None #regs of current location
	flags=None
	ea=-1
	mn=''
	rep=''
	ops=None
	inferred=False
	#toJmp=False #transfer to other address

	def __init__(this, ea):
		this.ea=ea

		ins=DecodeInstruction(ea)
		this.mn=ins.get_canon_mnem()
		if ins.auxpref&2:
			if ins.itype==0x1c or ins.itype==0xa7:
				this.rep='repe'
			else:
				this.rep='rep'
		elif ins.auxpref&4:
			this.rep='repne'

		this.parse()

	def parse(this):
		this.ops=(Op.trans(this.ea, 0), Op.trans(this.ea, 1))
		#this.toJmp=this.mn.startswith('j') or this.mn in ('call', 'ret', 'retn')
		if this.ops[0] is not None:
			this.__parse()


	def __parse(this):
		ix=-1
		for op in this.ops:
			if op is None: break
			ix=ix+1
			op['isMem']=False
			op['value']=op['addr']=0

			xea=None
			if op['type']=='imm':
				op['value']=op['imm']
			elif op['type']=='near':
				op['value']=op['disp']
			elif op['type']=='reg':
				reg=this.getReg(op['reg'])
				if reg is not None: 
					op['value']=reg
					if not (not ix and this.ops[1] is not None): # not to write to reg;
						xea=op['value']
				else: #withdraw reversely
					aop=AltOp(this.ea, ix)
					if aop:
						addr=LocByName(aop.split().pop()) #strip 'offset'
						if addr>-1:
							this.inferred=True
							this.setReg(op['reg'], addr)
							op['addr']=op['value']=addr

			elif op['type']=='moff':
				op['isMem']=True
				xea=op['addr']=op['disp']
			elif op['type']=='sib' or op['type']=='sib+dis': 
				op['isMem']=True
				base=this.getReg(op['base'])
				if base is not None: #all regs' value were specified
					if op.has_key('index'):
						index=this.getReg(op['index'])
						if index is not None: 
							xea=base+index*op['scale']
					else:
						xea=base
				if xea is not None:
					xea=op['addr']=xea+op['disp']
				else:#withdraw reversely
					aop=AltOp(this.ea, ix)
					got=None
					if aop:
						addr=LocByName(aop.split().pop())
						if addr>-1: #only one reg can be infered;
							if base is not None: #index must exist and no value specified
								this.setReg(op['index'], (addr-op['disp']-base)/op['scale'])
								got=True
							else:
								if op.has_key('index'):
									index=this.getReg(op['index'])
									if index is not None: 
										this.setReg(op['base'], addr-op['disp']-index*op['scale'])
										got=True
								else:
									this.setReg(op['base'], addr-op['disp'])
									got=True

					if got is None:
						print 'no sib regs'
					else:
						op['addr']=addr
						this.inferred=True
			if xea is not None:
				xref=op['xref']=Xref(ix, xea, op)
				this.procXref(xref)

	def procXref(this, xref):
		flg=GetFlags(xref.ea)
		flw=-1
		
		eaCalled=0
		if flg:
			if this.mn=='call':
				flw=fl_CN
			elif this.mn.startswith('j'):
				flw=fl_JN
			if flw>-1:
				if xref.src['type']=='near':
					eaCalled=xref.ea
				else: #indirect call/jump; two refs;
					eaCalled=getValue()
					flg=GetFlags(eaCalled)
					add_dref(this.ea, xref.ea, XREF_USER|dr_R)
				if isUnknown(flg) or not isCode(flg):
					MakeCode(eaCalled) #will fail if not unknown;
				AddCodeXref(this.ea, eaCalled, XREF_USER|flw) #name is created or changed;
				name=Name(eaCalled)
			else:
				isOff=False
				if this.mn=='lea' or this.ops[xref.ix]['type']=='reg': #offset
					#lea
					#mov x, eax
					add_dref(this.ea, xref.ea, XREF_USER|dr_O)
					isOff=True
				else: #memory
					if isUnknown(flg) or not isData(flg):
						if this.ops[0]['dtype']=='byte':
							MakeByte(xref.ea)
						elif this.ops[0]['dtype']=='word':
							MakeWord(xref.ea)
						elif this.ops[0]['dtype']=='dword':
							MakeDword(xref.ea)
						elif this.ops[0]['dtype']=='qword':
							MakeQword(xref.ea)
					if not xref.ix and this.ops[1]:
						add_dref(this.ea, xref.ea, XREF_USER|dr_W)
					else:
						add_dref(this.ea, xref.ea, XREF_USER|dr_R)
				name=Name(xref.ea)

				if not name: #offset;	?????inside a defined data
#create?????
					name='unk_'+hex(xref.ea)
				if isOff:
					name='offset '+name

		else: #number instead of address in reg or lea target;
			name=hex(xref.ea)
			if this.ops[xref.ix]['isMem']:
				name='['+name+']'
		op=GetOpnd(this.ea, xref.ix)
		if not op: #stos/movs....
			name=' '+name+' '
		OpAlt(this.ea, xref.ix, name)
		if not this.inferred and op:
			if xref.ix and this.ops[0].has_key('xref'): #reserve the first operand comment
				op=(CommentEx(this.ea, 0) or '')+','+op
			MakeComm(this.ea, op)
		#if flw>-1:
		#	del_dref(this.ea, xref.ea) #text ref is useless

	def calc(this):
		if opcodes.has_key(this.mn):
			eaJmped= opcodes[this.mn](this)
			if this.ops[0] and this.ops[0].has_key('xref'): #content changed, name maybe changed accordingly;
				this.procXref(this.ops[0]['xref'])
				if this.ops[1] and this.ops[1].has_key('xref'): #may be overriden
					cmt=(CommentEx(this.ea, 0) or '')+','+ GetOpnd(this.ea, 1)
					MakeComm(this.ea, cmt)
			return eaJmped
		else:
			print '[%s] not implemented.'%this.mn

	def setReg(this, reg, val):
		if reg in this.ctx.REGS:
			this.regs[reg]=val
		elif this.ctx.REGMAP.has_key(reg):
			mp=this.ctx.REGMAP[reg]
			if not mp[1]&1: #ff00
				val=val<<(-mp[1])&mp[1]
			if this.regs.has_key(mp[0]):
				oval=this.regs[mp[0]]
				oval=oval&(~mp[1])
				val=val|oval
			this.regs[mp[0]]=val
		else:
			print 'no ', reg

	def getReg(this, reg):
		val=None
		if this.regs.has_key(reg):
			val=this.regs[reg]
		elif this.ctx.REGMAP.has_key(reg):
			mp=this.ctx.REGMAP[reg]
			if this.regs.has_key(mp[0]):
				val=this.regs[mp[0]]&mp[1]
				if not mp[1]&1: #ff00
					val=val>>(-mp[1])&mp[1]
		return val

	def getRegIx(this, regIx):
		reg=this.ctx.REGS[regIx]
		if reg:
			return this.getReg(reg)
		else:
			return None

	#def patch(this, addr, val, tp):
	#	patch(addr, val, tp)
	#	if tp==this.ctx.wordType and isEnabled(val): #address
	#		name=Name(val)
	#		#OpAlt(addr, 0, name)
	#		if not name:
	#			name=str(val)
	#		MakeName(addr, 'os_'+name)
	#		this.parse()


	def setValue(this, val, ix=0):
		op=this.ops[ix]
		if op is None: raise ValueError('op of ix 0 is none')
		if op['type']=='reg':
			this.setReg(op['reg'], val)
		elif op['addr']:
			patch(op['addr'], val, op['dtype'])

	def getValue(this, ix=0):
		op=this.ops[ix]
		if op is None: raise ValueError('op of ix:%i is none'%ix)
		#print op['dtype']
		if op['isMem']:
			if op['addr']:
				addr=op['addr']
				return read(addr, op['dtype'])
			else:
				return 0
		else:
			return op['value']
	
	def setOverrunFlag(this, val):
		msk=_SIZE_MASK[this.ops[0]['dtype']]
		#signed
		if val<-msk[_SIZE_MASK_SIGN] or val>=msk[_SIZE_MASK_SIGN]:
			this.flags['of']=1
		else:
			this.flags['of']=0
		#unsigned
		if val>msk[_SIZE_MASK_MAGN] or val<0:
			this.flags['cf']=1
		else:
			this.flags['cf']=0

	def setOverflowFlag(this, val):
		msk=_SIZE_MASK[this.ops[0]['dtype']]
		#signed
		if val<-msk[_SIZE_MASK_SIGN] or val>=msk[_SIZE_MASK_SIGN]:
			this.flags['of']=1
		else:
			this.flags['of']=0
	def setCarryFlag(this, val):
		msk=_SIZE_MASK[this.ops[0]['dtype']]
		#unsigned
		if val>msk[_SIZE_MASK_MAGN] or val<0:
			this.flags['cf']=1
		else:
			this.flags['cf']=0



	def setGenFlag(this, val):
		if val:
			this.flags['zf']=0
			if val<0:
				this.flags['sf']=1
			else:
				this.flags['sf']=0
		else:
			this.flags['zf']=1


_array_key='EMU_DATA'
_array_id=-1
rnrs={}

_MAXRUNS=100
_BITNESS=['word', 'dword', 'qword']

#1). stack will not change if no 'sp' is specified
class rnr():
	Line=_Line #subclass of Line
	cln=None #current line
	regList=None #[] init regs value of all function
	startEA=0
	stepIn=False #when true, just transfer to the first function and stop
	#toPH=False
	SP=''
	IP=''
	REGMAP=None
	REGS=None
	FLAGS={'zf':0, 'sf':0, 'cf':0, 'of':0, 'df':0}


	def __init__(this, regDictOrEA=None, **_regs):
		if not _regs:
			if regDictOrEA is None:
				_regs={}
			elif type(regDictOrEA)==dict:
				_regs=regDictOrEA
			else:
				this.startEA=regDictOrEA

		class SubLine(_Line):
			ctx=this
			flags=dict(this.FLAGS)
		this.Line=SubLine
		#this.Line=type('Line', (_Line,), {ctx:this}) 

		ea=this.startEA or here()
		bitness=GetSegmentAttr(ea, SEGATTR_BITNESS)
		this.wordType=_BITNESS[bitness]
		this.wordMask=_SIZE_MASK[this.wordType]
		bits=this.wordMask[_SIZE_MASK_BITS]
		this.REGMAP=_REGMAPS[bits]
		this.REGS=_REGS[bits]
		this.SP= this.REGS[_SP]
		this.IP= this.REGS[len(this.REGS)-1]

		if this.startEA:
			if not this.restore(): return
		else: 
			this.startEA=ea
			this.regList=[]
			this.appendFun(this.startEA, _regs)

		rnrs[this.startEA]=this;
		print 'Start EA:', this.startEA


	def sp(this):
		if this.Line.regs.has_key(this.SP):
			this.jump(this.Line.regs[this.SP])
		else:
			print 'no ', this.SP

	def curEA(this, ea=None):
		if ea:
			this.Line.regs[this.IP]=ea
		else:
			return this.Line.regs[this.IP]

	def upd(this, ea=None): #reparse this line
		this.Line(ea or here())

	def ip(this, ea=None):
		this.jump(ea or this.curEA())

	def regs(this):
		pprint(this.regList)

	def flg(this, r=None, v=None):
		if r is None:
			pprint(this.cln.flags)
		elif v is None:
			print this.cln.flags(r)
		else:
			this.cln.flags(r, v)
			print 'set:', r

	def reg(this, r=None, v=None):
		if r is None:
			pprint(this.cln.regs)
		elif v is None:
			print this.cln.getReg(r)
		else:
			this.cln.setReg(r, v)
			print 'set:', r

	def jump(this, ea):
		if isEnabled(ea):
			this.curEA(ea)
			Jump(ea)
			print 'to:', ea
			return True
		else:
			print 'Invalid address:', ea

	def restore(this):
		_data=GetArrayElement(AR_STR, _array_id, this.startEA)
		if _data:
			try:
				_data=eval(_data)
				this.regList=_data['regs']
				this.Line.regs=_data['cregs']
				this.jump(this.curEA())
			except:
				DelArrayElement(AR_STR, _array_id, this.startEA)
				return False
		return True

	def save(this, withoutEA=False):
		checkId()
		SetArrayString(_array_id, this.startEA, str({'regs':this.regList, 'cregs':this.Line.regs}))
		if not withoutEA:
			SetArrayString(_array_id, 0, str(rnrs.keys()))
		

	def appendFun(this, ea, _regs):
		_regs[this.IP]=ea
		this.regList.append(_regs)
		this.Line.regs=dict(_regs)

	#0: to function start upward with level of 'levelBack'
	def reset(this, levelBack=None): 
		if levelBack is None:
			level=0

		if levelBack > 0:
			ln=len(this.regList)
			del this.regList[ln-levelBack:ln]

		cur=this.regList[len(this.regList)-1]
		this.Line.regs=dict(cur)
		this.ip()

#####################################################
#running

	#step
	def s(this, _stepIn=False): 
		this.ss(1, _stepIn)

	#steps
	def ss(this, steps=1, _stepIn=False):
		this.stepIn=_stepIn
		eaJmped=None
		eaStart=this.curEA()-1;
		while steps:
			if eaJmped:
				eaStart=eaJmped
			else:
				eaStart=FindCode(eaStart, SEARCH_DOWN)
			print 'EA:', eaStart
			curLine=this.Line(eaStart)
			eaJmped=curLine.calc()
			steps=steps-1

		#if not curLine.toJmp or not _stepIn:
		this.cln=curLine
		if not eaJmped:
			this.jump(FindCode(eaStart, SEARCH_DOWN))
		print('done')
		this.stepIn=False
	
	def to(this, eaEnd, _stepIn=False): 
		this.stepIn=_stepIn
		runs=_MAXRUNS
		eaJmped=None
		eaStart=this.curEA();
		while eaStart!=eaEnd:
			print 'EA:', eaStart
			curLine=this.Line(eaStart)
			eaJmped=curLine.calc()
			if not runs:
				print 'reach max runs: ', _MAXRUNS
				break
			if eaJmped:
				eaStart=eaJmped
			else:
				eaStart=FindCode(eaStart, SEARCH_DOWN)
		this.cln=curLine
		this.jump(eaStart)
		print('done')
		this.stepIn=False

	def end(this, _stepIn=False, _out=False):  #don't step in;
		end=GetFunctionAttr(this.curEA(), FUNCATTR_END)
		if end==-1:
			print("can't find end address")
			return 
		end=FindCode(end, SEARCH_UP) #end is beyond the end
		if not GetMnem(end).startswith('ret'): #incorrect function
			this.stepIn=_stepIn
			runs=_MAXRUNS
			eaJmped=None
			eaStart=this.curEA()-1;
			while eaStart:
				if eaJmped:
					eaStart=eaJmped
				else:
					eaStart=FindCode(eaStart, SEARCH_DOWN)
				print 'EA:', eaStart
				curLine=this.Line(eaStart)
				ret=curLine.mn.startswith('ret')
				if ret:
					if _out:
						eaJmped=curLine.calc()
					else:
						eaJmped=eaStart
						this.jump(eaStart)
					break
				else:
					eaJmped=curLine.calc()
				runs=runs-1
				if not runs:
					print 'reach max runs: ', _MAXRUNS
					break
			this.cln=curLine
			if not eaJmped:
				this.jump(FindCode(eaStart, SEARCH_DOWN))
			print('done')
			this.stepIn=False
		else:
			this.to(end if _out else FindCode(end, SEARCH_UP), _stepIn)
					
	def out(this, _stepIn=False):  #don't step in;
		end(this, _stepIn, True)

	#branching
	def ph(this, _stepIn=False):
		this.stepIn=_stepIn
		#if not _stepIn:
		#	this.toPH=True
		runs=_MAXRUNS
		eaJmped=None
		eaStart=this.curEA()-1;
		while eaStart:
			if eaJmped:
				eaStart=eaJmped
			else:
				eaStart=FindCode(eaStart, SEARCH_DOWN)
			print 'EA:', eaStart
			curLine=this.Line(eaStart)
			eaJmped=curLine.calc()
			if eaJmped is not None:break
			runs=runs-1
			if not runs:
				print 'reach max runs: ', _MAXRUNS
				break
		this.cln=curLine
		if not eaJmped:
			this.jump(FindCode(eaStart, SEARCH_DOWN))
		#this.toPH=this.stepIn=False
		this.stepIn=False
		print('done')


#manipulate rnrs;
def checkId():
	global _array_id
	if _array_id==-1: 
		_array_id=GetArrayId(_array_key)
	if _array_id==-1:
		_array_id=CreateArray(_array_key)

def save():
	ln=len(rnrs)
	if ln==0:
		print 'no rnrs'
		return
	checkId()
	eas=[]
	for ea, r in rnrs.iteritems():
		eas.append(ea)
		r.save(True)
	SetArrayString(_array_id, 0, str(eas))
	info('%d rnrs saved.'%ln)

def restore():
	global _array_id
	_array_id=GetArrayId(_array_key)
	if _array_id==-1:
		print 'No data stored, No _array_id'
		return
	eas=GetArrayElement(AR_STR, _array_id, 0)
	if not eas: 
		print 'No data stored, no EAS'
		return
	eas=eval(eas)
	for ea in eas:
		rnr(ea)
	SetArrayString(_array_id, 0, str(rnrs.keys()))

def remove(ea):
	if rnrs.has_key(ea):
		del rnrs[ea]
		if _array_id==-1: return
		DelArrayElement(AR_STR, _array_id, ea)

def clear():
	for ea in rnrs.keys():
		del rnrs[ea]
		if _array_id>-1: 
			DelArrayElement(AR_STR, _array_id, ea)
	if _array_id:
		DelArrayElement(AR_STR, _array_id, 0)
	
#save automatically
class HK(IDP_Hooks):
	def closebase(self):
		save()
		return 1

restore()
HK().hook()

#opcodes:
#return ea if jumped;
#return false if canceled jumped


def bypass(line):
	pass

def add(line):
	val=line.getValue()+line.getValue(1)
	line.setValue(val)

	line.setOverrunFlag(val)
	line.setGenFlag(val)

def AND(line):
	line.setValue(line.getValue()&line.getValue(1))

	line.flags['of']=line.flags['cf']=0
	line.setGenFlag(val)

def call(line):
	#if line.ctx.toPH: return True #stop here
	sp=line.ctx.SP
	ea=line.getValue()
	if line.ctx.stepIn and line.ctx.jump(ea):
		if line.regs.has_key(sp):
			line.regs[sp]=line.regs[sp]-line.ctx.wordMask[_SIZE_MASK_BYTES]
			nea=FindCode(line.ea, SEARCH_DOWN)
			patch(line.regs[sp], nea, line.ctx.wordType)
			OpAlt(line.regs[sp], 0, GetFuncOffset(nea))
		line.ctx.appendFun(ea, line.regs)
		return ea

	f=get_func(ea)
	bytes=0
	if f is None:
		if line.ops[0]['xref']:
			inf=tinfo_t()
			if get_tinfo2(line.ops[0]['xref'].ea, inf):
				fd=func_type_data_t()
				inf.get_func_details(fd)
				bytes=fd.stkargs
	else:
		bytes=f.argsize

	if bytes:
		line.regs[sp]+=bytes
	return False

def CMP(line):
	val=line.getValue()-line.getValue(1)

	line.setOverrunFlag(val)
	line.setGenFlag(val)

def dec(line):
	line.setValue(line.getValue()-1)
	line.setOverflowFlag(val, line.ops[0]['dtype'])

def inc(line):
	line.setValue(line.getValue()+1)
	line.setOverflowFlag(val, line.ops[0]['dtype'])

def jmp(line):
	#if line.ctx.toPH: return True #stop here
	if line.ctx.stepIn:
		ea=line.getValue()
		line.ctx.jump(ea)
		return ea
	else:
		print 'bypass jmp'
		return False

def _jcc(line, con):
	#if line.ctx.toPH: return True #stop here
	print 'con:', con
	if line.ctx.stepIn:
		if con:
			ea=line.getValue()
		else:
			ea=FindCode(line.ea, SEARCH_DOWN)
		line.ctx.jump(ea)
		return ea
	else:
		print 'bypass jcc'
		return False
_jcon={
	'z': lambda ln: ln.flags['zf'] #e
	,'nz': lambda ln: not ln.flags['zf'] #ne
	,'cxz': lambda ln: not ln.regs['cx']
	,'ecxz': lambda ln: not ln.regs['ecx']
	,'rcxz': lambda ln: not ln.regs['rcx']
	,'o': lambda ln: ln.flags['of']
	,'no': lambda ln: not ln.flags['of']
	,'s': lambda ln: ln.flags['sf']
	,'ns': lambda ln: not ln.flags['sf']
	,'a': lambda ln: not ln.flags['cf'] and not ln.flags['zf'] #nbe
	,'ae': lambda ln: not ln.flags['cf'] #nb/nc
	,'b': lambda ln: ln.flags['cf'] #b
	,'be': lambda ln: ln.flags['cf'] or ln.flags['zf']
	,'g': lambda ln: not ln.flags['zf'] and ln.flags['sf']==ln.flags['of'] #nle
	,'ge': lambda ln: ln.flags['sf']==ln.flags['of'] #nl
	,'l': lambda ln: ln.flags['sf']!=ln.flags['of'] #nge
	,'le': lambda ln: ln.flags['zf'] or ln.flags['sf']!=ln.flags['of'] #ng
	,'na': lambda ln: ln.flags['cf'] or ln.flags['zf']
	,'nae': lambda ln: ln.flags['cf']
}
def getJcc(ev):
	def fun(line):
		return _jcc(line, ev(line))
	return fun

def lea(line):
	line.regs[line.ops[0]['reg']]=line.ops[1]['addr']

def mov(line):
	line.setValue(line.getValue(1))

def OR(line):
	val=line.getValue()|line.getValue(1)
	line.setValue(val)

	line.flags['of']=line.flags['cf']=0
	line.setGenFlag(val)

def pop(line):
	sp=line.ctx.SP
	if line.regs.has_key(sp):
		line.setValue(Dword(line.regs[sp]))
		line.regs[sp]=line.regs[sp]+line.ctx.wordMask[_SIZE_MASK_BYTES]
		line.parse()
	else:
		print 'no ', sp
	
def push(line):
	sp=line.ctx.SP
	if line.regs.has_key(sp):
		line.regs[sp]=line.regs[sp]-line.ctx.wordMask[_SIZE_MASK_BYTES]
		patch(line.regs[sp], line.getValue(), 'dword')
	else:
		print 'no ', sp

def shl(line):
	val0=line.getValue()
	val1=line.getValue(1)
	msk=_SIZE_MASK[line.ops[0]['dtype']]
	val1&= msk[_SIZE_MASK_MAGN];
	if not val1: return

	val=val0<<val1
	line.setValue(val)

	line.setGenFlag(val)

	cf=val0&(msk[_SIZE_MASK_SIGN]>>(val1-1))
	line.flags['cf']=cf and 1 or 0

	if val1==1:
		line.flags['of']=cf^(val&msk[_SIZE_MASK_SIGN])
	
def shr(line):
	val0=line.getValue()
	val1=line.getValue(1)
	msk=_SIZE_MASK[line.ops[0]['dtype']]
	val1&= msk[_SIZE_MASK_MAGN];
	if not val1: return

	val=val0>>val1
	line.setValue(val)

	line.setGenFlag(val)

	cf=val0&(1<<(val1-1))
	line.flags['cf']=cf and 1 or 0

	if val1==1:
		line.flags['of']=val0&msk[_SIZE_MASK_SIGN] and 1 or 0
	
def stos(line):
	DI=line.ctx.REGS[_DI]
	if not DI:
		print 'no DI for stos'
		return 
	CX=line.ctx.REGS[_CX]
	if line.rep:
		cx=line.getReg(CX)
		if not cx: 
			print 'no CX for rep stos'
			return
	else:
		cx=1

	dType=line.ops[0]['dtype']
	dSize=_SIZE_MASK[dType][_SIZE_MASK_BYTES]	
	if line.flags['df']:
		dSize=-dSize;
	val1=line.getValue(1)
	di=line.getReg(DI)
	while cx:
		patch(di, val1, dType)
		di+=dSize;
		cx-=1
	line.setReg(DI, di);

	if line.rep:
		line.setReg(CX, 0)

def lods(line):
	SI=line.ctx.REGS[_SI]
	if not SI:
		print 'no SI for lods'
		return 
	CX=line.ctx.REGS[_CX]
	if line.rep:
		cx=line.getReg(CX)
		if not cx: 
			print 'no CX for rep lods'
			return
	else:
		cx=1

	dType=line.ops[0]['dtype']
	dSize=_SIZE_MASK[dType][_SIZE_MASK_BYTES]	
	if line.flags['df']:
		dSize=-dSize;

	AX=line.ops[0]['reg']
	si=line.getReg(SI)
	while cx:
		line.setReg(AX, read(si, dType))
		cx-=1
		si+=dSize
	line.setReg(SI, si);

	if line.rep:
		line.setReg(CX, 0)

def movs(line):
	SI=line.ctx.REGS[_SI]
	DI=line.ctx.REGS[_DI]
	if not SI:
		print 'no SI for movs'
		return 
	if not DI:
		print 'no DI for movs'
		return 
	CX=line.ctx.REGS[_CX]
	if line.rep:
		cx=line.getReg(CX)
		if not cx: 
			print 'no CX for rep movs'
			return
	else:
		cx=1

	dType=line.ops[0]['dtype']
	dSize=_SIZE_MASK[dType][_SIZE_MASK_BYTES]	
	if line.flags['df']:
		dSize=-dSize;
	si=line.getReg(SI)
	di=line.getReg(DI)
	while cx:
		patch(di, read(si, dType), dType)
		cx-=1
		si+=dSize
		di+=dSize
	line.setReg(SI, si);
	line.setReg(DI, di);

	if line.rep:
		line.setReg(CX, 0)

def cmps(line):
	SI=line.ctx.REGS[_SI]
	DI=line.ctx.REGS[_DI]
	if not SI:
		print 'no SI for movs'
		return 
	if not DI:
		print 'no DI for movs'
		return 
	CX=line.ctx.REGS[_CX]
	if line.rep:
		cx=line.getReg(CX)
		if not cx: 
			print 'no CX for rep movs'
			return
	else:
		cx=1

	dType=line.ops[0]['dtype']
	dSize=_SIZE_MASK[dType][_SIZE_MASK_BYTES]	
	if line.flags['df']:
		dSize=-dSize;

	si=line.getReg(SI)
	di=line.getReg(DI)
	while cx:
		val=read(si, dType)-read(di, dType)
		line.setGenFlag(val)
		line.setOverrunFlag(val)

		si+=dSize
		di+=dSize
		cx-=1

		if line.rep=='repe' and not line.flags.zf:
			break
		elif line.rep=='repne' and line.flags.zf:
			break

	line.setReg(SI, si);
	line.setReg(DI, di);

	if line.rep:
		line.setReg(CX, cx)

def scas(line):
	DI=line.ctx.REGS[_DI]
	if not DI:
		print 'no DI for movs'
		return 
	CX=line.ctx.REGS[_CX]
	if line.rep:
		cx=line.getReg(CX)
		if not cx: 
			print 'no CX for rep movs'
			return
	else:
		cx=1

	dType=line.ops[0]['dtype']
	dSize=_SIZE_MASK[dType][_SIZE_MASK_BYTES]	
	if line.flags['df']:
		dSize=-dSize;
	val1=line.getValue(1)
	di=line.getReg(DI)
	while cx:
		val=val1-read(di, dType)
		line.setGenFlag(val)
		line.setOverrunFlag(val)

		di+=dSize;
		cx-=1

		if line.rep=='repe' and not line.flags.zf:
			break
		elif line.rep=='repne' and line.flags.zf:
			break
	line.setReg(DI, di);

	if line.rep:
		line.setReg(CX, cx)


def sar(line):
	val0=line.getValue()
	val1=line.getValue(1)
	msk=_SIZE_MASK[line.ops[0]['dtype']]
	val1&= msk[_SIZE_MASK_MAGN];
	if not val1: return

	val=val0>>val1
	if val0&msk[_SIZE_MASK_SIGN]: #minus
		val|=~((msk[_SIZE_MASK_SIGN]>>(val1-1))-1)
	line.setValue(val)

	line.setGenFlag(val)

	cf=val0&(1<<(val1-1))
	line.flags['cf']=cf and 1 or 0

	if val1==1:
		line.flags['of']=0
	
def sub(line):
	val=line.getValue()-line.getValue(1)
	line.setValue(val)

	line.setOverrunFlag(val)
	line.setGenFlag(val)

def ret(line):
	if not line.ctx.stepIn: return
	regList=line.ctx.regList
	ix=len(regList)-1
	if not ix: 
		print 'at the topmost!'
		return
	if line.regs.has_key(line.ctx.SP):
		sp=line.ctx.SP
		ea=Dword(line.regs[sp])
		OpAlt(line.regs[sp], 0, '') #remove GetFuncOffset
		line.regs[sp]=line.regs[sp]+line.ctx.wordMask[_SIZE_MASK_BYTES]
		if line.ops[0] is not None: #remove arguments
			line.regs[sp]=line.regs[sp]+line.getValue()
		while ix:
			if regList[ix]['regs'][sp]<line.regs[sp]:
				del regList[ix]
			else:
				break
			ix=ix-1
	else:
		del regList[ix]
		ea=regList[ix-1].ea
	
	line.ctx.jump(ea)
	print 'ret:', ea
	return ea

def xor(line):
	val=line.getValue()^line.getValue(1)
	line.setValue(val)

	line.flags['of']=line.flags['cf']=0
	line.setGenFlag(val)

def xchg(line):
	val0=line.getValue()
	line.setValue(line.getValue(1))
	line.setValue(val0, 1)

opcodes={
	'add':add
	,'and':AND
	,'call':call
	,'cmp':CMP
	,'cmps':cmps
	,'dec':dec
	,'inc':inc
	,'jmp':jmp
	,'jz':getJcc(_jcon['z'])
	,'je':getJcc(_jcon['z'])
	,'jnz':getJcc(_jcon['nz'])
	,'jne':getJcc(_jcon['nz'])
	,'jcxz':getJcc(_jcon['cxz'])
	,'jecxz':getJcc(_jcon['ecxz'])
	,'jrcxz':getJcc(_jcon['rcxz'])
	,'jo':getJcc(_jcon['o'])
	,'jno':getJcc(_jcon['no'])
	,'js':getJcc(_jcon['s'])
	,'jns':getJcc(_jcon['ns'])
	,'ja':getJcc(_jcon['a'])
	,'jae':getJcc(_jcon['ae'])
	,'jb':getJcc(_jcon['b'])
	,'jbe':getJcc(_jcon['be'])
	,'jg':getJcc(_jcon['g'])
	,'jge':getJcc(_jcon['ge'])
	,'jl':getJcc(_jcon['l'])
	,'jle':getJcc(_jcon['le'])
	,'jna':getJcc(_jcon['na'])
	,'jnae':getJcc(_jcon['nae'])
	,'lea':lea
	,'lods':lods
	,'mov':mov
	,'movs':movs
	,'or':OR
	,'pop':pop
	,'push':push
	,'ret':ret
	,'retn':ret
	,'sal':shl
	,'sar':sar
	,'scas':scas
	,'shl':shl
	,'shr':shr
	,'stos':stos
	,'sub':sub
	,'xor':xor
	,'xchg':xchg
}
inf=tinfo_t()
get_tinfo2(here(), inf)
fd=func_type_data_t()
inf.get_func_details(fd)
