#!/bin/bash

DB="./var/game/db/game.db"
CSV="$1"

[ -z "$CSV" ] && echo "Usage: $0 <names.csv>" && exit 1
[ ! -f "$CSV" ] && echo "File not found" && exit 1

sed -i 's/\r$//' "$CSV"

echo "[+] Importing level names..."

{


    while IFS=',' read -r level name
    do
        level=$(echo "$level" | xargs)
        name=$(echo "$name" | xargs)

        [ -z "$level" ] && continue
        [ -z "$name" ] && continue

        echo "[+] Level $level → $name"

        sqlite3 "$DB" <<EOF
INSERT OR REPLACE INTO level_names (level, name)
VALUES ($level, '$name');
EOF

    done
} < "$CSV"

echo "[✓] Done"