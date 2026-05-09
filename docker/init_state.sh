#!/bin/bash
set -e

echo "[INIT] Starting game container..."

# ---------------- BASIC PERMISSIONS ----------------
chown root:root /home
chmod 711 /home

# ---------------- ROOT PATH ----------------
chown root:root /var
chmod 755 /var

# ---------------- GAME ROOT ----------------
chown root:root /var/game
chmod 755 /var/game

# ---------------- DATABASE ----------------
chown root:root /var/game/db
chmod 700 /var/game/db

chown root:root /var/game/db/game.db
chmod 600 /var/game/db/game.db

# ---------------- DONE ----------------
echo "Initialization complete."

exec "$@"