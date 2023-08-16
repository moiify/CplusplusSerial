#ifndef __UT_DDS_TOPIC_ADAPTER_HPP__
#define __UT_DDS_TOPIC_ADAPTER_HPP__

#include <cstdlib>
#include <iostream>
#include <vector>
#include "dds_entity.hpp"

#ifndef __DDS_SAFE_DEL
#define __DDS_SAFE_DEL(x)       \
    if ((x) != NULL)            \
    {                           \
        delete (x); (x) = NULL; \
    }
#endif

namespace unitree
{
namespace common
{
class DdsTopicAdapterAbstract
{
public:
    virtual void Release() = 0;
};

template<typename MSG>
class DdsTopicAdapter : public DdsTopicAdapterAbstract
{
public:
    DdsTopicAdapter() :
        mTopic(NULL), mWriter(NULL), mReader(NULL)
    {}

    void SetTopic(const DdsParticipant* participant, const std::string& name, const DdsTopicQos* qos)
    {
        mTopic = new DdsTopic<MSG>(participant, name, qos);
    }

    void SetWriter(const DdsPublisher* publisher, const DdsWriterQos* qos)
    {
        mWriter = new DdsWriter<MSG>(publisher, mTopic, qos);
    }

    DdsWriter<MSG>* GetWriter() const
    {
        return mWriter;
    }

    void SetReader(const DdsSubscriber* subscriber, const DdsReaderQos* qos, const DdsReaderCallback* rcb)
    {
        mReader = new DdsReader<MSG>(subscriber, mTopic, qos);
        mReader->SetCallback(rcb);
    }

    DdsReader<MSG>* GetReader() const
    {
        return mReader;
    }

    bool Write(const MSG& message, bool matchReader)
    {
        return mWriter->Write(message, matchReader);
    }

    void Release()
    {
        __DDS_SAFE_DEL(mWriter);
        __DDS_SAFE_DEL(mReader);
        __DDS_SAFE_DEL(mTopic);

        this->~DdsTopicAdapter();
    }

private:
    ~DdsTopicAdapter()
    {}

private:
    DdsTopic<MSG>* mTopic;
    DdsWriter<MSG>* mWriter;
    DdsReader<MSG>* mReader;
};

}
}

#endif//__UT_DDS_TOPIC_ADAPTER_HPP__
