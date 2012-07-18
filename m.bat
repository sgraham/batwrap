@echo off
cl /nologo /Ox /MD /EHsc /DUNICODE /D_UNICODE batwrap.cc /link shell32.lib && ^
echo Copy batwrap.exe to other_name.exe to make it a wrapper for other_name.bat.
