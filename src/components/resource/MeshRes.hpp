#pragma once

#include "stl/CreviceSharedPtr.h"
#include "resource/Model.h"
#include "common/Resource.h"

struct MeshRes
{
    RID rid;
    crevice::SharedPtr<Mesh> data;
};
