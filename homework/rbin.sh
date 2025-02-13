#!/bin/bash

# *******************************************************************************
#  Author  : Joseph Gargiulo
#  Date    : 2/5/24
#  Description: CS392 - Homework 1
#  Pledge  : I pledge my honor that I have abided by the Stevens Honor System.
# ******************************************************************************

#variables to help keep track of option counts and arguments count
helpcount=0
listcount=0 
purgecount=0
argscount=0

# Define read-only variable for the path to .recycle
readonly RECYCLE_PATH="$HOME/.recycle"
mkdir -p $RECYCLE_PATH



# Function to display usage message
usage() {
cat << EOF
Usage: rbin.sh [-hlp] [list of files]
   -h: Display this help;
   -l: List files in the recycle bin;
   -p: Empty all files in the recycle bin;
   [list of files] with no other flags,
        these files will be moved to the
        recycle bin.
EOF
}


# Parse command line arguments using getopts
while getopts ":hlp" opt; do
  case $opt in
    h) ((helpcount++));;
    l) ((listcount++));;
    p) ((purgecount++));;
    \?) echo "Error: Unknown option '-$OPTARG'." >&2
        usage
        exit 1 ;;
  esac
done

# Shift the processed options
shift $((OPTIND - 1))

# Check for too many options enabled
if [ $((helpcount + listcount + purgecount)) -gt 1 ]; then
  echo "Error: Too many options enabled." >&2
  usage
  exit 1
fi

# Function to check if the file exists and move to recycle bin
check_files() {
  if [ ! -e "$1" ]; then
    echo "Warning: '$1' not found." >&2
    exit 1
  else
    mv -f "$1" "$RECYCLE_PATH"
  fi
}

# Process the remaining arguments (list of files)
for f in "$@"; do
  ((argsflag++))
  # If one or more flags are specified and files are supplied, throw error
  if [ $((helpcount + listcount + purgecount)) -gt 0 ] && [ $argsflag -gt 0 ]; then
    echo "Error: Too many options enabled." >&2
    usage
    exit 1
  fi

  check_files "$f"
done
shift "$((OPTIND-1))"


# Additional conditions
if [ $helpcount -eq 1 ]; then
  usage
  exit 0
fi

if [ $listcount -eq 1 ]; then
  ls -lAF "$RECYCLE_PATH"
  exit 0
fi

if [ $purgecount -eq 1 ]; then
  find "$RECYCLE_PATH" -mindepth 1 -delete
  exit 0
fi


# If no arguments are provided, display usage message
if [ $# -eq 0 ]; then
  usage
  exit 1
fi


exit 0