import time

def clock():
    return time.monotonic()

def fib(n):
    if n < 2: return n
    return fib(n-1) + fib(n-2)

before = clock()
result = fib(35)
after = clock()

elapsed = after - before
print(f'fib(35) result:\n{result}\n')
print(f'fib(35) elapsed time:\n{elapsed}\n')

# fib(40): 16.791 seconds elapsed
# fib(35):  1.480 seconds elapsed
