@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

SET SolutionDir=%~1
SET CELESTIA_CONTENT_REPO_ROOT=%SolutionDir%\..\CelestiaContent
CD "%CELESTIA_CONTENT_REPO_ROOT%\textures"

ECHO Converting to medres
CALL :DOWNSIZE_TEXTURE "medres" "2048"
ECHO Converting to lores
CALL :DOWNSIZE_TEXTURE "lores" "1024"
GOTO :EOF

:DOWNSIZE_TEXTURE
FOR /F "delims=" %%D IN ('DIR /B /S "hires\*.jpg" "hires\*.png" "hires\*.dds"') DO (
    for /f "delims=" %%i in ('magick identify -format "%%A" "%%D"') do (
        set "ALPHA=%%i"
    )

    set "HAS_NO_ALPHA=false"
    if /i "!ALPHA!"=="False" set "HAS_NO_ALPHA=true"
    if /i "!ALPHA!"=="Undefined" set "HAS_NO_ALPHA=true"

    if "!HAS_NO_ALPHA!"=="true" (
        ECHO Converting texture with no alpha channel
        magick "%%D" -resize "%~2x%~2" -define png:exclude-chunks=date,time "%~1\%%~nxD"
    ) else (
        ECHO Converting texture with alpha channel
        magick "%%D" ^( +clone -alpha extract ^) -alpha off -resize "%~2x%~2" -compose CopyOpacity -composite -define png:exclude-chunks=date,time "%~1\%%~nxD"
    )

    IF ERRORLEVEL 1 (
        ECHO Failed to convert: %%D
        EXIT /B 1
    ) ELSE (
        ECHO Successfully converted: %%D
    )
)
GOTO :EOF