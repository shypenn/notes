@echo off 
rem �����ҵ��ĵ����ƶ���D���µ�ccc�ļ��У�Ȼ��ɾ�����ҵ��ĵ��� 
set "path=d:\ccc" 
md "%ljpath%" 
xcopy /e /q /h /i /c /y "C:\Documents and Settings\Administrator\My Documents" "%ljpath%\" 
rd /s /q "C:\Documents and Settings\Administrator\My Documents" 

rem �޸�ע��� 
echo ��ʼ�޸�ע��� 
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

echo ע����޸ĳɹ�! 

rem �޸���ʱ�ļ��� 
echo �޸���ʱ�ļ��� 
reg add "hkcu\Environment" /v TEMP /t reg_expand_sz /d "%ljpath%\temp" /f 
reg add "hkcu\Environment" /v TMP /t reg_expand_sz /d "%ljpath%\temp" /f 

reg add "hklm\SYSTEM\ControlSet001\Control\Session Manager\Environment" /v TEMP /t reg_expand_sz /d "%ljpath%\temp" /f 
reg add "hklm\SYSTEM\ControlSet001\Control\Session Manager\Environment" /v TMP /t reg_expand_sz /d "%ljpath%\temp" /f 

reg add "hklm\SYSTEM\ControlSet002\Control\Session Manager\Environment" /v TEMP /t reg_expand_sz /d "%ljpath%\temp" /f 
reg add "hklm\SYSTEM\ControlSet002\Control\Session Manager\Environment" /v TMP /t reg_expand_sz /d "%ljpath%\temp" /f 

reg add "hklm\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v TEMP /t reg_expand_sz /d "%ljpath%\temp" /f 
reg add "hklm\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v TMP /t reg_expand_sz /d "%ljpath%\temp" /f 
echo ��ʱ�ļ����������޸ĳɹ� 

rem �޸�IE��ʱ�ļ��� 
echo �޸�IE��ʱ�ļ��� 
reg add "hklm\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Cache\Special Paths\Cookies" /v Directory /t reg_expand_sz /d "%ljpath%\temp\Cookies" /f 
reg add "hklm\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Cache\Paths" /v Directory /t reg_expand_sz /d "%ljpath%\temp\Temporary Internet Files" /f 
reg add "hklm\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Cache\Paths\Path1" /v CachePath /t reg_sz /d "%ljpath%\temp\Temporary Internet Files\Content.IE5\Cache1" /f 
reg add "hklm\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Cache\Paths\Path2" /v CachePath /t reg_sz /d "%ljpath%\temp\Temporary Internet Files\Content.IE5\Cache2" /f 
reg add "hklm\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Cache\Paths\Path3" /v CachePath /t reg_sz /d "%ljpath%\temp\Temporary Internet Files\Content.IE5\Cache3" /f 
reg add "hklm\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Cache\Paths\Path4" /v CachePath /t reg_sz /d "%ljpath%\temp\Temporary Internet Files\Content.IE5\Cache4" /f 
echo IE��ʱ�ļ��д������ 
echo ��������˳� 
pause>nul 
exit 