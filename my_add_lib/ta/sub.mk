global-incdirs-y += include

# Add by YL
# -I../libs/algo/include
global-incdirs-y += ../libs/algo/include

srcs-y += my_add_lib_ta.c

# Add by YL
libnames += algo  # -lalgo
libdirs += ../libs/algo  # -L../libs/algo
#libdeps += ../libs/algo/libalgo.a
