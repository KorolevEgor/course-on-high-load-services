# Без кеша
```
Params: threads: 1, connections: 1
Running 10s test @ http://localhost:8080/
  1 threads and 1 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.04ms  287.74us   8.37ms   93.67%
    Req/Sec     0.96k    75.55     1.07k    77.00%
  Latency Distribution
     50%    0.99ms
     75%    1.03ms
     90%    1.14ms
     99%    2.11ms
  9567 requests in 10.00s, 3.38MB read
Requests/sec:    956.30
Transfer/sec:    345.57KB
Params: threads: 2, connections: 2
Running 10s test @ http://localhost:8080/
  2 threads and 2 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.01ms  196.56us   9.95ms   98.23%
    Req/Sec     0.99k    34.89     1.03k    81.50%
  Latency Distribution
     50%    0.98ms
     75%    1.02ms
     90%    1.07ms
     99%    1.29ms
  19747 requests in 10.00s, 6.97MB read
Requests/sec:   1974.57
Transfer/sec:    713.50KB
Params: threads: 5, connections: 5
Running 10s test @ http://localhost:8080/
  5 threads and 5 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.38ms  391.86us  13.86ms   96.52%
    Req/Sec   727.81     34.91   780.00     79.60%
  Latency Distribution
     50%    1.32ms
     75%    1.43ms
     90%    1.56ms
     99%    2.65ms
  36227 requests in 10.01s, 12.78MB read
Requests/sec:   3620.65
Transfer/sec:      1.28MB
Params: threads: 10, connections: 10
Running 10s test @ http://localhost:8080/
  10 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     2.10ms    1.55ms  42.22ms   98.39%
    Req/Sec   496.15     28.00   560.00     80.80%
  Latency Distribution
     50%    1.92ms
     75%    2.18ms
     90%    2.51ms
     99%    4.71ms
  49408 requests in 10.01s, 17.43MB read
Requests/sec:   4937.61
Transfer/sec:      1.74MB
```

# С кешем
```
Params: threads: 1, connections: 1
Running 10s test @ http://localhost:8080/
  1 threads and 1 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.10ms  389.64us  14.51ms   97.40%
    Req/Sec     0.92k    63.18     1.00k    75.00%
  Latency Distribution
     50%    1.05ms
     75%    1.11ms
     90%    1.20ms
     99%    1.94ms
  9105 requests in 10.00s, 3.21MB read
Requests/sec:    910.48
Transfer/sec:    329.01KB
Params: threads: 2, connections: 2
Running 10s test @ http://localhost:8080/
  2 threads and 2 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.10ms  134.82us   6.59ms   94.23%
    Req/Sec     0.90k    27.98     0.95k    85.00%
  Latency Distribution
     50%    1.08ms
     75%    1.12ms
     90%    1.19ms
     99%    1.50ms
  17972 requests in 10.00s, 6.34MB read
Requests/sec:   1796.62
Transfer/sec:    649.20KB
Params: threads: 5, connections: 5
Running 10s test @ http://localhost:8080/
  5 threads and 5 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.56ms  520.58us  13.67ms   95.96%
    Req/Sec   646.90     47.45   717.00     79.20%
  Latency Distribution
     50%    1.47ms
     75%    1.61ms
     90%    1.79ms
     99%    3.69ms
  32214 requests in 10.01s, 11.37MB read
Requests/sec:   3218.85
Transfer/sec:      1.14MB
Params: threads: 10, connections: 10
Running 10s test @ http://localhost:8080/
  10 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     2.24ms  613.45us  18.91ms   87.39%
    Req/Sec   448.46     20.65   500.00     71.20%
  Latency Distribution
     50%    2.13ms
     75%    2.43ms
     90%    2.79ms
     99%    4.27ms
  44674 requests in 10.01s, 15.76MB read
Requests/sec:   4462.22
Transfer/sec:      1.57MB
```
