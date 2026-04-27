@echo off
compiler.exe test.c
if %errorlevel% neq 0 (
    echo Compilation failed.
    pause
    exit /b 1
)

dot -Tpng ast.dot -o ast.png
if %errorlevel% neq 0 (
    echo Graphviz failed.
    pause
    exit /b 1
)

:: Show the AST tree
start "" ast.png

:: Open output.asm in emu8086 and run it
start "" "C:\Users\User\Desktop\Folders_main\Tools\Assembly\emu8086\emu8086.exe" "C:\Projects\SimpleCCompiler\output.asm"