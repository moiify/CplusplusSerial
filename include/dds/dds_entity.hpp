#ifndef __UT_DDS_ENTITY_HPP__
#define __UT_DDS_ENTITY_HPP__

#include <cstdlib>
#include <iostream>
#include <vector>
#include <dds/dds.hpp>
#include "common/log/log.hpp"
#include "common/thread/thread.hpp"
#include "common/time/sleep.hpp"
#include "dds_exception.hpp"
#include "dds_callback.hpp"
#include "dds_qos.hpp"

#define __UT_DDS_NULL__ ::dds::core::null

#define __UT_DDS_WAIT_MATCHED_TIME_SLICE 10

using namespace org::eclipse::cyclonedds;

namespace unitree
{
namespace common
{
class DdsLogger
{
public:
    DdsLogger()
    {
        mDdsLogger = GetLogger("/unitree/dds/dds_entity_wrapper");
    }

protected:
    Logger* mDdsLogger;
};

class DdsParticipant : public DdsLogger
{
public:
    typedef ::dds::domain::DomainParticipant NATIVE_TYPE;

    DdsParticipant(uint32_t domainId, const DdsParticipantQos* qos = NULL, const std::string& config = "") :
        mNative(__UT_DDS_NULL__)
    {
        UT_DDS_EXCEPTION_TRY

        auto particQos = ::dds::domain::DomainParticipant::default_participant_qos();
        if (qos != NULL)
        {
            particQos = qos->GetNative();
        }

        if (config.empty())
        {
            mNative = NATIVE_TYPE(domainId, particQos);
        }
        else
        {
            mNative = NATIVE_TYPE(domainId, particQos, NULL, ::dds::core::status::StatusMask::none(), config);
        }

        UT_DDS_EXCEPTION_CATCH(mDdsLogger, true)
    }

    ~DdsParticipant()
    {
        mNative = __UT_DDS_NULL__;
    }

    const NATIVE_TYPE& GetNative() const
    {
        return mNative;
    }

private:
    NATIVE_TYPE mNative;
};

class DdsPublisher : public DdsLogger
{
public:
    typedef ::dds::pub::Publisher NATIVE_TYPE;

    DdsPublisher(const DdsParticipant* participant, const DdsPublisherQos* qos = NULL) :
        mNative(__UT_DDS_NULL__)
    {
        UT_DDS_EXCEPTION_TRY

        auto pubQos = participant->GetNative().default_publisher_qos();
        if (qos != NULL)
        {
            pubQos = qos->GetNative();
        }

        mNative = NATIVE_TYPE(participant->GetNative(), pubQos);

        UT_DDS_EXCEPTION_CATCH(mDdsLogger, true)
    }

    ~DdsPublisher()
    {
        mNative = __UT_DDS_NULL__;
    }

    const NATIVE_TYPE& GetNative() const
    {
        return mNative;
    }

private:
    NATIVE_TYPE mNative;
};

class DdsSubscriber : public DdsLogger
{
public:
    typedef ::dds::sub::Subscriber NATIVE_TYPE;

    DdsSubscriber(const DdsParticipant* participant, const DdsSubscriberQos* qos = NULL) :
        mNative(__UT_DDS_NULL__)
    {
        UT_DDS_EXCEPTION_TRY

        auto subQos = participant->GetNative().default_subscriber_qos();
        if (qos != NULL)
        {
            subQos = qos->GetNative();
        }

        mNative = NATIVE_TYPE(participant->GetNative(), subQos);

        UT_DDS_EXCEPTION_CATCH(mDdsLogger, true)
    }

    ~DdsSubscriber()
    {
        mNative = __UT_DDS_NULL__;
    }

    const NATIVE_TYPE& GetNative() const
    {
        return mNative;
    }

private:
    NATIVE_TYPE mNative;
};

template<typename MSG>
class DdsTopic : public DdsLogger
{
public:
    typedef ::dds::topic::Topic<MSG> NATIVE_TYPE;

    DdsTopic(const DdsParticipant* participant, const std::string& name, const DdsTopicQos* qos = NULL) :
        mNative(__UT_DDS_NULL__)
    {
        UT_DDS_EXCEPTION_TRY

        auto topicQos = participant->GetNative().default_topic_qos();
        if (qos != NULL)
        {
            topicQos = qos->GetNative();
        }

        ::dds::core::policy::History history;

        history.kind();
        history.depth(1);
        topicQos.policy(history);

        mNative = NATIVE_TYPE(participant->GetNative(), name, topicQos);

        UT_DDS_EXCEPTION_CATCH(mDdsLogger, true)
    }

    ~DdsTopic()
    {
        mNative = __UT_DDS_NULL__;
    }

    const NATIVE_TYPE& GetNative() const
    {
        return mNative;
    }

private:
    NATIVE_TYPE mNative;
};

template<typename MSG>
class DdsWriter : public DdsLogger
{
public:
    typedef ::dds::pub::DataWriter<MSG> NATIVE_TYPE;

    DdsWriter(const DdsPublisher* publisher, const DdsTopic<MSG>* topic, const DdsWriterQos* qos = NULL) :
        mNative(__UT_DDS_NULL__)
    {
        UT_DDS_EXCEPTION_TRY

        auto writerQos = publisher->GetNative().default_datawriter_qos();
        if (qos != NULL)
        {
            writerQos = qos->GetNative();
        }

        mNative = NATIVE_TYPE(publisher->GetNative(), topic->GetNative(), writerQos);

        UT_DDS_EXCEPTION_CATCH(mDdsLogger, true)
    }

    ~DdsWriter()
    {
        mNative = __UT_DDS_NULL__;
    }

    const NATIVE_TYPE& GetNative() const
    {
        return mNative;
    }

    bool Write(const MSG& message, bool matchReader)
    {
        if (matchReader && mNative.publication_matched_status().current_count() <= 0)
        {
            return false;
        }

        UT_DDS_EXCEPTION_TRY
        {
            mNative.write(message);
            return true;
        }

        UT_DDS_EXCEPTION_CATCH(mDdsLogger, false)

        return false;
    }

private:
    NATIVE_TYPE mNative;
};

template<typename MSG>
class DdsReaderListener : public ::dds::sub::NoOpDataReaderListener<MSG>, DdsLogger
{
public:
    typedef ::dds::sub::DataReaderListener<MSG> NATIVE_TYPE;
    DdsReaderListener()
    {
        mCb = NULL;
        mMask = ::dds::core::status::StatusMask::none();
    }

    ~DdsReaderListener()
    {
        delete mCb;
    }

    void SetCallback(const DdsReaderCallback* cb)
    {
        if (cb->HasMessageHandler())
        {
            mMask |= ::dds::core::status::StatusMask::data_available();
        }

        if (cb->HasMessageWriterChangeHandler())
        {
             mMask |= ::dds::core::status::StatusMask::liveliness_changed();
        }

        mCb = new DdsReaderCallback(*cb);
    }

    void on_data_available(::dds::sub::DataReader<MSG>& reader)
    {
        ::dds::sub::LoanedSamples<MSG> samples;

        UT_DDS_EXCEPTION_TRY
            samples = reader.take();
        UT_DDS_EXCEPTION_CATCH(mDdsLogger, false)

        if (samples.length() <= 0)
        {
            return;
        }

        typename ::dds::sub::LoanedSamples<MSG>::const_iterator iter;
        for (iter=samples.begin(); iter<samples.end(); ++iter)
        {
            const MSG& m = iter->data();
            if (iter->info().valid())
            {
                UT_EXCEPTION_TRY
                    mCb->DoMessageHandler((const void*)&m);
                UT_EXCEPTION_CATCH(mDdsLogger, false)
            }
        }
    }

    void on_liveliness_changed(::dds::sub::DataReader<MSG>& reader, const ::dds::core::status::LivelinessChangedStatus& status)
    {
        UT_EXCEPTION_TRY
            mCb->DoMessageWriterChangeHandler(status.alive_count());
        UT_EXCEPTION_CATCH(mDdsLogger, false)
    }

    NATIVE_TYPE* GetNative() const
    {
        return (NATIVE_TYPE*)this;
    }

    const ::dds::core::status::StatusMask& GetStatusMask() const
    {
        return mMask;
    }

private:
    DdsReaderCallback* mCb;
    ::dds::core::status::StatusMask mMask;
};

template<typename MSG>
class DdsReader : public DdsLogger
{
public:
    typedef ::dds::sub::DataReader<MSG> NATIVE_TYPE;

    DdsReader(const DdsSubscriber* subscriber, const DdsTopic<MSG>* topic, const DdsReaderQos* qos = NULL) :
        mNative(__UT_DDS_NULL__)
    {
        UT_DDS_EXCEPTION_TRY

        auto readerQos = subscriber->GetNative().default_datareader_qos();
        if (qos != NULL)
        {
            readerQos = qos->GetNative();
        }

        mNative = NATIVE_TYPE(subscriber->GetNative(), topic->GetNative(), readerQos);

        UT_DDS_EXCEPTION_CATCH(mDdsLogger, true)
    }

    ~DdsReader()
    {
        mNative = __UT_DDS_NULL__;
    }

    const NATIVE_TYPE& GetNative() const
    {
        return mNative;
    }

    void SetCallback(const DdsReaderCallback* cb)
    {
        mListener.SetCallback(cb);
        mNative.listener(mListener.GetNative(), mListener.GetStatusMask());
    }

private:
    NATIVE_TYPE mNative;
    DdsReaderListener<MSG> mListener;
};

}
}

#endif//__UT_DDS_ENTITY_HPP__
