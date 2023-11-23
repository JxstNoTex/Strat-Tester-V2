@echo off
git submodule update --init --recursive
Tools\premake5 %* vs2022