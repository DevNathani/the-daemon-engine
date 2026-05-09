#!/bin/bash

USER="$1"

if [ -z "$USER" ]; then
    echo "Usage: $0 <username>"
    exit 1
fi

# check user exists
if ! id "$USER" &>/dev/null; then
    echo " User does not exist: $USER"
    exit 1
fi

echo " Killing sessions for $USER..."

# kill all processes of user (includes ssh sessions)
pkill -KILL -u "$USER" 2>/dev/null

echo " All sessions terminated for $USER"