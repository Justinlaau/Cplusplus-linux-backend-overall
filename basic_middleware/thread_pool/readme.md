# 什麼時候用線程池

1. IO異步操作
2. Logging存儲

# 怎麼實現線程池

1. 消費者
2. 生產者
3. 互斥

會寫C的就會寫C++的。


![alt text](./../../image/thread_pool.png)


# 對線程池的放縮

free_thread / sum_thread > 80%, release thread
free_thread / sum_thread < 40%, add thread