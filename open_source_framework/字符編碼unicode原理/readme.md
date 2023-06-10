# 字符編碼

用什麼編碼方式表示字符


# Unicode 字符集，每個字符都有編號
```
碼表                            編碼
                        utf8 utf16 utf32 


```
unicode 編碼16進製

utf-8一種可變長的編碼方案，使用1-6個字節來存儲。

utf32 一種固定長度的編碼方法，不管字符編號大小，始終使用4個字節來存儲

utf-16 介於8和32之間使用2-4個字符來存儲，長度既固定又可變

# 前綴

utf-8 是可變長度，如何區分他用了多少個字節
```
0xxxxxxx第一個字節 110xxxxx第二個字節 10xxxxxx 1110xxxx第三個字節 10xxxxxx 10xxxxxx 11110xxx第4個字節 10xxxxxx 10xxxxxx 10xxxxxx

前綴，目的是用來區分我們utf8表示一個unicode用了多少字節
```

# 大小端

UTF8編碼 一律採用大端

UTF16BE編碼 FEFF <- 這個是大端

UTF16LE FFFE <- 這個是小端

# 小結

程序打開文件，如何知道文件使用的是utf8 還是16呢？

在文件的開頭有幾個字節是標誌

比如：

```
EF BB BF -> UTF8
FE FF -> UTF-16BE
FF FE -> UTF-16LE
00 00 FE FF -> UTF32-BE
FF FE 00 00 -> UTF32-LE

只有 UTF-8 兼容ASCII, UTF-32 和 UTF-16 都不兼容 ASCII, 因為他們都沒有單字節編碼

在linux 中 用 命令

file -i xxx.txt 

可以查看編碼方式
```

# 轉換文件的編碼方式

```
iconv -f UTF-8 -t UTF-16BE xxx.txt -o res.txt 
```

# 能否在C++代碼中 轉編碼方式呢？

可以，利用linux自帶的iconv.h header可以做到，但實際工程中沒有人會這麼做。
```
CharsetConverter 字符轉換類
code-notepad	用notepad編輯文件編碼
iconv			用iconv命令轉換文件
mysql			utf 和 utfmb4 的區別
nginx			nginx中文亂碼問題
charset_converter.c 字符轉換測試
utf8_to_utf16.c     字符轉換測試
```

如果數據庫要存中文 最好用utf8mb，utf8 有些漢子顯示不了

