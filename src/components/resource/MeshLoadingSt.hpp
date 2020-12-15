#pragma once

#include "common/Resource.h"
#include <future>
#include "resource/Model.h"
#include "stl/CreviceSharedPtr.h"

struct MeshLoadingSt
{
    /* data */
    RID rid;
    bool loading = false;
    std::shared_future<crevice::SharedPtr<Mesh>> meshF;
};
