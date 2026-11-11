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
