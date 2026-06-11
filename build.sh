#/bin/sh
echo "Starting build."
echo Logging to $1 
git clean -dfx
cd build_tool2 && make -j 
./build_tool2 --target dmg_installer --logs $1
echo Trying to copy the installer. 
cp ../dmg_installer/iguanax.dmg $1/iguanax.dmg
echo Hopefully copied the installer.
