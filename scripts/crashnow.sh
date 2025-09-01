#!/bin/bash
# Script to forcefully kill the citybits process to test WAL recovery.
echo "Finding and killing citybits process..."
PID=$(pgrep citybits)
if [ -z "$PID" ]; then
    echo "No citybits process found."
else
    echo "Killing citybits (PID: $PID) with SIGKILL..."
    kill -9 $PID
    echo "Process killed."
fi