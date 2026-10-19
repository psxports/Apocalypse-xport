#ifndef MODEL_MATRIX_H
#define MODEL_MATRIX_H

extern "C"
{
#include "psx.h"
}

void BuildModelRotationMatrix(const uint16 angles[3], MATRIX &matrix);
void ScaleModelRotationMatrix(const uint16 factors[3], MATRIX &matrix);
void InitializeModelIdentityMatrix(MATRIX &matrix);
void TransformModelMatrix(const MATRIX &input, MATRIX &output);
void ClearModelRotationMatrix(MATRIX &output);
void CopyModelRotationMatrix(const MATRIX &input, MATRIX &output);
void TransposeRotationMatrix(MATRIX &matrix);
void TransposeRotationMatrix(const MATRIX &input, MATRIX &output);
void TransposeModelMatrix(const MATRIX &input, MATRIX &output);

#endif
