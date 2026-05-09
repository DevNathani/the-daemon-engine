#!/bin/bash

# Exit immediately if any command fails (like throwing an exception)
set -e

echo " Starting import process..."

# Create database
sudo ./db-scripts/create-db.sh
echo " Database game.db created successfully"

# Run all imports
sudo ./db-scripts/import-level-content.sh ./CSVs/demo-levels-content.csv
echo " Level content imported"

sudo ./db-scripts/import-level-hashes.sh ./CSVs/demo-levels-hash.csv
echo " Level hashes imported"

sudo ./db-scripts/import-level-names.sh ./CSVs/demo-levels-name.csv
echo " Level names imported"

sudo ./db-scripts/import-players.sh ./CSVs/demo-players.csv
echo " Players imported"

echo " All imports completed successfully!"