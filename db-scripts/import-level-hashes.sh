#!/bin/bash

DB="./var/game/db/game.db"
CSV="$1"

[ -z "$CSV" ] && echo "Usage: $0 <hashes.csv>" && exit 1
[ ! -f "$CSV" ] && echo "File not found" && exit 1

sed -i 's/\r$//' "$CSV"

echo "[+] Importing level hashes..."

{
    while IFS=',' read -r level hash
    do
        level=$(echo "$level" | xargs)
        hash=$(echo "$hash" | xargs)

        [ -z "$level" ] && continue
        [ -z "$hash" ] && continue

        echo "[+] Level $level"

        sqlite3 "$DB" <<EOF
INSERT OR REPLACE INTO level_data (level, hash)
VALUES ($level, '$hash');
EOF

    done
} < "$CSV"

echo "[✓] Done"