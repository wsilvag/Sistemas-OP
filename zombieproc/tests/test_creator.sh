#!/bin/bash
echo "--- Test 1: Create Zombies ---"
N=5
./zombie_creator $N &
CREATOR_PID=$!
sleep 1
echo "Checking for $N zombies created by PID $CREATOR_PID..."
# Use awk to find zombies ('Z') and filter by the creator's PID
ZOMBIES=$(ps -o pid,ppid,stat,cmd -ax | awk -v ppid="$CREATOR_PID" '{ if ($3 == "Z" && $2 == ppid) print $0 }' | wc -l)
if [ "$ZOMBIES" -eq "$N" ]; then
    echo "SUCCESS: Found $ZOMBIES/$N zombies."
else
    echo "FAILURE: Found $ZOMBIES/$N zombies. Expected $N."
    ps -o pid,ppid,stat,cmd -ax | grep $CREATOR_PID
fi
# Clean up the creator (which cleans up the zombies)
kill $CREATOR_PID
wait $CREATOR_PID 2>/dev/null
echo ""
