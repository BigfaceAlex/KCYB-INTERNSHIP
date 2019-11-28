import random

data_a = [] 
# 300000
for i in range(0, 5000000):
  num = random.randint(0, 20000000)
  data_a.append(num)
data_a.sort()
for i in range(0, len(data_a)):
  if i != 0 and data_a[i] != data_a[i - 1]:
    print data_a[i]


