rm -rf build 
mkdir -p build
build-wrapper-win-x86-64.exe --out-dir wrapper bash -c  "cmake -B build -S . && cmake --build build"
sonar-scanner.bat --define sonar.cfamily.build-wrapper-output=wrapper