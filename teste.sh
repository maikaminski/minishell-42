#!/bin/bash

# Comandos simples
echo "Hello World"
pwd
env

# Redirecionamentos
echo "test" > file.txt
cat < file.txt
echo "append" >> file.txt

# Pipes
ls | grep minishell
echo "hello" | cat | cat

# Variáveis
export TEST=hello
echo $TEST
echo $?

# Heredoc
cat << EOF
hello
world
EOF
