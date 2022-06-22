# Binary and.
def binary& 5 (l r)
  if l then
    if r then
      1
    else
      0
  else
    0

# Test.
def test(a b)
  a & b

# Test and operation.
test(0, 0)
test(0, 1)
test(1, 0)
test(1, 1)