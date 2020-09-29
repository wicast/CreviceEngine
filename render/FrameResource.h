#pragma once

#include "stl/CreviceSharedPtr.h"
#include "stl/CreviceVector.h"

namespace crevice
{
    
template <class R>
class FrameResource
{
    Vector<SharedPtr<R>> fRes;
    uint8_t latestIdx;
    bool updateEveryFrame;
    uint8_t swapChainSize;

public:
    bool prepared = false;

    FrameResource() {};

    FrameResource(R res, uint8_t chainSize) {
        fRes.resize(chainSize);
        for (auto i = 0; i < chainSize; i++)
        {
           fRes[i] = make_shared<R>(res);
        }
        swapChainSize = chainSize;
        prepared =true;
    };


    FrameResource(Vector<R> res) {
        for (auto r : res)
        {
            fRes.push_back(make_shared<R>(r));
        }
        swapChainSize = res.size();
        prepared = true;
    };

    FrameResource(bool updateEveryFrame,uint8_t chainSize)
    {
        swapChainSize = chainSize;
        if (updateEveryFrame)
        {
            fRes.resize(swapChainSize);
        }
        prepared=true;
    };

    FrameResource(SharedPtr<R> initR, bool updateEveryFrame,uint8_t chainSize)
    {
        swapChainSize = chainSize;
        fRes.push_back(initR);
        if (updateEveryFrame)
        {
            fRes.resize(swapChainSize);
        }
        prepared=true;
    };

    SharedPtr<R> getRes()
    {
        return fRes[latestIdx];
    };

    SharedPtr<R> getRes(uint64_t frameNo) {
        auto fn= frameNo % swapChainSize;
        return fRes[fn];
    };

    SharedPtr<R>* getForUpdate()
    {
        return &fRes[latestIdx];
    }

    SharedPtr<R>* getForUpdate(uint64_t frameNo)
    {
        latestIdx = frameNo % swapChainSize;
        if (latestIdx + 1 > fRes.size())
        {
            fRes.resize(swapChainSize);
        }

        return &fRes[latestIdx];
    }

    void updateOnce(uint64_t frameNo, SharedPtr<R> r)
    {
        fRes[latestIdx] = nullptr;
        latestIdx = frameNo % swapChainSize;
        fRes[latestIdx] = r;
    };

    void update(uint64_t frameNo, SharedPtr<R> r)
    {
        latestIdx = frameNo % swapChainSize;
        fRes[latestIdx] = r;
    };
};
} // namespace crevice

