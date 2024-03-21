#!/bin/bash

# Check if two command-line arguments are provided
if [ $# -ne 2 ]; then
    echo "Usage: $0 <number1> <number2>"
    exit 1
fi

# Assign command-line arguments to variables
num1=$1
num2=$2

# Print the entered numbers
echo "You entered the first number as: $num1"
echo "You entered the second number as: $num2"

