@echo off
set exedir=..\bin\
set srcdir=..\..\tests\
set events="exception"
set bilevel_file=%srcdir%input_bilevel.miff
set gray_file=%srcdir%input_gray.miff
set pallette_file=%srcdir%input_pallette.miff
set truecolor_file=%srcdir%input_truecolor.miff
@echo on
%exedir%drawtest drawtest_out.miff
%exedir%drawtest drawtest_out.mvg
%exedir%drawtest drawtest_out.svg
%exedir%rwfile.exe -debug %events% %bilevel_file% AVS 
%exedir%rwfile.exe -debug %events% %gray_file% AVS 
%exedir%rwfile.exe -debug %events% %pallette_file% AVS 
%exedir%rwfile.exe -debug %events% %truecolor_file% AVS 
%exedir%rwfile.exe -debug %events% %bilevel_file% BMP 
%exedir%rwfile.exe -debug %events% %gray_file% BMP 
%exedir%rwfile.exe -debug %events% %pallette_file% BMP 
%exedir%rwfile.exe -debug %events% %truecolor_file% BMP 
%exedir%rwfile.exe -debug %events% %bilevel_file% DIB 
%exedir%rwfile.exe -debug %events% %gray_file% DIB 
%exedir%rwfile.exe -debug %events% %pallette_file% DIB 
%exedir%rwfile.exe -debug %events% %truecolor_file% DIB 
%exedir%rwfile.exe -debug %events% %bilevel_file% DPX 
%exedir%rwfile.exe -debug %events% %gray_file% DPX 
%exedir%rwfile.exe -debug %events% %pallette_file% DPX 
%exedir%rwfile.exe -debug %events% %truecolor_file% DPX 
%exedir%rwfile.exe -debug %events% %bilevel_file% EPDF 
%exedir%rwfile.exe -debug %events% %gray_file% EPDF 
%exedir%rwfile.exe -debug %events% %pallette_file% EPDF 
%exedir%rwfile.exe -debug %events% %truecolor_file% EPDF 
%exedir%rwfile.exe -debug %events% %bilevel_file% EPSF 
%exedir%rwfile.exe -debug %events% %gray_file% EPSF 
%exedir%rwfile.exe -debug %events% %pallette_file% EPSF 
%exedir%rwfile.exe -debug %events% %truecolor_file% EPSF 
%exedir%rwfile.exe -debug %events% %bilevel_file% EPSI 
%exedir%rwfile.exe -debug %events% %gray_file% EPSI 
%exedir%rwfile.exe -debug %events% %pallette_file% EPSI 
%exedir%rwfile.exe -debug %events% %truecolor_file% EPSI 
%exedir%rwfile.exe -debug %events% %bilevel_file% EPI 
%exedir%rwfile.exe -debug %events% %gray_file% EPI 
%exedir%rwfile.exe -debug %events% %pallette_file% EPI 
%exedir%rwfile.exe -debug %events% %truecolor_file% EPI 
%exedir%rwfile.exe -debug %events% %bilevel_file% EPS 
%exedir%rwfile.exe -debug %events% %gray_file% EPS 
%exedir%rwfile.exe -debug %events% %pallette_file% EPS 
%exedir%rwfile.exe -debug %events% %truecolor_file% EPS 
%exedir%rwfile.exe -debug %events% %bilevel_file% EPT 
%exedir%rwfile.exe -debug %events% %gray_file% EPT 
%exedir%rwfile.exe -debug %events% %pallette_file% EPT 
%exedir%rwfile.exe -debug %events% %truecolor_file% EPT 
%exedir%rwfile.exe -debug %events% %bilevel_file% FAX 
%exedir%rwfile.exe -debug %events% %gray_file% FAX 
%exedir%rwfile.exe -debug %events% %pallette_file% FAX 
%exedir%rwfile.exe -debug %events% %truecolor_file% FAX 
%exedir%rwfile.exe -debug %events% %bilevel_file% FITS 
%exedir%rwfile.exe -debug %events% %gray_file% FITS 
%exedir%rwfile.exe -debug %events% %pallette_file% FITS 
%exedir%rwfile.exe -debug %events% %truecolor_file% FITS 
%exedir%rwfile.exe -debug %events% %bilevel_file% GIF 
%exedir%rwfile.exe -debug %events% %gray_file% GIF 
%exedir%rwfile.exe -debug %events% %pallette_file% GIF 
%exedir%rwfile.exe -debug %events% %truecolor_file% GIF 
%exedir%rwfile.exe -debug %events% %bilevel_file% GRAY 
%exedir%rwfile.exe -debug %events% %gray_file% GRAY 
%exedir%rwfile.exe -debug %events% %pallette_file% GRAY 
%exedir%rwfile.exe -debug %events% %truecolor_file% GRAY 
%exedir%rwfile.exe -debug %events% %bilevel_file% ICB 
%exedir%rwfile.exe -debug %events% %gray_file% ICB 
%exedir%rwfile.exe -debug %events% %pallette_file% ICB 
%exedir%rwfile.exe -debug %events% %truecolor_file% ICB 
%exedir%rwfile.exe -debug %events% %bilevel_file% JBIG 
%exedir%rwfile.exe -debug %events% %gray_file% JBIG 
%exedir%rwfile.exe -debug %events% %pallette_file% JBIG 
%exedir%rwfile.exe -debug %events% %truecolor_file% JBIG 
%exedir%rwfile.exe -debug %events% %bilevel_file% JPEG 
%exedir%rwfile.exe -debug %events% %gray_file% JPEG 
%exedir%rwfile.exe -debug %events% %pallette_file% JPEG 
%exedir%rwfile.exe -debug %events% %truecolor_file% JPEG 
%exedir%rwfile.exe -debug %events% %bilevel_file% JP2 
%exedir%rwfile.exe -debug %events% %gray_file% JP2 
%exedir%rwfile.exe -debug %events% %pallette_file% JP2 
%exedir%rwfile.exe -debug %events% %truecolor_file% JP2 
%exedir%rwfile.exe -debug %events% %bilevel_file% MIFF 
%exedir%rwfile.exe -debug %events% %gray_file% MIFF 
%exedir%rwfile.exe -debug %events% %pallette_file% MIFF 
%exedir%rwfile.exe -debug %events% %truecolor_file% MIFF 
%exedir%rwfile.exe -debug %events% %bilevel_file% MNG 
%exedir%rwfile.exe -debug %events% %gray_file% MNG 
%exedir%rwfile.exe -debug %events% %pallette_file% MNG 
%exedir%rwfile.exe -debug %events% %truecolor_file% MNG 
%exedir%rwfile.exe -debug %events% %bilevel_file% MTV 
%exedir%rwfile.exe -debug %events% %gray_file% MTV 
%exedir%rwfile.exe -debug %events% %pallette_file% MTV 
%exedir%rwfile.exe -debug %events% %truecolor_file% MTV 
%exedir%rwfile.exe -debug %events% %bilevel_file% P7 
%exedir%rwfile.exe -debug %events% %gray_file% P7 
%exedir%rwfile.exe -debug %events% %pallette_file% P7 
%exedir%rwfile.exe -debug %events% %truecolor_file% P7 
%exedir%rwfile.exe -debug %events% %bilevel_file% PBM 
%exedir%rwfile.exe -debug %events% %gray_file% PBM 
%exedir%rwfile.exe -debug %events% %pallette_file% PBM 
%exedir%rwfile.exe -debug %events% %truecolor_file% PBM 
%exedir%rwfile.exe -debug %events% %bilevel_file% PCD 
%exedir%rwfile.exe -debug %events% %gray_file% PCD 
%exedir%rwfile.exe -debug %events% %pallette_file% PCD 
%exedir%rwfile.exe -debug %events% %truecolor_file% PCD 
%exedir%rwfile.exe -debug %events% %bilevel_file% PCDS 
%exedir%rwfile.exe -debug %events% %gray_file% PCDS 
%exedir%rwfile.exe -debug %events% %pallette_file% PCDS 
%exedir%rwfile.exe -debug %events% %truecolor_file% PCDS 
%exedir%rwfile.exe -debug %events% %bilevel_file% PCX 
%exedir%rwfile.exe -debug %events% %gray_file% PCX 
%exedir%rwfile.exe -debug %events% %pallette_file% PCX 
%exedir%rwfile.exe -debug %events% %truecolor_file% PCX 
%exedir%rwfile.exe -debug %events% %bilevel_file% PGM 
%exedir%rwfile.exe -debug %events% %gray_file% PGM 
%exedir%rwfile.exe -debug %events% %pallette_file% PGM 
%exedir%rwfile.exe -debug %events% %truecolor_file% PGM 
%exedir%rwfile.exe -debug %events% %bilevel_file% PICON 
%exedir%rwfile.exe -debug %events% %gray_file% PICON 
%exedir%rwfile.exe -debug %events% %pallette_file% PICON 
%exedir%rwfile.exe -debug %events% %truecolor_file% PICON 
%exedir%rwfile.exe -debug %events% %bilevel_file% PICT 
%exedir%rwfile.exe -debug %events% %gray_file% PICT 
%exedir%rwfile.exe -debug %events% %pallette_file% PICT 
%exedir%rwfile.exe -debug %events% %truecolor_file% PICT 
%exedir%rwfile.exe -debug %events% %bilevel_file% PNG 
%exedir%rwfile.exe -debug %events% %gray_file% PNG 
%exedir%rwfile.exe -debug %events% %pallette_file% PNG 
%exedir%rwfile.exe -debug %events% %truecolor_file% PNG 
%exedir%rwfile.exe -debug %events% %bilevel_file% PNM 
%exedir%rwfile.exe -debug %events% %gray_file% PNM 
%exedir%rwfile.exe -debug %events% %pallette_file% PNM 
%exedir%rwfile.exe -debug %events% %truecolor_file% PNM 
%exedir%rwfile.exe -debug %events% %bilevel_file% PPM 
%exedir%rwfile.exe -debug %events% %gray_file% PPM 
%exedir%rwfile.exe -debug %events% %pallette_file% PPM 
%exedir%rwfile.exe -debug %events% %truecolor_file% PPM 
%exedir%rwfile.exe -debug %events% %bilevel_file% PS 
%exedir%rwfile.exe -debug %events% %gray_file% PS 
%exedir%rwfile.exe -debug %events% %pallette_file% PS 
%exedir%rwfile.exe -debug %events% %truecolor_file% PS 
%exedir%rwfile.exe -debug %events% %bilevel_file% PSD 
%exedir%rwfile.exe -debug %events% %gray_file% PSD 
%exedir%rwfile.exe -debug %events% %pallette_file% PSD 
%exedir%rwfile.exe -debug %events% %truecolor_file% PSD 
%exedir%rwfile.exe -debug %events% %bilevel_file% RAS 
%exedir%rwfile.exe -debug %events% %gray_file% RAS 
%exedir%rwfile.exe -debug %events% %pallette_file% RAS 
%exedir%rwfile.exe -debug %events% %truecolor_file% RAS 
%exedir%rwfile.exe -debug %events% %bilevel_file% SGI 
%exedir%rwfile.exe -debug %events% %gray_file% SGI 
%exedir%rwfile.exe -debug %events% %pallette_file% SGI 
%exedir%rwfile.exe -debug %events% %truecolor_file% SGI 
%exedir%rwfile.exe -debug %events% %bilevel_file% SUN 
%exedir%rwfile.exe -debug %events% %gray_file% SUN 
%exedir%rwfile.exe -debug %events% %pallette_file% SUN 
%exedir%rwfile.exe -debug %events% %truecolor_file% SUN 
%exedir%rwfile.exe -debug %events% %bilevel_file% TGA 
%exedir%rwfile.exe -debug %events% %gray_file% TGA 
%exedir%rwfile.exe -debug %events% %pallette_file% TGA 
%exedir%rwfile.exe -debug %events% %truecolor_file% TGA 
%exedir%rwfile.exe -debug %events% %bilevel_file% TIFF 
%exedir%rwfile.exe -debug %events% %gray_file% TIFF 
%exedir%rwfile.exe -debug %events% %pallette_file% TIFF 
%exedir%rwfile.exe -debug %events% %truecolor_file% TIFF 
%exedir%rwfile.exe -debug %events% %bilevel_file% VDA 
%exedir%rwfile.exe -debug %events% %gray_file% VDA 
%exedir%rwfile.exe -debug %events% %pallette_file% VDA 
%exedir%rwfile.exe -debug %events% %truecolor_file% VDA 
%exedir%rwfile.exe -debug %events% %bilevel_file% VICAR 
%exedir%rwfile.exe -debug %events% %gray_file% VICAR 
%exedir%rwfile.exe -debug %events% %pallette_file% VICAR 
%exedir%rwfile.exe -debug %events% %truecolor_file% VICAR 
%exedir%rwfile.exe -debug %events% %bilevel_file% VIFF 
%exedir%rwfile.exe -debug %events% %gray_file% VIFF 
%exedir%rwfile.exe -debug %events% %pallette_file% VIFF 
%exedir%rwfile.exe -debug %events% %truecolor_file% VIFF 
%exedir%rwfile.exe -debug %events% %bilevel_file% VST 
%exedir%rwfile.exe -debug %events% %gray_file% VST 
%exedir%rwfile.exe -debug %events% %pallette_file% VST 
%exedir%rwfile.exe -debug %events% %truecolor_file% VST 
%exedir%rwfile.exe -debug %events% %bilevel_file% WBMP 
%exedir%rwfile.exe -debug %events% %gray_file% WBMP 
%exedir%rwfile.exe -debug %events% %pallette_file% WBMP 
%exedir%rwfile.exe -debug %events% %truecolor_file% WBMP 
%exedir%rwfile.exe -debug %events% %bilevel_file% XBM 
%exedir%rwfile.exe -debug %events% %gray_file% XBM 
%exedir%rwfile.exe -debug %events% %pallette_file% XBM 
%exedir%rwfile.exe -debug %events% %truecolor_file% XBM 
%exedir%rwfile.exe -debug %events% %bilevel_file% XPM 
%exedir%rwfile.exe -debug %events% %gray_file% XPM 
%exedir%rwfile.exe -debug %events% %pallette_file% XPM 
%exedir%rwfile.exe -debug %events% %truecolor_file% XPM
: %exedir%rwfile.exe -debug %events% %bilevel_file% XWD 
: %exedir%rwfile.exe -debug %events% %gray_file% XWD 
: %exedir%rwfile.exe -debug %events% %pallette_file% XWD 
: %exedir%rwfile.exe -debug %events% %truecolor_file% XWD
%exedir%rwfile.exe -debug %events% -size 70x46 %bilevel_file% CMYK 
%exedir%rwfile.exe -debug %events% -size 70x46 %gray_file% CMYK 
%exedir%rwfile.exe -debug %events% -size 70x46 %pallette_file% CMYK 
%exedir%rwfile.exe -debug %events% -size 70x46 %truecolor_file% CMYK 
%exedir%rwfile.exe -debug %events% -size 70x46 %bilevel_file% GRAY 
%exedir%rwfile.exe -debug %events% -size 70x46 %gray_file% GRAY 
%exedir%rwfile.exe -debug %events% -size 70x46 %pallette_file% GRAY 
%exedir%rwfile.exe -debug %events% -size 70x46 %truecolor_file% GRAY 
%exedir%rwfile.exe -debug %events% -size 70x46 %bilevel_file% MONO 
%exedir%rwfile.exe -debug %events% -size 70x46 %gray_file% MONO 
%exedir%rwfile.exe -debug %events% -size 70x46 %pallette_file% MONO 
%exedir%rwfile.exe -debug %events% -size 70x46 %truecolor_file% MONO 
%exedir%rwfile.exe -debug %events% -size 70x46 %bilevel_file% PAL 
%exedir%rwfile.exe -debug %events% -size 70x46 %gray_file% PAL 
%exedir%rwfile.exe -debug %events% -size 70x46 %pallette_file% PAL 
%exedir%rwfile.exe -debug %events% -size 70x46 %truecolor_file% PAL 
%exedir%rwfile.exe -debug %events% -size 70x46 %bilevel_file% RGB 
%exedir%rwfile.exe -debug %events% -size 70x46 %gray_file% RGB 
%exedir%rwfile.exe -debug %events% -size 70x46 %pallette_file% RGB 
%exedir%rwfile.exe -debug %events% -size 70x46 %truecolor_file% RGB 
%exedir%rwfile.exe -debug %events% -size 70x46 %bilevel_file% RGBA 
%exedir%rwfile.exe -debug %events% -size 70x46 %gray_file% RGBA 
%exedir%rwfile.exe -debug %events% -size 70x46 %pallette_file% RGBA 
%exedir%rwfile.exe -debug %events% -size 70x46 %truecolor_file% RGBA 
%exedir%rwfile.exe -debug %events% -size 70x46 %bilevel_file% UYVY 
%exedir%rwfile.exe -debug %events% -size 70x46 %gray_file% UYVY 
%exedir%rwfile.exe -debug %events% -size 70x46 %pallette_file% UYVY 
%exedir%rwfile.exe -debug %events% -size 70x46 %truecolor_file% UYVY 
%exedir%rwfile.exe -debug %events% -size 70x46 %bilevel_file% YUV 
%exedir%rwfile.exe -debug %events% -size 70x46 %gray_file% YUV 
%exedir%rwfile.exe -debug %events% -size 70x46 %pallette_file% YUV 
%exedir%rwfile.exe -debug %events% -size 70x46 %truecolor_file% YUV 

