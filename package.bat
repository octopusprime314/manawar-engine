@echo off

mkdir 4fps-dist
pushd 4fps-dist
mkdir shading
popd

cmake --build _build --config Release --target install -- /verbosity:m || goto :EOF

cp -v ./libs/freeimage/lib/FreeImage.dll   4fps-dist/bin/
cp -v ./libs/fmod/lowlevel/lib/fmod64.dll  4fps-dist/bin/
cp -v ./libs/fmod/lowlevel/lib/fmodL64.dll 4fps-dist/bin/
echo Copying shaders...
cp -R shading/shaders                      4fps-dist/shading/
echo Copying assets...
cp -R assets/                              4fps-dist/
