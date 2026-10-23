#ifndef MODEL_MATRIX_H
#define MODEL_MATRIX_H

#include "vector3.h"

extern "C"
{
#include "psx.h"
}

void InitializeModelIdentityMatrix(MATRIX &matrix);
void TransformModelMatrix(const MATRIX &input, MATRIX &output);
void ClearModelRotationMatrix(MATRIX &output);
void CopyModelRotationMatrix(const MATRIX &input, MATRIX &output);
void TransposeRotationMatrix(MATRIX &matrix);
void TransposeRotationMatrix(const MATRIX &input, MATRIX &output);
void TransposeModelMatrix(const MATRIX &input, MATRIX &output);

#endif
