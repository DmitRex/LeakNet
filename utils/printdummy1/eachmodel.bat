@echo off
set modelpath=..\..\..\hl2\models_original

for /R %modelpath% %%I IN (*.mdl) DO (
	Debug\printdummy1.exe %%~fI
)

echo All done
pause > nul