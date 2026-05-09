#!/bin/bash

DB="./var/game/db/game.db"
CSV="$1"

if [ -z "$CSV" ]; then
    echo "Usage: $0 <players.csv>"
    exit 1
fi

if [ ! -f "$CSV" ]; then
    echo "CSV file not found"
    exit 1
fi

# fix CRLF
sed -i 's/\r$//' "$CSV"

echo "[+] Importing players..."
echo

created=0
skipped=0

# 🔥 NO PIPE — FIXED
{

    while IFS=',' read -r username password
    do
        username=$(echo "$username" | xargs)
        password=$(echo "$password" | xargs)

        if [ -z "$username" ] || [ -z "$password" ]; then
            continue
        fi

        echo " user=$username pass=$password"

        exists=$(sqlite3 "$DB" "SELECT COUNT(*) FROM players WHERE username='$username';")

        if [ "$exists" -ne 0 ]; then
            echo "[=] Already exists: $username"
            skipped=$((skipped+1))
            continue
        fi

        echo "[+] Adding: $username"

        sqlite3 "$DB" <<EOF
INSERT INTO players (username, password,created_at)
VALUES ('$username', '$password',strftime('%s','now'));
EOF

        created=$((created+1))

    done
} < "$CSV"

echo
echo "================================="
echo "SUMMARY"
echo "================================="
echo "Created : $created"
echo "Skipped : $skipped"
echo "================================="