src = $(wildcard ./*.cpp) # 获取当前路径下的.c文件
obj = $(patsubst %.cpp, %.o, $(src))
target = app
$(target) : $(obj)
	gcc  $^ -o $@ -lstdc++ 
%.o : %.c
	gcc -c $< -o $@
.PHONY : clean
clean :
	-rm $(obj) $(target) -f
