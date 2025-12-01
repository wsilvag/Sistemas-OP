#!/bin/bash
echo "--- Test 4: Long-Running Daemon ---"
DAEMON_LOG="/tmp/daemon.log"
rm -f $DAEMON_LOG
echo "Starting process_daemon in background..."
./process_daemon
DAEMON_PID=$(ps aux | grep 'process_daemon' | grep -v grep | awk '{print $2}')
if [ -z "$DAEMON_PID" ]; then
    echo "FAILURE: Could not find daemon PID."
    exit 1
fi
echo "Daemon PID: $DAEMON_PID. Monitoring for 15 seconds..."

# Check for zombies every 3 seconds
ZOMBIES_FOUND=0
for i in {1..5}; do
    sleep 3
    # Look for 'Z' status processes whose parent is the daemon
    ZOMBIES=$(ps -o pid,ppid,stat,cmd -ax | awk -v ppid="$DAEMON_PID" '{ if ($3 == "Z" && $2 == ppid) print $0 }' | wc -l)
    if [ "$ZOMBIES" -ne 0 ]; then
        echo "FAILURE: Detected $ZOMBIES zombies at check $i."
        ps aux | grep 'Z' | grep $DAEMON_PID
        ZOMBIES_FOUND=1
        break
    else
        echo "Check $i: No zombies detected."
    fi
done

# Clean up
echo "Killing daemon (PID $DAEMON_PID)..."
kill $DAEMON_PID
sleep 1
kill -9 $DAEMON_PID 2>/dev/null # Ensure it's dead
wait $DAEMON_PID 2>/dev/null

if [ "$ZOMBIES_FOUND" -eq 0 ]; then
    echo "SUCCESS: Daemon ran for 15s without creating permanent zombies."
else
    echo "FAILURE: Daemon created zombies."
fi

echo "Last 10 lines of $DAEMON_LOG:"
tail $DAEMON_LOG 

echo ""
