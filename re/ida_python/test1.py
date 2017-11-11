import sys
#import op
#from idc import *

def fun():
	return 1, 2

print fun()
a, b=fun()
print a, b
a=fun()
print a

def dump(eaS, eaE=-1):
	if eaE==-1:
		eaE=eaS
	
	sys.stdout1=sys.stdout
	sys.stdout=open('py_out2.txt', 'w')

	try:
		while eaS<=eaE:
			op.raw(eaS, 1)
			eaS=FindCode(eaS, SEARCH_DOWN)
	except:
		print >>sys.stdout1, 'Error occurs'
	finally:
		sys.stdout.close()
		sys.stdout=sys.stdout1
		print 'Done'

def parse(eaS, eaE):
	if eaS is None:
		eaS=ScreenEA()
	if eaE is None:
		eaE=eaS
	
	try:
		while eaS<=eaE:
			op.restore(eaS, 1)
			eaS=FindCode(eaS, SEARCH_DOWN)
	except:
		print 'errrrrrrrrrrrrr'
def varArg(**args):
	print args

class intx(int):
	def __repr__(this):
		return hex(this)
	def __str__(this):
		return hex(this)

from ctypes import *

funType1=CFUNCTYPE(c_void_p)
intxFn=funType1(intx)
print hex(id(intxFn))
funType2=CFUNCTYPE(c_void_p, c_char)
parseFn=funType2(parse)
print hex(id(parseFn))
