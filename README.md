# Push_swap

*This project has been created as part of the 42 curriculum by mousabir , ilazerou.*

## Description

Push_swap is a sorting algorithm project. The goal is to sort a stack of integers using a limited set of operations and the smallest possible number of moves.

The project explores different sorting strategies and algorithm complexity.

 ## checker : 
 is the bonus verification program for `push_swap`. It takes an initial list of unique integers representing stack `a`, reads newline-separated `push_swap` instructions (`sa`, `sb`, `ss`, `pa`, `pb`, `ra`, `rb`, `rr`, `rra`, `rrb`, `rrr`) from `stdin`, executes them on stacks `a` and `b`, and prints `OK\n` if stack `a` ends sorted in ascending order with stack `b` empty—or `KO\n` otherwise.

## Instructions

### Compilation

Run the following command:

make
for the checker :
make checker

### Execution

./push_swap 4 2 7 1 3
for the checker :
./checker 42 2006 1337

The program outputs the operations needed to sort the stack.

## Resources

- 42 Push_swap subject
- Sorting algorithms
- Algorithm complexity (Big O notation)
- Stack data structures

## AI Usage

AI was used as a learning and support tool to understand concepts, explore algorithmic approaches, and review ideas during development.

## Why I Chose These Algorithms

 * **Simple ($O(n^2)$) – Selection Sort:** I went with Selection Sort because the idea is straightforward and easy to picture. For each of the $n$ numbers, you scan through the rest to find what comes next, which naturally leads to $n \times n$ comparisons. Other algorithms do the same thing, so this felt like a clean, practical starting point.
- * **Medium ($O(n\sqrt{n})$) – Chunk Sort:** Splitting the stack into chunks and using rank order makes a lot of sense when you don't want to rotate through the whole stack every time. Grouping elements into smaller ranges keeps moves much more manageable.
- * **Complex ($O(n \log n)$) – Radix Sort:** Looking at a number part by part feels similar to how we compare two numbers in our head—checking each place value until one stands out. Once you see it that way, the logic is easy to follow and scales well as the list gets bigger.