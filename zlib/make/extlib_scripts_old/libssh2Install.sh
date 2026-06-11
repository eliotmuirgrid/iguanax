LIB_NAME="libssh2"
LIB_REPO="https://github.com/libssh2/libssh2.git"
LIB_VERSION="libssh2-1.10.0"

PARTY_DIR="$HOME/ThirdParty"
INSTALL_DIR="$PARTY_DIR/installed"
LIB_SRC_DIR="$PARTY_DIR/$LIB_NAME"
LIB_INSTALL_DIR="$INSTALL_DIR/$LIB_NAME"
LIB_INSTALL_LOG="$PARTY_DIR/${LIB_NAME}Setup.log"

if [ ! -d $PARTY_DIR ]; then
   mkdir -p $INSTALL_DIR
fi

if [ -f $LIB_INSTALL_LOG ]; then
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
autoreconf -fi >> $LIB_INSTALL_LOG 2>&1
# LIBS="-lpthread -ldl" is needed for CentOS 7, not macOS
./configure LIBS="-lpthread -ldl" --enable-shared=no --enable-examples-build=no --prefix="$LIB_INSTALL_DIR" --with-libssl-prefix="$INSTALL_DIR/OpenSSL" >> $LIB_INSTALL_LOG 2>&1
if [ $? -ne 0 ]; then
   echo Configuring $LIB_NAME failed
   exit 1
fi

echo Building...
make >> $LIB_INSTALL_LOG 2>&1
if [ $? -ne 0 ]; then
   echo Building $LIB_NAME failed
   exit 1
fi

echo Installing...
make install >> $LIB_INSTALL_LOG 2>&1
if [ $? -ne 0 ]; then
   echo Installing $LIB_NAME failed
   exit 1
fi

echo Installed to $LIB_INSTALL_DIR
echo $LIB_NAME ready.
