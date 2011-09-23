@echo off
set TESTS=0
set TYPE=NONE

if not {%1}=={} (
  (if {%1}=={all} goto :_testall)
  if not {%2}=={} (
    call :_dorwtest %2 %1 & goto :_exit
  ) else (
    call :_dorwtest both %1 & goto :_exit
  )
)
@echo to run all tests:
@echo    runtest all
@echo to run a single test:
@echo    runtest XXX were XXX is a format like BMP, TIFF, or JPEG
@echo to run a subset of a single test:
@echo    runtest XXX YYY were YYY is none, file, blob, or both
goto :_exit

:_testall
call :_dorwtest both AVS
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both BMP
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both CMYK
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both DIB
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both DPX
if not %COUNT% EQU 6 goto :_failed
:call :_dorwtest both EPDF
:if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both EPSF
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both EPSI
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both EPI
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both EPS
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both EPT
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both FAX
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both FITS
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both GIF
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both GRAY
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both ICB
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both JBIG
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both JP2
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both JPEG
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both JNG
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both MIFF
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both MNG
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both MONO
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both MTV
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both P7
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both PBM
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both PCD
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both PCDS
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both PCX
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both PGM
if not %COUNT% EQU 6 goto :_failed
:call :_dorwtest both PICON
:if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both PICT
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both PNG
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both PNM
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both PPM
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both PS
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both PSD
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both PTIF
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both RAS
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both RGB
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both RGBA
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both SGI
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both SUN
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both TGA
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both TIFF
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both UYVY
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both VDA
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both VICAR
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both VIFF
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both VST
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both WBMP
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both XBM
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest both XPM
if not %COUNT% EQU 6 goto :_failed
call :_dorwtest none XWD
if not %COUNT% EQU 0 goto :_failed
call :_dorwtest both YUV
if not %COUNT% EQU 6 goto :_failed
echo %TESTS% tests completed!
goto :EOF

:_dorwtest
set COUNT=0
set TYPE=%2
set /a TESTS += 1
if {%1}=={none} goto :EOF
if {%1}=={blob} goto :_doblob
if {%1}=={file} goto :_dofile
:_dofile
..\bin\rwfile ..\..\PerlMagick\t\input_p4.pbm %TYPE%
if not %ERRORLEVEL% EQU 0 goto :_problem
set /a COUNT += 1
..\bin\rwfile ..\..\PerlMagick\t\input_p5.pgm %TYPE%
if not %ERRORLEVEL% EQU 0 goto :_problem
set /a COUNT += 1
..\bin\rwfile ..\..\PerlMagick\t\input_p6.ppm %TYPE%
if not %ERRORLEVEL% EQU 0 goto :_problem
set /a COUNT += 1
if not {%1}=={both} goto :EOF
:_doblob
..\bin\rwblob ..\..\PerlMagick\t\input_p4.pbm %TYPE%
if not %ERRORLEVEL% EQU 0 goto :_problem
set /a COUNT += 1
..\bin\rwblob ..\..\PerlMagick\t\input_p5.pgm %TYPE%
if not %ERRORLEVEL% EQU 0 goto :_problem
set /a COUNT += 1
..\bin\rwblob ..\..\PerlMagick\t\input_p6.ppm %TYPE%
if not %ERRORLEVEL% EQU 0 goto :_problem
set /a COUNT += 1
goto :EOF
:_problem
@echo Test %COUNT% on type %TYPE% failed
goto :EOF

:_failed
@echo Test failed on test %TESTS% type %TYPE%!
:_exit
popd

