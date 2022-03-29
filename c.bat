@echo off

set ICLCFG=icl.cfg

del psrc.exe

call C:\IntelI0019\bin-ia32\icl1.bat psrc.cpp

del *.exp *.obj
