#!/bin/bash

# Define source directory (Downloads folder)
DOWNLOADS_DIR="$HOME/Downloads"

# Define target directories
IMAGE_DIR="$DOWNLOADS_DIR/Images"
DOC_DIR="$DOWNLOADS_DIR/Documents"
SCRIPT_DIR="$DOWNLOADS_DIR/Scripts"
VIDEO_DIR="$DOWNLOADS_DIR/Videos"

# Create directories if they do not exist
mkdir -p "$IMAGE_DIR" "$DOC_DIR" "$SCRIPT_DIR" "$VIDEO_DIR"

# Move files based on their extensions
mv "$DOWNLOADS_DIR"/*.jpg "$IMAGE_DIR" 2>/dev/null
mv "$DOWNLOADS_DIR"/*.png "$IMAGE_DIR" 2>/dev/null
mv "$DOWNLOADS_DIR"/*.pdf "$DOC_DIR" 2>/dev/null
mv "$DOWNLOADS_DIR"/*.docx "$DOC_DIR" 2>/dev/null
mv "$DOWNLOADS_DIR"/*.sh "$SCRIPT_DIR" 2>/dev/null
mv "$DOWNLOADS_DIR"/*.mp4 "$VIDEO_DIR" 2>/dev/null

echo "File organization completed!"
