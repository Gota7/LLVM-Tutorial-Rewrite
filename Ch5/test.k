# Factorial calculation.
def fact(x)
  if x < 2 then
    1
  else
    x * fact(x - 1)

# Calculate factorials.
for i = 0, i < 7, 1 in
  fact(i)