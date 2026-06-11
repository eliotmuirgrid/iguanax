if [ ! -d ~/ThirdParty ]
then
mkdir -p ~/ThirdParty/installed
fi

if test -f ~/ThirdParty/gitInstall.log; then
rm ~/ThirdParty/gitInstall.log
fi
logfile=~/ThirdParty/gitInstall.log

cd ~/ThirdParty 

# Clone, checkout, and install git
echo setting up git...
if [ ! -d ./git ]
then 
git clone https://github.com/git/git.git >> $logfile 2>&1
cd git
git checkout v2.30.0 >> $logfile 2>&1
else
cd git
git clean -dfx >> $logfile 2>&1
fi

if [ -d ~/ThirdParty/installed/git ]
then
rm -rf ~/ThirdParty/installed/git
fi
mkdir ~/ThirdParty/installed/git

make configure >> $logfile 2>&1
if [ $? -ne 0 ]
then
echo Building configure failed
exit 1
fi

if [[ "$OSTYPE" == "linux-gnu"* ]];
then
./configure --prefix=$HOME/ThirdParty/installed/git --with-openssl=$HOME/ThirdParty/installed/openssl \
   --with-curl=$HOME/ThirdParty/installed/curl --with-libpcre1=$HOME/ThirdParty/installed/pcre \
   --with-iconv=$HOME/ThirdParty/installed/iconv --with-zlib=$HOME/ThirdParty/installed/zlib \
   LDFLAGS="-L /usr/lib64/ -ldl" >> $logfile 2>&1
else
./configure --prefix=$HOME/ThirdParty/installed/git --with-openssl=$HOME/ThirdParty/installed/openssl \
   --with-curl=$HOME/ThirdParty/installed/curl --with-libpcre1=$HOME/ThirdParty/installed/pcre \
   --with-iconv=$HOME/ThirdParty/installed/iconv --with-zlib=$HOME/ThirdParty/installed/zlib >> $logfile 2>&1
fi
if [ $? -ne 0 ]
then
echo Configuring git failed
exit 1
fi

RUNTIME_PREFIX=1 make >> $logfile 2>&1
if [ $? -ne 0 ]
then
echo Building git failed
exit 1
fi

make install >> $logfile 2>&1
if [ $? -ne 0 ]
then
echo Building git failed
exit 1
fi

echo git ready.