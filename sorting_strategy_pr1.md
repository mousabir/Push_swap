# Push_swap – Argument Parsing and Sorting Strategy

## 1. Argument Parsing

The first step is to validate the arguments passed to the program.

I decided to use a structure to store the program's data and options. The structure will be passed from the main function to helper functions, including the function responsible for validating the arguments.

The parsing process:

1. Check whether an argument is a valid option.
2. If it is an option, save the selected option in the structure.
3. Otherwise, check whether it is a valid integer.
4. If it is valid, add it to the stack.
5. Continue until all arguments have been processed.

At the end, the argument-parsing function should return the initialized stack and the selected options.

## 2. Representing the Stack

After validating the arguments, we need a way to represent the stack.

I chose to use a linked list to represent each stack, allowing us to manipulate the elements dynamically.

The next step is to implement the functions responsible for scanning and manipulating the stack.

## 3. Simple Sorting Strategy – Selection Sort

For the first sorting strategy, I chose Selection Sort.

The main idea is simple:

- Find the smallest element.
- Move it to the top.
- Move it to the other stack.
- Repeat the process with the remaining elements.

However, Push_swap has specific stack operations. We cannot directly access or move an element from the middle of a stack.

We must follow the available operations to bring the desired element to the top.

## 4. Finding the Smallest Element

The first function I need is a scanning function.

Its responsibilities:

- Receive the stack and a starting index.
- Scan the stack from that index.
- Find the smallest element in the remaining unsorted section.
- Return a pointer to the node containing that element.

This allows us to find the next smallest element without scanning the already sorted section again.

## 5. Finding the Best Rotation

After finding the smallest element, we need to calculate how to bring it to the top using the available operations.

For example:

- `ra`: Move the top element to the bottom.
- `rra`: Move the bottom element to the top.

Let:

- `p` = position of the target element.
- `n` = number of elements in the stack.

The rotation strategy:

1. If `p == 0`, the element is already at the top. Do nothing.
2. Otherwise, compare `p` with `n - p`.
3. If `p <= n - p`, use `ra` exactly `p` times.
4. Otherwise, use `rra` exactly `n - p` times.

The goal is to minimize the number of rotations.

**Important:** This is a basic rotation strategy. Additional optimization can be introduced later.

## 6. Moving Elements Between Stacks

The algorithm I want to implement works as follows:

1. Scan stack A to find the smallest element.
2. Calculate the best way to bring it to the top.
3. Apply the required rotations.
4. Use `pb` to move it to stack B.
5. Repeat with the remaining elements until stack A is empty.
6. Move the elements back from stack B to stack A using `pa`.

Since we push the smallest elements first, stack B will contain the elements in descending order, with the largest element at the top.

Moving them back to stack A using `pa` will produce an ascending order.

## 7. Updating the Stack State

One important implementation detail is that the stack changes after every operation.

I need to make sure that:

- The stack structure is updated after `pb` and `pa`.
- The size of each stack is updated.
- Element positions are recalculated when necessary.
- The algorithm always works with the current state of the stacks.

The position of an element can change after a rotation or a push operation, so I must not rely on positions calculated during a previous iteration.

## 8. Functions to Implement

The first functions I plan to develop are:

- **Scanner:** Find the smallest element in the unsorted section of a stack.
- **Rotation calculator:** Determine the cheapest direction to bring an element to the top.
- **Rotation executor:** Apply the required `ra` or `rra` operations.
- **Stack updater:** Keep track of stack contents and sizes.
- **Sorting function:** Coordinate the scanning, rotating, and pushing operations.

## 9. Current Challenge

I have identified the basic algorithm and the operations needed to implement it.

My next challenge is deciding how to organize the execution of these operations.

I need to make sure that the algorithm always uses the current stack state, recalculates positions when necessary, and executes the correct operations at every iteration.

The goal is not only to sort the stack correctly, but also to keep the implementation clean, flexible, and easy to optimize later.

# Push_swap changes

## The algorithm used in this part (medium, chunk sort, O(n sqrt n))

This part is the medium strategy of the subject, called with --medium. It is a chunk-based sort. The idea is to cut the numbers into about sqrt(n) groups by value, so we do not search the whole stack every time.

Step 1, ranking. creat_rank copies all the values into an array, sorts the array, and gives each node an index from 0 to n-1 (its place in the sorted order). After this we work with the index and not with the real number, so negative numbers and big numbers are not a problem.

Step 2, chunk size. The chunk size is the square root of n (ft_sqrt). For 100 numbers the chunk size is 10. For 4 numbers it is 2.

Step 3, scatter from A to B. We keep a range of indexes, from current_min to current_max (the upper limit is not included). We look at the top of A. If its index is inside the range, we push it to B with pb. If not, we rotate A with ra. When we have pushed chunk size numbers, the range moves up by one chunk. We continue until A is empty. At this point B holds all the numbers, and the small chunks are deeper and the big chunks are on top.

Step 4, bring back from B to A. We push back with pa until B is empty.

Why it is O(n sqrt n): each number is pushed once, and between two pushes we rotate at most about the size of one range, which is around sqrt(n) steps in a good case. So n numbers times sqrt(n) gives n sqrt(n).

Important note: the pa phase as written now does not sort the result yet. It pops B from the top, so inside each chunk the numbers come back in the wrong order, and the stack A is only sorted by chunks. To sort it fully, before each pa we must bring the biggest index of B to the top of B (with rb or rrb, choosing the shorter way), and then do pa. This is also what will reduce the number of moves for the 100 and 500 targets. It is in the still to do list below.

## What was wrong and what we changed

1. stack_b in main_porpes was a full struct (t_list stack_b) used as a fake first node. It was on the stack memory, not malloc. This caused the invalid pointer error when freeing. Now stack_b is a pointer set to NULL, and an empty stack is just NULL.

2. ft_medimum and ft_smeple now take stack_b as a pointer to pointer (t_list **), the same as head. The prototypes in push_swap.h were changed too. main_porpes sends &stack_b and frees it with free_list(stack_b).

3. In ft_medimum, the while loop of pa was checking stack_b itself, which is never NULL. Now it checks *stack_b. This fixed the segfault in push_node. The debug printf was removed.

4. In scatter_chunks, the upper limit is now strict (index < current_max) and the counter check is pushed >= chunks_size. This stops the infinite ra loop.

5. Printing and counting: pb now goes through bring_to_top(0, ...), which prints pb and counts it. The pa loop in ft_medimum prints pa and counts it before calling move_a. move_a itself does not print or count, so nothing is counted twice.

6. Parser (check file): a lone + or - or an empty string is now an Error. A number outside the int range is now an Error (chek_range). Duplicates are now an Error (has_dup). The two new functions are in a new file, check_utils.c, and push_swap.h includes limits.h.

## Why the infinite loop with 8 to 10 numbers happened

The input had duplicate numbers. asind_index gave the same index to equal numbers, so some indexes did not exist, a chunk never became full, and ra repeated forever. The subject says duplicates must give Error, so the duplicate check fixes it.

## Still to do

1. Use adaptive as the default strategy when no flag is given.
2. Write the complex strategy O(n log n) and the adaptive strategy.
3. Compute the disorder before any move.
4. Write the bench output on stderr.
5. Improve the pa phase (bring the biggest number of B to the top first) and test 100 and 500 numbers with wc -l.
6. Add -g to the Makefile flags while testing, and check for leaks.