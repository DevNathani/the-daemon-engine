#!/bin/bash

DB="./var/game/db/game.db"
CSV="$1"

[ -z "$CSV" ] && echo "Usage: $0 <levels.csv>" && exit 1
[ ! -f "$CSV" ] && echo "File not found" && exit 1

sed -i 's/\r$//' "$CSV"

echo "[+] Importing level content..."

{


    while IFS=',' read -r level message hint
    do
        level=$(echo "$level" | xargs)

        [ -z "$level" ] && continue

        echo "[+] Level $level content"

        sqlite3 "$DB" <<EOF
INSERT OR REPLACE INTO level_content (level, messages, final_hint)
VALUES ($level, '$message', '$hint');
EOF

    done
} < "$CSV"

echo "[✓] Done"