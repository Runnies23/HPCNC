#!/bin/bash

# Usage: ./run_mpi.sh file_name.c [args...]
# Example: ./run_mpi.sh cannon.c 10

# Exit immediately if a command exits with a non-zero status
set -e

# Check if a file name is provided
if [ $# -lt 1 ]; then
  echo "Usage: $0 <file.c> [args...]"
  exit 1
fi

# Get filename and remove extension
SRC_FILE="$1"
BASENAME=$(basename "$SRC_FILE" .c)
OUTPUT="${BASENAME}.output"

# Compile the MPI C file
echo "Compiling $SRC_FILE ..."
mpicc "$SRC_FILE" -o "$OUTPUT" -lm

# Run with 25 processes and pass all arguments except the first (the source file)
echo "Running with mpirun..."
mpirun --oversubscribe -np 4 "./$OUTPUT" "${@:2}"

echo "" #in case of text output not have new line
echo "Cleaning up..."
rm -f "$OUTPUT"

echo "Done!"

# Optional cleanup (uncomment if you want to remove the output after running)
# rm -f "$OUTPUT"


# // run ========
# chmod +x run_mpi.sh
# ./run_mpi.sh myprogram.c 10 20
# // run ========
