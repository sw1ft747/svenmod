IF EXIST "Release" (
	IF EXIST "Release\svenmod.lib" (
		move %~dp0\Release\svenmod.lib %~dp0\lib\public\
		exit
	) ELSE (
		echo "File 'Release/svenmod.lib' doesn't exist"
	)
) ELSE (
	echo "Folder 'Release' doesn't exist"
)