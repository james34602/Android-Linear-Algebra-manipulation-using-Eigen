# Linear Algebra computing using Eigen C++ library for Android

A sample app to compute arbitrary size matrix.

It came to perform these matrix operations:

1. Addition
2. Subtraction
3. Multiplication
4. Division
5. Transpose

# How is the numbers calculated?

Java part:

1. Define a set of array.
2. Define row and column.
3. LoadLibrary()
4. Call JNI via native function.
5. When C++ return array, display result to user.

C++ part:

1. Receive a set of array and row column configuration.
2. Call Eigen Library.
3. Do the math!
4. Return computed array.