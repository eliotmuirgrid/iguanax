#!/bin/sh

FILE="./.git/hooks/commit-msg"

cat <<EOM >$FILE
#!/bin/sh 

test "" = "\$(grep '^IE\\-\\d*' "\$1")" && {
   echo >&2 "Commit must start with IE-xxxx Jira Ticket number"
   exit 1
}

exit 0
EOM

chmod +x ./.git/hooks/commit-msg

echo "Updated commit-msg hook"
