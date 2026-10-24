#ifndef MODEL_POSE_H
#define MODEL_POSE_H

#include "vector3.h"

struct ModelPose
{
    ShortVector3 rotation[3];
    ShortVector3 translation;

public:
    void Compose(const ModelPose &left, const ModelPose &right);
    void SetRelative(const ModelPose &left, const ModelPose &right);
    void CaptureHd(MATRIXF &matrix) const;
};

#endif
