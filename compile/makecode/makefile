src = $(wildcard ./*.c) # 获取当前路径下的.c文件
obj = $(patsubst %.c, %.o, $(src))
target = app
$(target) : $(obj)
	gcc  $^ -o $@

%.o : %.c
	gcc -c $< -o $@

.PHONY : clean
clean :
	-rm $(obj) $(target) -f
