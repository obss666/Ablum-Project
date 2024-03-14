#ifndef CONST_H
#define CONST_H

enum TreeItemType{
    TreeItemPro = 1, //项目 根目录
    TreeItemDir = 2, //文件夹
    TreeItemPic = 3, //图片
};

enum PicBtnState{
    PicBtnStateNormal = 1,
    PicBtnStateHover = 2,
    PicBtnStatePress = 3,

    PicBtnState2Normal = 4,
    PicBtnState2Hover = 5,
    PicBtnState2Press = 6,
};

const int PROGRESS_WIDTH = 300;
const int PROGRESS_MAX = 300;             // 最大进度条
const int PREITEM_SIZE = 100;
const int PREICON_SIZE = 90;
#endif // CONST_H
