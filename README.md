What's the fastest way to resize and pad an animated gif to webp with libvips?
Let's find out

Install:
```
conan install . --output-folder=build --build=missing`
pushd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release`
popd build
```

Compile:
```
pushd build
cmake --build . --config Release
popd build
```
