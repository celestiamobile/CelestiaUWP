SET SolutionDir=%~1
SET OutDir=%~2

SET CELESTIA_TEMP_DIR=%TEMP%\celestia
IF NOT EXIST %CELESTIA_TEMP_DIR% MKDIR %CELESTIA_TEMP_DIR%

SET EXCLUDE_FILE=%CELESTIA_TEMP_DIR%\EXCLUDE.txt
echo CMakeLists.txt>>"%EXCLUDE_FILE%"
echo well-known-dsonames.txt>>"%EXCLUDE_FILE%"
echo well-known-starnames.txt>>"%EXCLUDE_FILE%"

SET CELESTIA_ROOT=%OutDir%\CelestiaResources
SET CELESTIA_REPO_ROOT=%SolutionDir%\..\Celestia

IF NOT EXIST "%CELESTIA_ROOT%" MKDIR "%CELESTIA_ROOT%"

SET CELESTIA_CONTENT_REPO_ROOT=%SolutionDir%\..\CelestiaContent

FOR %%D in (images locale scripts shaders) DO (
	XCOPY "%CELESTIA_REPO_ROOT%\%%D" "%CELESTIA_ROOT%\%%D" /E /I /Y /EXCLUDE:%EXCLUDE_FILE%
)

FOR %%D in (data extras extras-standard models textures warp) DO (
	XCOPY "%CELESTIA_CONTENT_REPO_ROOT%\%%D" "%CELESTIA_ROOT%\%%D" /E /I /Y /EXCLUDE:%EXCLUDE_FILE%
)

FOR %%D in (fonts) DO (
	XCOPY "%SolutionDir%\%%D" "%CELESTIA_ROOT%\%%D" /E /I /Y /EXCLUDE:%EXCLUDE_FILE%
)

FOR %%D in (controls.txt demo.cel guide.cel start.cel COPYING AUTHORS TRANSLATORS) DO (
	XCOPY "%CELESTIA_REPO_ROOT%\%%D" "%CELESTIA_ROOT%" /Y
)

powershell -Command "(Get-Content '%CELESTIA_REPO_ROOT%\celestia.cfg.in') -replace '@CELCFG_EXTRAS_DIRS@', '' | Set-Content '%CELESTIA_ROOT%\celestia.cfg'"

RD "%CELESTIA_TEMP_DIR%" /S /Q
