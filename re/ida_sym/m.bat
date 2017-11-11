@if not defined $VCVARSET (
	(call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\vsvars32.bat")||(goto:ERROR)
)
@(
	cl.exe /c /Zi /DCL /Fdida.pdb /Tp ida_r.h
	cl.exe /c /Zi /DCL /Fdida64.pdb /Tp ida_r64.h
	pause
)
