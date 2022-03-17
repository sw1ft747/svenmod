IF EXIST "Release" (
	IF EXIST "Release\common.lib" (
		move %~dp0\Release\common.lib %~dp0\lib\public\
		exit
	) ELSE (
		echo "File 'Release/common.lib' doesn't exist"
	)
) ELSE (
	echo "Folder 'Release' doesn't exist"
)