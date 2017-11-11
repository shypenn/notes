from struct import *
import idc

def copy(fl, ea=None):
	if ea is None:
		ea=idc.ScreenEA();

	fl=open(fl, 'rb')
	ctn=fl.read();
	fl.close()

	for i, c in enumerate(ctn):
		idc.PatchByte(ea+i, ord(c))

def dump(_char, _format):
	print hex(unpack('@'+_format.upper(), pack('@'+_format, _char))[0])

