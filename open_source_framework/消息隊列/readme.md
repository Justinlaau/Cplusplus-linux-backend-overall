# 觀察者模式 與 發佈訂閱模式的分別

觀察者模式是被觀察者與觀察者之間直接互動，而發佈訂閱是 發佈者 是依靠中間件 比如 消息隊列 來通知訂閱者的。

所以發佈訂閱是完全解耦的，就是發佈者並不需要知道訂閱者是誰。


# 那 消息隊列就不用加API嗎？

消息隊列 大部分都有api提供，不用自己寫

![alt text](./../../image/%E7%99%BC%E4%BD%88%E8%A8%82%E9%96%B1.png)

# 消息隊列-高並發緩衝

- kafka 日誌服務，監控上報

# useful link
1. https://zhuanlan.zhihu.com/p/372205805

# 消息隊列-基本概念和原理1

1. broker
    - Broker的概念來自於 Apache ActiveMQ，通俗來講就是MQ的服務器，中間人，券商

2. 消息的生產者和消費者
    - producer： 發送消息到消息隊列
    - consumer： 從消息隊列收取信息

3. 點對點消息隊列模型
    - producer 向特定的隊列發送消息，consumer從中收取
    - producer和consumer可以不同時處於運行狀態，只要broker不死就可以
    - 每一個成功處理的消息都有consumer 簽收確認，返回ack
    - e.g rabbitmq 雖然只是點對點不能通知多台服務器，不過可以透過交換機達到擴張效果。
    ![alt text](./../../image/%E9%BB%9E%E5%88%B0%E9%BB%9E.png)

4. 發佈訂閱消息模型-Topic

    - 發佈訂閱消息模型中，支持像一個特定的主題Topic發佈消息，0個或多個訂閱者接受來自這個消息主題的消息。在這種模型下，發佈者和訂閱者彼此不知道對方。
 ![alt text](./../../image/%E7%99%BC%E4%BD%88%E8%A8%82%E9%96%B1x.png)

5. 消息順序性保證
    - 基於Queue，利用fifo特性

6. 消息的ACK確認機制
    - 一旦消息隊列收到consumer的確認號，就會把消息刪除，所以消費者必須在把業務處理完才發送確認號，不然業務過程出現錯誤將永遠丟失消息。

7. 消息的持久化
    - 消息持久化，對於一些核心的業務非常重要，一旦消息隊列死機後，不能恢復消息隊列中的消息就會變得很不安全，而消息持久化可以讓消息進行持久化存儲並且恢復數據。信息一旦進入消息隊列，就會存儲在disk中，才會到出口讓消費者消費。

8. 消息的同步和異步收發
    - 為什麼發消息要用異步而不是同步？
    - 原因是個tcp ip一樣的，發一次應答一次，吞吐量會非常低。
    為了增加吞吐量，採用異步的方式，同時發出多個消息，並且收取多個應答。

    - 再比如數據要持久化，不可能每條消息都刷一次硬盤，肯定是批量的刷。s
    - 再來看一個場景，A 系統接收一個請求，需要在自己本地寫庫，還需要在BCD 三個系統寫庫，自己本地寫庫要3ms，BCD 三個系統分別寫庫要300ms、450ms、200ms。最終請求總延時是3 + 300 + 450 + 200 = 953ms，接近1s，用戶感覺搞個什麼東西，慢死了慢死了。用戶通過瀏覽器發起請求，等待個1s，這幾乎是不可接受的。

    - 一般互聯網類的企業，對於用戶直接的操作，一般要求是每個請求都必須在200 ms 以內完成，對用戶幾乎是無感知的。

如果使用MQ，那麼A 系統連續發送3 條消息到MQ 隊列中，假如耗時5ms，A 系統從接受一個請求到返迴響應給用戶，總時長是3 + 5 = 8ms，對於用戶而言，其實感覺上就是點個按鈕，8ms 以後就直接返回了，爽！網站做得真好，真快！

 ![alt text](./../../image/%E7%95%B0%E6%AD%A5%E5%8D%80%E5%88%A51.png)
 
 ![alt text](./../../image/%E7%95%B0%E6%AD%A5%E5%8D%80%E5%88%A52.png)

    - 上面兩個圖片很明顯的告訴你，如果A系統自己也要負責確認BCD是否收到信息，將會死無全尸，因為要等BCD處理完業務後，再發ACK，那麼我收到ACK的時候已經快天黑了。
    - 如果把這些工作給MQ做，並且使用異步，就可以免去不同服務器的性能差別帶來的負面影響。

9. 消息的事務支持
    

 ![alt text](./../../image/%E4%B8%8D%E5%90%8C%E7%9A%84%E6%B6%88%E6%81%AF%E9%9A%8A%E5%88%97%E7%94%A2%E5%93%81.png)



# ZeroMQ的特性

它是嵌入在你的程式裡的，就是他就是一個api，不能獨立存在，當然你可以自己開一個服務包裝一下。

ZEROMQ 也不支持持久化

```
                A 系統
               集成了PUB
            /             \
          B SUB         C SUB
```


# TCP 是一對一的， 一對多就要設計Reactor 模式，Proactor 不講，異步實現太難了

# 網絡編程常見問題

- 調用的socket接口較多
- TCP 是一對一
- 不支持跨平台編程
- 需要自行處理分包，組包問題
- 流式傳輸時需要處理的粘包和半包問題
- 需要自行處理網絡異常，e.g 連接異常中斷，重連
- 服務端和客戶端啟動有先後
- 自行處理IO模型
- 自行實現消息的緩存
- 自行實現對消息的加密

# ZeroMQ 都解決了以上的問題


# ZeroMQ的安裝和編譯
去zeromq官網下載代碼

必要的庫
```
sudo apt-get install libtool
sudo apt-get install pkg-config
sudo apt-get install build-essential
sudo apt-get install autoconf
sudo apt-get install automake
```

加密庫 Sodium
```
git clone git://github.com/jedisct1/libsodium.git
cd libsodium 
./autogen.sh -s 
./configure && make check
sudo make install 
sudo ldconfig
cd ..
```

編譯和下載source code 不要用master branch 有問題的

```
git clone https://github.com/zeromq/libzmq.git 
cd libzmq 
git tag 
git checkout v4.3.2 8 
./autogen.sh 
./configure && make check
sudo make install 
sudo ldconfig
cd ..
```

# 基本設計模式


 ![alt text](./../../image/%E7%99%BC%E4%BD%88%E8%80%85%E8%A8%82%E9%96%B1%E8%80%85%E5%9C%96.png)

  ![alt text](./../../image/%E6%8E%A8%E6%8B%89%E8%88%87%E7%B6%B2%E9%97%9C%E6%A8%A1%E5%BC%8F.png)