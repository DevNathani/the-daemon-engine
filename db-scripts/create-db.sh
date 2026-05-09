#!/bin/bash

# ---------------- DB INIT ----------------
DB=./var/game/db/game.db

if [ ! -f "$DB" ]; then
    echo "[INIT] Creating database..."

    sqlite3 "$DB" <<EOF
CREATE TABLE players (
    username TEXT PRIMARY KEY,
 password TEXT,created_at INTEGER)
;
CREATE TABLE progress (
    username TEXT PRIMARY KEY,
    level INTEGER DEFAULT 0,
    completed INTEGER DEFAULT 0,
    score INTEGER DEFAULT 0,
    gaveup INTEGER DEFAULT 0,
    last_updated INTEGER,
    FOREIGN KEY(username) REFERENCES players(username)
);
CREATE TABLE submissions (
    username TEXT,
    level INTEGER,
    gaveup INTEGER DEFAULT 0,
    completed INTEGER DEFAULT 0,
    time INTEGER,
    PRIMARY KEY(username, level)
);
CREATE TABLE level_names (
    level INTEGER PRIMARY KEY,
    name TEXT NOT NULL
);
CREATE TABLE level_data (
    level INTEGER PRIMARY KEY,
    hash TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS "level_content" (
	"level"	INTEGER NOT NULL,
	"messages"	TEXT,
	"final_hint"	TEXT,
	PRIMARY KEY("level")
);

EOF
fi

chown root:root "$DB"
chmod 600 "$DB"
