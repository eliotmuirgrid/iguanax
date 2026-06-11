LIB_NAME="OpenSSL"
LIB_REPO="https://github.com/openssl/openssl.git"
LIB_VERSION="OpenSSL_1_1_1m"

PARTY_DIR="$HOME/ThirdParty"
INSTALL_DIR="$PARTY_DIR/installed"
LIB_SRC_DIR="$PARTY_DIR/$LIB_NAME"
LIB_INSTALL_DIR="$INSTALL_DIR/$LIB_NAME"
LIB_INSTALL_LOG="$PARTY_DIR/${LIB_NAME}Setup.log"

if [ ! -d $PARTY_DIR ]; then
   mkdir -p $INSTALL_DIR
fi

if test -f $LIB_INSTALL_LOG; then
   rm $LIB_INSTALL_LOG
fi
echo Logging to $LIB_INSTALL_LOG

cd $PARTY_DIR

# Clone, checkout, build and install
echo Setting up $LIB_NAME...
if [ ! -d $LIB_SRC_DIR ]; then
   echo Cloning $LIB_NAME repository
   git clone $LIB_REPO $LIB_NAME >> $LIB_INSTALL_LOG 2>&1
   cd $LIB_SRC_DIR
else
   echo Cleaning $LIB_NAME repository
   cd $LIB_SRC_DIR
   git clean -dfx >> $LIB_INSTALL_LOG 2>&1
   echo Cleaned $LIB_NAME repository
   git pull >> $LIB_INSTALL_LOG 2>&1
fi
git checkout $LIB_VERSION >> $LIB_INSTALL_LOG 2>&1
echo Checked out commit $LIB_VERSION

if [ -d $LIB_INSTALL_DIR ]; then
   echo Removed installed $LIB_NAME
   rm -rf $LIB_INSTALL_DIR
fi
mkdir $LIB_INSTALL_DIR

echo Configuring...
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
   ./Configure linux-generic64 no-shared --prefix="$LIB_INSTALL_DIR" --openssldir="$LIB_INSTALL_DIR" >> $LIB_INSTALL_LOG 2>&1
else  # assume macOS
   MAC_ARCH=$(uname -m)    # x86_64 for Intel and arm64 for Apple M series
   ./Configure darwin64-$MAC_ARCH-cc no-shared --prefix="$LIB_INSTALL_DIR" --openssldir="$LIB_INSTALL_DIR" >> $LIB_INSTALL_LOG 2>&1
fi
if [ $? -ne 0 ]; then
   echo Configuring $LIB_NAME failed
   exit 1
fi

echo Building...
# Default, builds everything including app and tests.
# make >> $LIB_INSTALL_LOG 2>&1
# Build libraries only.
make LIBS="libcrypto.a libssl.a" build_libs >> $LIB_INSTALL_LOG 2>&1
if [ $? -ne 0 ]; then
   echo Building $LIB_NAME failed
   exit 1
fi

echo Installing...
# Default, installs everything including app, utils, doc, etc.
# make install >> $LIB_INSTALL_LOG 2>&1
# Install libraries and headers only.
make install_dev >> $LIB_INSTALL_LOG 2>&1
if [ $? -ne 0 ]; then
   echo Installing $LIB_NAME failed
   exit 1
fi

echo Installed to $LIB_INSTALL_DIR
echo $LIB_NAME ready.
