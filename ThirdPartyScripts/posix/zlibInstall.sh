if [ ! -d ~/ThirdParty ]
then
mkdir -p ~/ThirdParty/installed
fi

if test -f ~/ThirdParty/zlibInstall.log; then
rm ~/ThirdParty/zlibInstall.log
fi
logfile=~/ThirdParty/zlibInstall.log

cd ~/ThirdParty

# Download zlib if the folder doesn't exist in ThirdParty
echo Setting up zlib...
if [ ! -d ./zlib ]
then
mkdir zlib
curl http://www.zlib.net/zlib-1.2.11.tar.gz -o tmp.tar.gz >> $logfile 2>&1
tar -xf tmp.tar.gz -C zlib --strip-components=1
rm tmp.tar.gz
fi

# Make the installed directory (we delete the folder if it exists so the build output is always clean)
if [ -d  ~/ThirdParty/installed/zlib ]
then
rm -rf ~/ThirdParty/installed/zlib
fi
mkdir ~/ThirdParty/installed/zlib
cd zlib
mkdir build
cd build

# Configure the build, if command returns a non-zero exit code (failure) then exit script
cmake .. -DCMAKE_INSTALL_PREFIX="~/ThirdParty/installed/zlib"  >> $logfile 2>&1
if [ $? -ne 0 ]
then
echo zlib configure failed
exit 1
fi

# build zlib exit on failure
cmake --build . --config Release --target install  >> $logfile 2>&1
if [ $? -ne 0 ]
then
echo zlib configure failed
exit 1
fi
echo zlib ready.