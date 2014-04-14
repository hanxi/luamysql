# lua driver for mysql

------

在lua中操作mysql，封装基本的操作接口。

操作接口：

>* 创建连接：mysql.connect (host,user,pass,dbname,dbport)
>* 关闭连接：db:close()
>* 查询：db:query(sql)
>* 插入：db:insert(sql)
>* 更新：db:update(sql)
>* 设置语言：db:character_set(character_type)


