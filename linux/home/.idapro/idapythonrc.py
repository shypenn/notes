#place this file in get_user_idadir() 
#the next line is set by install.cmd 
CUSTOM_SCRIPT_DIR='F:/NOTES/re/ida_python' 
#don't touch the previous line 
import sys 
sys.path.append(CUSTOM_SCRIPT_DIR) 
import idaapi 
idaapi.require('hexnum') 

idaapi.enable_extlang_python(True)

