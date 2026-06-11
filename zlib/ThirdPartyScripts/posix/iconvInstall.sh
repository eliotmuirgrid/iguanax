if [ ! -d ~/ThirdParty ]
then
mkdir -p ~/ThirdParty/installed
fi

if test -f ~/ThirdParty/iconvInstall.log; then
rm ~/ThirdParty/iconvInstall.log
fi
logfile=~/ThirdParty/iconvInstall.log

cd ~/ThirdParty

# Download zlib if the folder doesn't exist in ThirdParty
echo Setting up iconv...
if [ ! -d ./iconv ]
then
mkdir iconv
curl https://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.16.tar.gz -o tmp.tar.gz >> $logfile 2>&1
tar -xf tmp.tar.gz -C iconv --strip-components=1
rm tmp.tar.gz
fi

# Make the installed directory (we delete the folder if it exists so the build output is always clean)
if [ -d  ~/ThirdParty/installed/iconv ]
then
rm -rf ~/ThirdParty/installed/iconv
fi
mkdir ~/ThirdParty/installed/iconv
cd iconv

# Configure the build, if command returns a non-zero exit code (failure) then exit script
./configure --prefix="$HOME/ThirdParty/installed/iconv" --exec-prefix="$HOME/ThirdParty/installed/iconv" \
--enable-static=yes --disable-shared >> $logfile 2>&1
if [ $? -ne 0 ]
then
echo iconv configure failed
exit 1
fi

make >> $logfile 2>&1
if [ $? -ne 0 ]
then
echo Building iconv failed
exit 1
fi

make install >> $logfile 2>&1
if [ $? -ne 0 ]
then
echo Installing iconv failed
exit 1
fi
echo iconv ready.

cd ~/ThirdParty