if [ ! -d ~/ThirdParty ]
then
mkdir -p ~/ThirdParty/installed
fi

if test -f ~/ThirdParty/ssh2Install.log; then
rm ~/ThirdParty/ssh2Install.log
fi
logfile=~/ThirdParty/ssh2Install.log
echo "Logging to " $logfile

cd ~/ThirdParty

# Clone, checkout, and install libssh2
echo Setting up libssh2...
if [ ! -d ./libssh2 ]
then
git clone https://github.com/libssh2/libssh2.git >> $logfile 2>&1
cd libssh2
git checkout libssh2-1.9.0 >> $logfile 2>&1
else
cd libssh2
git clean -dfx >> $logfile 2>&1
fi
if [ -d ~/ThirdParty/installed/libssh2 ]
then
rm -rf ~/ThirdParty/installed/libssh2
fi
mkdir ~/ThirdParty/installed/libssh2

mkdir build
cd build

cmake .. -DCMAKE_INSTALL_PREFIX="~/ThirdParty/installed/libssh2" -DCMAKE_PREFIX_PATH="~/ThirdParty/installed/openssl" \
   -DCRYPTO_BACKEND=OpenSSL -DCMAKE_INSTALL_PREFIX="~/ThirdParty/installed/libssh2" -DBUILD_EXAMPLES=NO -DBUILD_TESTING=YES \
   -DENABLE_ZLIB_COMPRESSION=YES -DZLIB_LIBRARY="$HOME/ThirdParty/installed/zlib/lib/libz.a" >> $logfile 2>&1
if [ $? -ne 0 ]
then
echo Configuring libssh2 failed
exit 1
fi

cmake --build . --config Release --target install >> $logfile 2>&1
if [ $? -ne 0 ]
then
echo Building libssh2 failed
exit 1
fi

# This is necessary on CentOS or else curl cannot find libssh2's libraries to link against
if [[ "$OSTYPE" == "linux-gnu"* ]];
then 
cp -R ~/ThirdParty/installed/libssh2/lib64/* ~/ThirdParty/installed/libssh2/lib/
fi

echo libssh2 ready.
