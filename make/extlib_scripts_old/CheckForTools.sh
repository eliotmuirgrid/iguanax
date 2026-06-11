# Check OS and store
OS=$(sh -c 'uname 2>/dev/null')

# Only check for OS specific tools
if [ $OS = Darwin ]; then
  echo "Checking for Xcode Command Line Tools"
  RESULT=$(
    xcode-select -p 1>/dev/null
    echo $?
  )
  if [ $RESULT != "0" ]; then
    echo "Xcode Command Line Tools not found!"
    exit 1
  fi
  echo "Found xcode-select"
else
  if ! command -v git &>/dev/null; then
    echo "Git not found"
    exit 1
  fi
  if ! command -v gcc &>/dev/null; then
    echo "GCC not found"
    exit 1
  fi
fi

if ! command -v automake --version &>/dev/null; then
  echo "automake not found"
  exit 1
fi
