IF EXIST "Release" (
	IF EXIST "Release\interface.lib" (
		move %~dp0\Release\interface.lib %~dp0\lib\public\
		exit
	) ELSE (
		echo "File 'Release/interface.lib' doesn't exist"
	)
) ELSE (
	echo "Folder 'Release' doesn't exist"
)