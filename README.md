> __Note:__ this compiler isn't 100% functioning (read below).

This is a compiler, in C, for the B-Minor programming language described in Douglas Thain's
["Introduction to Compilers and Language Design"](https://www3.nd.edu/~dthain/compilerbook/).
Really at the moment it's a compiler that _almost_ works, but not quite. In particular, there
are some issues with string storage/operations (as is often the case in C), and some assembly
instructions are constructed fine without problems, but others are not.

I may return to this in the future; however, it's quite a big project full of a lot of code
that built off of example starting points given by the textbook, and I feel to dig deep into
fixing it I'd have to refactor quite a bit of it to patterns I feel are more effective for me.
Not mentioning I likely would have had a much better time using at the very least C++ rather
than C, which the book was geared towards.

That said, having [practiced with some compiler tools in Rust](https://github.com/joculatrix/foo_llvm),
I'm planning to rewrite a compiler for B-Minor soon in Rust (which I'm far more comfortable
with) and, hopefully, end up with something that actually works.
