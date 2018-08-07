
#ifndef VETSVMHOG
#define VETSVMHOG


//待分类标志牌的标准尺寸
#define VET_RED_WIDTH 32
#define VET_RED_HEIGHT 32

//待分类数字的标准尺寸
#define VET_DIGITAL_WIDTH 20
#define VET_DIGITAL_HEIGHT 20

//预先训练好的SVM模型
#define VET_MODEL_RED_FIRST "first_red_new_addnopark_auto.xml"
#define VET_MODEL_RED_SIGN_SECOND "second_red_new_addnopark_auto.xml"
#define VET_MODEL_DIGITAL "digital_color_auto.xml"

//始终以VET_FRAME_NUM帧为单位进行统计。
//在VET_FRAME_NUM帧中检测到大于等于VET_HOPE_NUM帧中存在某类交通标志，则认为开始检测到该类标志；
//在VET_FRAME_NUM帧中检测到小于等于VET_NOT_HOPE_NUM帧中存在某类交通标志，则认为未检测到该类标志；
#define VET_HOPE_NUM 3
#define VET_NOT_HOPE_NUM 2
#define VET_FRAME_NUM 5

//参考上段注释
#define VET_FRAME_NUM_light 10
#define VET_HOPE_NUM_light 7
#define VET_NOT_HOPE_NUM_light 5

//标志牌ROI区域
#define VETSKYROIX 350  
#define VETSKYROIY 50 
#define VETSKYROIWIDTH 900  
#define VETSKYROIHEIGHT 400  

//信号灯ROI区域
#define VETLIGHTSROIX 400  
#define VETLIGHTSROIY 230
#define VETLIGHTSROIWIDTH 500
#define VETLIGHTSROIHEIGHT 200

#define VETSIGNNUM 12  //待研究的交通标志种类数目 限速40、限速50、限速60、禁鸣喇叭等八种普通标志牌、信号灯。



#endif