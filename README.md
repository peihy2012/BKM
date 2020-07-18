# BKM 
分块KM算法（Block KM Algorithm, BKM）解决二分图最优匹配问题。采用分块的方法，将设定匹配阈值（权重）范围内的节点分为一个区块，分区块采用KM算法。

## support

支持c++11

支持Linux，Windows系统

## deps

依赖[Eigen](https://github.com/artsy/eigen)库

## test

```shell
cd BKM && mkdir build && cd build && cmake .. && make
./assignment
```

output:

```shell
create Assingment
Assingment created and start Solve
Assingment Solved and get result
get result and print
Left: 0 --> Right: 2
Left: 1 --> Right: 12
Left: 2 --> Right: 11
Left: 3 --> Right: 4
Left: 4 --> Right: 9
Left: 5 --> Right: 13
Left: 6 --> Right: 5
Left: 7 --> Right: 10
Left: 8 --> null
Left: 9 --> Right: 0
```

## reference

参考**[Multitarget-tracker](https://github.com/Smorodov/Multitarget-tracker)**中的KM算法。