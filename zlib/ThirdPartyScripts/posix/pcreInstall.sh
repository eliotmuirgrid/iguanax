if [ ! -d ~/ThirdParty ]
then
mkdir -p ~/ThirdParty/installed
fi

if test -f ~/ThirdParty/pcreInstall.log; then
rm ~/ThirdParty/pcreInstall.log
fi
logfile=~/ThirdParty/pcreInstall.log

cd ~/ThirdParty

# Download, extract, and install pcre
# echo setting up pcre...
# if [ ! -d ./pcre ]
# then 
# mkdir pcre
# curl https://ftp.pcre.org/pub/pcre/pcre-8.36.tar.gz -o tmp.tar.gz >> $logfile 2>&1
# tar -xf tmp.tar.gz -C pcre --strip-components=1
# rm tmp.tar.gz
# fi 

# Read this!
# 1. Downloard pcre.tar.gz 8.36 from https://sourceforge.net/projects/pcre/ 
# 2. Move downloaded file into ~/ThirdParty
# 3. Delete pcre directory if it exists
# 4. Double click the .tar file (open it) and rename the new directory to pcre (it will likely be 'pcre-8.36')

# echo "--------------------------------------------------------------------"
# echo "Please read the comments in ThirdPartyScripts/posix/pcreInstall.sh"
# echo "--------------------------------------------------------------------"
# exit 1

# 5. Comment out the lines above and the script again

if [ -d ~/ThirdParty/installed/pcre ]
then
rm -rf ~/ThirdParty/installed/pcre
fi
mkdir ~/ThirdParty/installed/pcre

cd pcre
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX="~/ThirdParty/installed/pcre" \
-DPCRE_BUILD_TESTS=OFF -DCMAKE_PREFIX_PATH="~/ThirdParty/installed/zlib/" \
-DPCRE_BUILD_PCREGREP=OFF >> $logfile 2>&1
if [ $? -ne 0 ]
then
echo Configuring pcre failed
exit 1
fi

cmake --build . --config Release --target install >> $logfile 2>&1
if [ $? -ne 0 ]
then
echo building pcre failed
exit 1
fi

echo pcre ready.