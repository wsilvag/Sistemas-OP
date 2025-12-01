#!/bin/bash
echo "--- Test 3: Reaping Strategies ---"
for strategy in 1 2 3; do
    echo "Testing Strategy $strategy..."
    ./zombie_reaper $strategy
    # Verify no zombies remain from this program
    ZOMBIES=$(ps -o pid,stat,cmd -ax | grep -E 'Z.*zombie_reaper|defunct.*zombie_reaper' | grep -v grep | wc -l)
    if [ "$ZOMBIES" -eq 0 ]; then
        echo "SUCCESS: Strategy $strategy reaped all zombies (0 remaining)."
    else
        echo "FAILURE: Strategy $strategy left $ZOMBIES zombies."
        ps aux | grep 'Z' | grep zombie_reaper
    fi
done
echo ""
