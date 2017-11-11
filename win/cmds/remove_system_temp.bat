@echo off 
rem 将“我的文档”移动到D盘下的ccc文件夹，然后删除“我的文档” 
set "path=d:\ccc" 
md "%ljpath%" 
xcopy /e /q /h /i /c /y "C:\Documents and Settings\Administrator\My Documents" "%ljpath%\" 
rd /s /q "C:\Documents and Settings\Administrator\My Documents" 

rem 修改注册表 
echo 开始修改注册表， 
reg add "hkcu\software\microsoft\windows\currentversion\explorer\user shell folders" /v personal /t reg_expand_sz /d "%ljpath%" /f 
reg add "hkcu\software\microsoft\windows\currentversion\explorer\user shell folders" /v "My Music" /t reg_expand_sz /d "%ljpath%\My Music" /f 
reg add "hkcu\software\microsoft\windows\currentversion\explorer\user shell folders" /v "My Pictures" /t reg_expand_sz /d "%ljpath%\My Pictures" /f 
reg add "hkcu\software\microsoft\windows\currentversion\explorer\user shell folders" /v "My Video" /t reg_expand_sz /d "%ljpath%\My Video" /f 
reg add "hkcu\software\microsoft\windows\currentversion\explorer\user shell folders" /v Templates /t reg_expand_sz /d "%ljpath%\temp\Templates" /f 
reg add "hkcu\software\microsoft\windows\currentversion\explorer\user shell folders" /v Cache /t reg_expand_sz /d "%ljpath%\temp\Temporary Internet Files" /f 
reg add "hkcu\software\microsoft\windows\currentversion\explorer\user shell folders" /v Cookies /t reg_expand_sz /d "%ljpath%\temp\Cookies" /f 

reg add "hkcu\software\microsoft\windows\currentversion\explorer\Shell Folders" /v personal /t reg_sz /d "%ljpath%" /f 
reg add "hkcu\software\microsoft\windows\currentversion\explorer\Shell Folders" /v "My Music" /t reg_sz /d "%ljpath%\My Music" /f 
reg add "hkcu\software\microsoft\windows\currentversion\explorer\Shell Folders" /v "My Pictures" /t reg_sz /d "%ljpath%\My Pictures" /f 
reg add "hkcu\software\microsoft\windows\currentversion\explorer\Shell Folders" /v "My Video" /t reg_sz /d "%ljpath%\My Video" /f 
reg add "hkcu\software\microsoft\windows\currentversion\explorer\shell folders" /v Templates /t reg_sz /d "%ljpath%\temp\Templates" /f 
reg add "hkcu\software\microsoft\windows\currentversion\explorer\shell folders" /v Cache /t reg_sz /d "%ljpath%\temp\Temporary Internet Files" /f 
reg add "hkcu\software\microsoft\windows\currentversion\explorer\shell folders" /v Cookies /t reg_sz /d "%ljpath%\temp\Cookies" /f 

echo 注册表修改成功! 

rem 修改临时文件夹 
echo 修改临时文件夹 
reg add "hkcu\Environment" /v TEMP /t reg_expand_sz /d "%ljpath%\temp" /f 
reg add "hkcu\Environment" /v TMP /t reg_expand_sz /d "%ljpath%\temp" /f 

reg add "hklm\SYSTEM\ControlSet001\Control\Session Manager\Environment" /v TEMP /t reg_expand_sz /d "%ljpath%\temp" /f 
reg add "hklm\SYSTEM\ControlSet001\Control\Session Manager\Environment" /v TMP /t reg_expand_sz /d "%ljpath%\temp" /f 

reg add "hklm\SYSTEM\ControlSet002\Control\Session Manager\Environment" /v TEMP /t reg_expand_sz /d "%ljpath%\temp" /f 
reg add "hklm\SYSTEM\ControlSet002\Control\Session Manager\Environment" /v TMP /t reg_expand_sz /d "%ljpath%\temp" /f 

reg add "hklm\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v TEMP /t reg_expand_sz /d "%ljpath%\temp" /f 
reg add "hklm\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v TMP /t reg_expand_sz /d "%ljpath%\temp" /f 
echo 临时文件夹批处理修改成功 

rem 修改IE临时文件夹 
echo 修改IE临时文件夹 
reg add "hklm\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Cache\Special Paths\Cookies" /v Directory /t reg_expand_sz /d "%ljpath%\temp\Cookies" /f 
reg add "hklm\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Cache\Paths" /v Directory /t reg_expand_sz /d "%ljpath%\temp\Temporary Internet Files" /f 
reg add "hklm\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Cache\Paths\Path1" /v CachePath /t reg_sz /d "%ljpath%\temp\Temporary Internet Files\Content.IE5\Cache1" /f 
reg add "hklm\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Cache\Paths\Path2" /v CachePath /t reg_sz /d "%ljpath%\temp\Temporary Internet Files\Content.IE5\Cache2" /f 
reg add "hklm\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Cache\Paths\Path3" /v CachePath /t reg_sz /d "%ljpath%\temp\Temporary Internet Files\Content.IE5\Cache3" /f 
reg add "hklm\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Cache\Paths\Path4" /v CachePath /t reg_sz /d "%ljpath%\temp\Temporary Internet Files\Content.IE5\Cache4" /f 
echo IE临时文件夹处理完成 
echo 按任意键退出 
pause>nul 
exit 