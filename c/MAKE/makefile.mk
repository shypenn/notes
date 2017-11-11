BUILDDIR=
APPNAME=
OBJLIST=
RESLIST=

$(BUILDDIR)\$(APPNAME).exe: $(OBJLIST) $(RESLIST)
	link /nologo /debug /subsystem:console /out:$(BUILDDIR)\$(APPNAME).exe $(OBJLIST) $(RESLIST)


{}.cpp{$(BUILDDIR)}.obj:
	cl /nologo /c /EHsc /Fo$@ $<
{}.rc{.\$(BUILDDIR)}.res:
	rc /nologo /fo$@ $<


