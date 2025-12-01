#!/bin/bash
echo "--- Test 2: Detect Zombies ---"
N=3
./zombie_creator $N &
CREATOR_PID=$!
sleep 1
echo "Running zombie_detector..."
./zombie_detector
# Clean up
kill $CREATOR_PID
wait $CREATOR_PID 2>/dev/null
echo ""
