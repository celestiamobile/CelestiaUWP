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

    REM --- Check if filename contains "normal" ---
    ECHO %%~nxD | findstr /I "normal" >NUL
    IF !ERRORLEVEL!==0 (
        ECHO Skipping resize for normal map: %%D
    ) ELSE (
        FOR /F "delims=" %%i IN ('magick identify -format "%%A" "%%D" 2^>NUL') DO (
            SET "ALPHA=%%i"
        )

        SET "HAS_NO_ALPHA=false"
        IF /I "!ALPHA!"=="False" SET "HAS_NO_ALPHA=true"
        IF /I "!ALPHA!"=="Undefined" SET "HAS_NO_ALPHA=true"

        IF "!HAS_NO_ALPHA!"=="true" (
            ECHO Converting texture with no alpha channel
            magick "%%D" -resize "%~2x%~2" -define png:exclude-chunks=date,time "%~1\%%~nxD"
        ) ELSE (
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
)
GOTO :EOF