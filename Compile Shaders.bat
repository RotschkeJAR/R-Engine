@echo off

set SH=shaders

set SC=glslc
set SFLAG=--target-env=vulkan1.3 --target-spv=spv1.6 -O

set VERT_GL=%SH%\vertex.glsl
set FRAG_GL=%SH%\fragment.glsl
set VERT_HL=%SH%\vertex.hlsl
set FRAG_HL=%SH%\fragment.hlsl

del /f %SH%\*.spv

if exist %VERT_GL% (
	%SC% %SFLAG% -x glsl -fshader-stage=vertex -o %SH%\vertex.spv %VERT_GL%
) else if exist %VERT_HL% (
	%SC% %SFLAG% -x hlsl -fshader-stage=vertex -o %SH%\vertex.spv %VERT_HL%
) else (
	echo Failed finding vertex shader source code!
)

if exist %FRAG_GL% (
	%SC% %SFLAG% -x glsl -fshader-stage=fragment -o %SH%\fragment.spv %FRAG_GL%
) else if exist %FRAG_HL% (
	%SC% %SFLAG% -x hlsl -fshader-stage=fragment -o %SH%\fragment.spv %FRAG_HL%
) else (
	echo Failed finding fragment shader source code!
)

pause
