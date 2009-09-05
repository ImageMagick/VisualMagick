@echo off
set srcdir=..\..\..\Magick++\tests\
set bindir=..\..\bin\
set PATH=%bindir%;%PATH%

echo exceptions ...
%bindir%exceptions.exe

echo appendImages ...
%bindir%appendImages.exe

echo attributes ...
%bindir%attributes.exe

echo averageImages ...
%bindir%averageImages.exe

echo coaleseImages ...
%bindir%coalesceImages.exe

echo coderInfo ...
%bindir%coderInfo.exe

echo color ...
%bindir%color.exe

echo montageImages ...
%bindir%montageImages.exe

echo morphImages ...
%bindir%morphImages.exe

echo readWriteBlob ...
%bindir%readWriteBlob.exe

echo readWriteImages ...
%bindir%readWriteImages.exe
