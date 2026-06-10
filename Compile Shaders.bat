@echo off

set SRC=shaders

set SC=glslc
set SFLAG=--target-env=vulkan1.3 --target-spv=spv1.3 -O

del /f %SRC%\*.spv

for %%f in (%SRC%\vertex_*.glsl) do (
	echo %%f
	%SC% %SFLAG% -x glsl -fshader-stage=vertex -o "%%f.spv" "%%f"
)

for %%f in (%SRC%\fragment_*.glsl) do (
	echo %%f
	%SC% %SFLAG% -x glsl -fshader-stage=fragment -o "%%f.spv" "%%f"
)

for %%f in (%SRC%\compute_*.glsl) do (
	echo %%f
	%SC% %SFLAG% -x glsl -fshader-stage=compute -o "%%f.spv" "%%f"
)

pause
