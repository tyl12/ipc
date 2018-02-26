CC = g++
# 可执行文件
# C文件
#TARGET = main
#SRCS = main.cpp

TARGET = fd
SRCS = fd.cpp
# 目标文件
OBJS = $(SRCS:.cpp=.o)
# 库文件
DLIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_highgui -lopencv_photo -lopencv_contrib -lopencv_imgproc -lopencv_stitching -lopencv_core       -lopencv_legacy    -lopencv_superres -lopencv_features2d -lopencv_ml        -lopencv_ts -lopencv_flann      -lopencv_objdetect -lopencv_video -lopencv_gpu        -lopencv_ocl       -lopencv_videostab


# -DGPU -fopenmp -lgomp
# 链接为可执行文件
$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(DLIBS)
clean:
	rm -rf $(TARGET) $(OBJS)
# 编译规则 $@代表目标文件 $< 代表第一个依赖文件
%.o:%.cpp
	$(CC) -o $@ -c $<
