# Allow continues.
def binary ; 1 (x y) y;

# Sum
def sum(x)
  var ret = 0 in
    (for i = 0, i < x, 1 in
      ret = ret + i);
    ret

sum(7)