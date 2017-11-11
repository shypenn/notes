from struct import *
from collections import namedtuple
class Header():
	MAGIC=0x5a4d
	PE=0x4550

	_fmtDosHeader='=H58xL'
	_tplDosHeader=namedtuple('DosHeader', 'magic peAddr')

	_fmtPEHeader='L20s112s'
	_tplPEHeader=namedtuple('PEHeader', 'sign coff opt')

	_fmtCOFFHeader='=HHLLLHH'
	_tplCOFFHeader=namedtuple('CoffHeader', (
		"Target_Machine_type",
		"Number_Of_Sections",
		"Time_Date_Stamp",
		"File_offset_to_COFF_symbol_table",
		"Number_of_symbols",
		"Size_of_optional_header",
		"characteristics"
		))

	_fmtOptHeader='=HHLLLLLLLLLLLLLLLLHHLLLLLL'
	_tplOptHeader=namedtuple('OptHeader', '''
	Magic
	Linker_Version
	Sum_file_size_of_all_CODE_sections
	Sum_file_size_of_all_INITIALIZED_DATA_sections
	Sum_file_size_of_all_UNINITIALIZED_DATA_sections
	OEP
	Base_of_Code
	Base_of_Data
	Image_Base
	section_alignment_in_memory
	section_alignment_in_file
	OS_version
	Image_version
	Subsystem_version
	Win32_version_reserved
	Size_of_Image
	Size_of_Header
	Checksum
	subsystem
	dll_characteristics
	stack_reserved
	stack_committe
	heap_reserved
	heap_committed
	Loader_flags
	number_of_directory_entries''')

	mem=''

	def parse(this, _mem):
		this.mem=_mem
		this.DosHeader=this._tplDosHeader._make(unpack(this._fmtDosHeader, this.mem[:calcsize(this._fmtDosHeader)]))
		if this.DosHeader.magic!=this.MAGIC:
			raise ValueError('Invalid PE')
		fmt='='+str(this.DosHeader.peAddr)+'x'+this._fmtPEHeader
		this.PEHeader=this._tplPEHeader._make(unpack(fmt, this.mem[:calcsize(fmt)]))
		if this.PEHeader.sign!=this.PE:
			raise ValueError('Invalid PE')
		this.COFFHeader=this._tplCOFFHeader._make(unpack(this._fmtCOFFHeader, this.PEHeader.coff))
		print '---COFFHeader---'
		for i in this.COFFHeader._fields:
			print i,': ', hex(getattr(this.COFFHeader,i))
		print '---OptionalHeader---'
		this.OptHeader=this._tplOptHeader._make(unpack(this._fmtOptHeader, this.PEHeader.opt[:calcsize(this._fmtOptHeader)]))
		for i in this.OptHeader._fields:
			print i,': ', hex(getattr(this.OptHeader,i))

	def __init__(this, *_mem):
		if len(_mem)>0:
			this.parse(_mem[0])

	
