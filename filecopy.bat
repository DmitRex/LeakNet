@echo off
@if exist %2 attrib -r %2
@if exist %2 del %2
@copy %1 %2
:: @if exist %2 attrib +r %2

:: VXP: Well, I've probably messed it up with filecopy_leavewriteable.bat, but I'm not going to change every filecopy.bat calls in all the projects since this is working just fine for now.
