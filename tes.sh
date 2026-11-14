ARG=$(shuf -i 0-9999 -n 100 | tr '\n' ' ')
./push_swap --simple $ARG | wc -l
./push_swap --medium $ARG | wc -l
