IF EXIST "Release" (
	IF EXIST "Release\convar.lib" (
		move %~dp0\Release\convar.lib %~dp0\lib\public\
		exit
	) ELSE (
		echo "File 'Release/convar.lib' doesn't exist"
	)
) ELSE (
	echo "Folder 'Release' doesn't exist"
)