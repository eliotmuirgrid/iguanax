if [ ! -d ~/ThirdParty ]
then
mkdir -p ~/ThirdParty/installed
fi

if test -f ~/ThirdParty/opensslInstall.log; then
rm ~/ThirdParty/opensslInstall.log
fi
logfile=~/ThirdParty/opensslInstall.log

cd ~/ThirdParty

# Clone openssl checkout to desired version, if repo cloned already remove all untracked files to reset to a clean state
echo Setting up OpenSSL...
if [ ! -d ./openssl ]
then 
git clone https://github.com/openssl/openssl.git >> $logfile 2>&1
cd openssl
git checkout OpenSSL_1_1_1i >> $logfile 2>&1
else
cd openssl
git clean -dfx >> $logfile 2>&1
fi

if [ -d ~/ThirdParty/installed/openssl ]
then
rm -rf ~/ThirdParty/installed/openssl
fi
mkdir ~/ThirdParty/installed/openssl

# Build static  openssl linked against zlib to enable compression
if [[ "$OSTYPE" == "linux-gnu"* ]];
then
./Configure linux-generic64 --prefix="$HOME/ThirdParty/installed/openssl" \
   --openssldir="$HOME/ThirdParty/installed/openssl" \
   enable-crypto-mdebug no-shared zlib="~/ThirdParty/installed/zlib/lib/zlib.a" \
   >> $logfile 2>&1
else
./Configure darwin64-x86_64-cc --prefix="$HOME/ThirdParty/installed/openssl" \
   --openssldir="$HOME/ThirdParty/installed/openssl" \
   enable-crypto-mdebug no-shared zlib="~/ThirdParty/installed/zlib/lib/zlib.a" \
   >> $logfile 2>&1
fi

if [ $? -ne 0 ]
then
echo Configuring OpenSSL failed
exit 1
fi

make >> $logfile 2>&1
if [ $? -ne 0 ]
then
echo Building OpenSSL failed
exit 1
fi

make install >> $logfile 2>&1
if [ $? -ne 0 ]
then
echo Installing OpenSSL failed
exit 1
fi
echo OpenSSL ready.

cd ~/ThirdParty