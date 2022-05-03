IF NOT EXIST "build" (
	mkdir build
)

cd build

cmake .. -A Win32
cmake --build . --config Release

pause