ScriptHome=$(pwd)
BUILDRESULTS_DIR="$1"

# check windows path to Cygwin path
unameOut="$(uname -s)"
case "${unameOut}" in
  CYGWIN*)    BUILDRESULTS_DIR="$(cygpath "$BUILDRESULTS_DIR")" ;;
esac

if [ -d "$BUILDRESULTS_DIR" ]; then rm -Rf "$BUILDRESULTS_DIR"; fi
mkdir "$BUILDRESULTS_DIR"
start_time="$(date -u +%s)"

make -j8 clean -C "$ScriptHome"/Iguana/

echo "Build examples"
for dir in "$ScriptHome"/*example/; do (
  filename="$(basename "$dir")"
  cd "$dir" && make -j8 >"$BUILDRESULTS_DIR/$filename.build.txt" 2>&1
  if [ "$?" -eq 0 ]; then
    rm -f "$BUILDRESULTS_DIR/$filename.build.txt"
    # kill test if it runs for more than 30s. May need to make it longer depending on test
    cd "$dir" && ./test >"$BUILDRESULTS_DIR/$filename.test.txt" 2>&1
    if [ "$?" -eq 0 ]; then
      rm -f "$BUILDRESULTS_DIR/$filename.test.txt"
    fi
  fi
); done

# build Iguana and output errors to ZBUILDresults/Iguana.txt
echo "Build Iguana"

# TODO: strip posix binary, sign windows
case "${unameOut}" in
    Linux*)                 make -j8 -C "$ScriptHome"/Iguana/                >"$BUILDRESULTS_DIR/Iguana.txt" 2>&1 ;;
    Darwin*)  IFW_RELEASE=1 make -j8 -C "$ScriptHome"/dmg_installer/         >"$BUILDRESULTS_DIR/Iguana.txt" 2>&1 ;;
    CYGWIN*)                make -j8 -C "$ScriptHome"/iguana_installer_nsis/ >"$BUILDRESULTS_DIR/Iguana.txt" 2>&1 ;;
    *)        echo "Unsupported OS: ${unameOut}"  return 1
esac

if [ "$?" -eq 0 ]; then
  rm -f "$BUILDRESULTS_DIR/Iguana.txt"
fi

end_time="$(date -u +%s)"

elapsed="$((end_time - start_time))"
echo "Build took $elapsed seconds"
