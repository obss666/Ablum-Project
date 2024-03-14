#ifndef CONST_H
#define CONST_H

enum TreeItemType{
    TreeItemPro = 1,
    TreeItemDir = 2,
    TreeItemPic = 3,
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
#endif // CONST_H
