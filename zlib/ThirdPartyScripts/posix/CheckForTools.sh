# Check OS and store
OS=$(sh -c 'uname 2>/dev/null')

if [ $OS = Darwin ]
then
list_pkgs="brew list --formula"
odbc="unixodbc"
else
list_pkgs="yum list installed"
odbc="unixODBC"
fi

# Only check for OS specific tools
if [ $OS = Darwin ]
then
   echo "Checking for brew..."
   RESULT=$(which brew)
   if [ $RESULT != /usr/local/bin/brew ]
   then
      echo "brew not found!"
      exit 1
   fi
   echo "Found brew"
else
   echo "Checking for libtool"
   RESULT=$($list_pkgs | grep libtool)
   if [ -z "$RESULT" ] # if RESULT is empty
   then
      echo "libtool not found!"
      exit 1
   else
      echo $RESULT "...Found"
   fi

   echo "Checking for gcc"
   RESULT=$($list_pkgs | grep gcc)
   if [ -z "$RESULT" ] # if RESULT is empty
   then
      echo "gcc not found!"
      exit 1
   else
      echo $RESULT "...Found"
   fi

   echo "Checking for devtoolset-7"
   RESULT=$($list_pkgs | grep devtoolset-7)
   if [ -z "$RESULT" ] # if RESULT is empty
   then
      echo "devtoolset-7 not found!"
      exit 1
   else
      echo $RESULT "...Found"
   fi

   echo "Checking for epel-release"
   RESULT=$($list_pkgs | grep epel-release)
   if [ -z "$RESULT" ] # if RESULT is empty
   then
      echo "epel-release not found!"
      exit 1
   else
      echo $RESULT "...Found"
   fi

fi

#checking for common tools

echo "Checking for cmake..."
RESULT=$(cmake --version | sed -n -r 's/^[^0-9]*([0-9]+).*$/\1/p')
if [ $RESULT -eq 3 ]
then
   echo "Found cmake version 3.x.x"
else
   echo "Missing cmake or version is lower than 3!"
   exit 1
fi

echo "Checking for ccache..."
RESULT=$($list_pkgs | grep ccache)
if [ -z "$RESULT" ] # if RESULT is empty
then
   echo "ccache not found!"
   exit 1
else
   echo $RESULT "...Found"
fi

echo "Checking for autoconf..."
RESULT=$($list_pkgs | grep autoconf)
if [ -z "$RESULT" ]
then
   echo "autoconf not found!"
   exit 1
else
   echo $RESULT "... Found"
fi

echo "Checking for automake..."
RESULT=$($list_pkgs | grep automake)
if [ -z "$RESULT" ]
then
   echo "automake not found!"
   exit 1
else
   echo $RESULT "... Found"
fi

echo "Checking for" $odbc "..."
RESULT=$($list_pkgs | grep $odbc)
if [ -z "$RESULT" ]
then
   echo $odbc " not found!"
   exit 1
else
   echo $RESULT "... Found"
fi
