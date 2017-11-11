from ctypes import *
import sys


INT=sys.maxsize > 2**32 and c_int64 or c_int
BYTES=sys.maxsize > 2**32 and 8 or 4

def printNum(this, fl, flag):
	print 'p'
	addr=pythonapi.PyFile_FromFile(fl, '', 'w', None)
	pfl=INT(addr)
	pfl=py_object.from_address(addressof(pfl))
	print >>pfl.value, hex(this)
	return 0

def reprNum(this):
	return hex(this)



printType=PYFUNCTYPE(INT, py_object, c_void_p, INT)
printFn=printType(printNum)
printPt=c_void_p.from_address(addressof(printFn))

tp_print=INT.from_address(id(int)+BYTES*7);
tp_print.value=printPt.value
tp_print=INT.from_address(id(long)+BYTES*7);
tp_print.value=printPt.value



reprType=PYFUNCTYPE(py_object, py_object)
reprFn=reprType(reprNum)
reprPt=c_void_p.from_address(addressof(reprFn))

tp_repr=INT.from_address(id(int)+BYTES*11);
tp_repr.value=reprPt.value
tp_str=INT.from_address(id(int)+BYTES*17);
tp_str.value=reprPt.value

tp_repr=INT.from_address(id(long)+BYTES*11);
tp_repr.value=reprPt.value
tp_str=INT.from_address(id(long)+BYTES*17);
tp_str.value=reprPt.value
