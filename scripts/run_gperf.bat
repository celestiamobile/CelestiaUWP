SET SolutionDir=%~1

SET CELESTIA_REPO_ROOT=%SolutionDir%\..\Celestia

cd "%CELESTIA_REPO_ROOT%\src\celscript\legacy"
"%SolutionDir%\tools\gperf.exe" commands.gperf -m4 --output-file=commands.inc

cd "%CELESTIA_REPO_ROOT%\src\celephem"
"%SolutionDir%\tools\gperf.exe" customrotation.gperf -m4 --output-file=customrotation.inc
"%SolutionDir%\tools\gperf.exe" customorbit.gperf -m4 --output-file=customorbit.inc

cd "%CELESTIA_REPO_ROOT%\src\celengine"
"%SolutionDir%\tools\gperf.exe" location.gperf -m4 --output-file=location.inc
"%SolutionDir%\tools\gperf.exe" solarsys.gperf -m4 --output-file=solarsys.inc