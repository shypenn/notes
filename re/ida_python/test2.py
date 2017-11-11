import ctypes
import sys

class SlotsProxy(ctypes.Structure):
    pass

SlotsProxy._fields_ = [
	('refcount', sys.maxsize > 2**32 and ctypes.c_int64 or ctypes.c_int),
    ('ob_type', ctypes.POINTER(SlotsProxy)),
    ('pdata', ctypes.POINTER(SlotsProxy)),
]

target=int.__dict__
proxy_dict = SlotsProxy.from_address(id(int.__dict__))
namespace = {}

# This is the way I found to `cast` this `proxy_dict.dict` into a python
# object, cause the `from_address()` function returns the `py_object`
# version
ctypes.pythonapi.PyDict_SetItem(
	ctypes.py_object(namespace),
	ctypes.py_object('data'),
	proxy_dict.pdata,
)
int_dict=namespace['data']
print type(proxy_dict.pdata)
print type(int_dict)
