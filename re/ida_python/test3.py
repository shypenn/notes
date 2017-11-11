import sys
import op
from idc import *
import ctypes
class cdictproxy(ctypes.Structure):
	pass
cdictproxy._fields_=[
		('refcount', sys.maxsize > 2**32 and ctypes.c_int64 or ctypes.c_int)
		,('pob', ctypes.POINTER(cdictproxy))
		,('pdata', ctypes.POINTER(cdictproxy))
	]

int_dict_c=cdictproxy.from_address(id(int.__dict__))
d={}
ctypes.pythonapi.PyDict_SetItem(ctypes.py_object(d), ctypes.py_object('dict'), int_dict_c.pdata)
int_dict=d['dict']
def xstr(this):
	print 'xxxxxxxxxx'
	return hex(this)
int_dict_c.pdata['say']= xstr
print type(int_dict_c.pdata)
print type(d['dict'])

