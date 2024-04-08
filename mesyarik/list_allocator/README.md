# List + StackAllocator
I really like this project, it is pretty nice.

But still... There are some things I am not sure about:
1) noexcept specifier: sometimes they are random, 
because cppreference and STL have different style, and I don't understand 
how to use it correctly. (Especially StackAllocator constructors marked noexcept looks strange)
2) Move semantics: It is my first time using it, so it could be wrong and strange.
List move constructor and move assignment operator aren't even implemented.
3) Allocator awareness and correctness of allocator. (I hope it's OK)

My List and StackAllocator passed all of Mecsherin's tests, but it doesn't mean that my code works :)
Probably it should be more commented, but I forgot to write comments.

Btw I don't know where my code doesn't work properly, if it does. 
Most of my previous projects has such tests, but List + StackAllocator haven't.

(You can find some Raigor reference in stackallocator_test.cpp)