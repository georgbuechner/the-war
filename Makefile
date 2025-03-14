install: 
	export VCPKG_ROOT="vcpkg" && cmake --preset=default && cmake --build build

setup: 
	export VCPKG_DISABLE_METRICS && ./vcpkg/bootstrap-vcpkg.sh 

compilecommands: 
	export VCPKG_ROOT="vcpkg" && cmake --preset=default -DCMAKE_EXPORT_COMPILE_COMMANDS=1 && cmake --build build
