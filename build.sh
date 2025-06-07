#!/bin/bash

case "$1" in
  "") cmd.exe /C "build.bat" 
  ;;
  -r) cmd.exe /C "r_build.bat"
  ;;
  *) cmd.exe /C "build.bat" 
  ;;
esac


