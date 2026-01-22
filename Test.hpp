#include "RSPIP.h"

static void _TestForNormalImage() {
    // Test Normal Image Read/Show/Save
    auto normalImageName = "C:/Users/RSPIP/Pictures/Camera Roll/tempTest.png";
    auto normalSaveImagePath = "E:/RSPIP/GuoShuai/Resource/Temp/";
    auto normalSaveImageName = "NormalImageOutput.png";
    auto normalImage = RSPIP::IO::NormalImageRead(normalImageName);
    RSPIP:: Util::ShowImage(*normalImage);
    RSPIP::Util::PrintImageInfo(*normalImage);
    RSPIP:: IO::SaveImage(*normalImage, normalSaveImagePath, normalSaveImageName);
}
