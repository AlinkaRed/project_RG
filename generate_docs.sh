#!/bin/bash
echo "Generating Doxygen documentation..."

doxygen Doxyfile

if [ $? -eq 0 ]; then
    echo "Documentation successfully generated in docs/html/"
    echo "Open docs/html/index.html in browser"
else
    echo "Documentation generation error!"
    exit 1
fi