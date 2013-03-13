cd output
fromelf -nodebug  -bin -output W828_V3.hyc W828.axf

MakeImage_HV image.hyc W828_V3.hyc W828_V3

//不用暂时先删掉
del W828_V3.hyc

//pause
