#ifndef __UT_DDS_CALLBACK_HPP__
#define __UT_DDS_CALLBACK_HPP__

#include "common/decl.hpp"

typedef std::function<void(const void*)> DdsMessageHandler;
typedef std::function<void(int)> DdsMessageWriterChangeHandler;

namespace unitree
{
namespace common
{
class DdsReaderCallback
{
public:
    DdsReaderCallback();
    DdsReaderCallback(const DdsMessageHandler& messageHandler);
    DdsReaderCallback(const DdsMessageWriterChangeHandler& messageWriterChangeHandler);
    DdsReaderCallback(const DdsMessageHandler& messageHandler, const DdsMessageWriterChangeHandler& messageWriterChangeHandler);
    DdsReaderCallback(const DdsReaderCallback& rcb);
    DdsReaderCallback& operator=(const DdsReaderCallback& rcb);

    ~DdsReaderCallback();

public:
    void Set(const DdsMessageHandler& messageHandler);
    void Set(const DdsMessageWriterChangeHandler& messageWriterChangeHandler);
    void Set(const DdsMessageHandler& messageHandler, const DdsMessageWriterChangeHandler& messageWriterChangeHandler);

    bool HasMessageHandler() const;
    bool HasMessageWriterChangeHandler() const;

    void DoMessageHandler(const void* message);
    void DoMessageWriterChangeHandler(int count);

private:
    DdsMessageHandler mMessageHandler;
    DdsMessageWriterChangeHandler mMessageWriterChangeHandler;
};

}
}

#endif//__UT_DDS_CALLBACK_HPP__
