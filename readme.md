# SDL Gpu Template

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)
![Vcpkg](https://img.shields.io/badge/VCPKG-yellow?style=for-the-badge)

<img src="screenshot.png" width="50%" />

Temporary boilerplate code, and library setup using cmake and vcpkg

## Getting Started

### Requirements:
- cmake install
- vcpkg install

### Steps

#### 1. Recursively clone this repository for submodule initialization 

```shell
git clone --recursive https://github.com/tadashibashi/sdlgpu-template
```

#### 2. Initialize vcpkg
Mac & Linux
```
./lib/vcpkg/bootstrap-vcpkg.sh
```

On Windows, use the .bat of the same file

#### 2. Run CMake with VCPKG toolchain file

```shell
# create build folder
mkdir build

# configure build files to build folder, run vcpkg dependency installation
cmake build -S . -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake -DVCPKG_FEATURE_FLAGS="--manifests"
```

Notes: 
- In CLion, you'll just need to add the `-D` flags above to the CMake options in the CMake profile settings.
- \<vcpkg-root\> should be replaced by the actual path to the vcpkg repo on your system

#### 3. Build the project

```shell
cmake --build build -t sdlgpu-template
```

If all goes well, when you run the built project file in the build folder via `./build/sdlgpu-template`, the above screen should appear!


## How to use

1. Edit the vcpkg.json file to change library dependencies that exist on vcpkg.io

2. In CMakeLists.txt you can change the project() title to your own / the usual CMake kung-fu etc.

3. In the same file, add the corresponding `find_package` and `target_link_library` settings or 
whichever recommended code appears in the CMake log during the automated call to vcpkg install.
