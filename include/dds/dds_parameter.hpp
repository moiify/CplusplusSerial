#ifndef __UT_DDS_PARAMETER_HPP__
#define __UT_DDS_PARAMETER_HPP__

#include "common/decl.hpp"
#include "dds_qos_parameter.hpp"

namespace unitree
{
namespace common
{
class DdsQosParameterHolder
{
public:
    DdsQosParameterHolder();

    virtual ~DdsQosParameterHolder();

    void SetQos(const DdsQosParameter& qos);
    const DdsQosParameter& GetQos() const;

    bool DefaultQos() const;

protected:
    DdsQosParameter mQos;
};

class DdsParticipantParameter : public DdsQosParameterHolder
{
public:
    DdsParticipantParameter();
    DdsParticipantParameter(uint32_t domainId, const std::string& config = "");

    ~DdsParticipantParameter();

    void SetDomainId(int32_t domainId);
    uint32_t GetDomainId() const;

    void SetConfig(const std::string& config);
    const std::string& GetConfig() const;

private:
    uint32_t mDomainId;
    std::string mConfig;
};

class DdsTopicParameter : public DdsQosParameterHolder
{
public:
    DdsTopicParameter();
    DdsTopicParameter(const std::string& name);

    ~DdsTopicParameter();

    void SetName(const std::string& name);
    const std::string& GetName() const;

private:
    std::string mName;
};

class DdsTopicParameterHolder
{
public:
    DdsTopicParameterHolder();
    DdsTopicParameterHolder(const std::string& topicName);

    virtual ~DdsTopicParameterHolder();

    void SetTopicName(const std::string& topicName);
    const std::string& GetTopicName() const;

private:
    std::string mTopicName;
};

class DdsWriterParameter : public DdsTopicParameterHolder, public DdsQosParameterHolder
{
public:
    DdsWriterParameter();
    DdsWriterParameter(const std::string& topicName);

    ~DdsWriterParameter();
};

class DdsWriterParameterHolder
{
public:
    DdsWriterParameterHolder();

    virtual ~DdsWriterParameterHolder();

    void SetWriter(const DdsWriterParameter& writer);
    const DdsWriterParameter& GetWriter() const;

private:
    DdsWriterParameter mWriter;
};

class DdsReaderParameter : public DdsTopicParameterHolder, public DdsQosParameterHolder
{
public:
    DdsReaderParameter();
    DdsReaderParameter(const std::string& topicName);

    virtual ~DdsReaderParameter();
};

class DdsReaderParameterHolder
{
public:
    DdsReaderParameterHolder();

    virtual ~DdsReaderParameterHolder();

    void SetReader(const DdsReaderParameter& reader);
    const DdsReaderParameter& GetReader() const;

private:
    DdsReaderParameter mReader;
};

class DdsPublisherParameter : public DdsQosParameterHolder
{
public:
    DdsPublisherParameter();

    ~DdsPublisherParameter();

    void AppendWriter(const DdsWriterParameter& writer);
    void SetWriter(const std::vector<DdsWriterParameter>& writer);
    const std::vector<DdsWriterParameter>& GetWriter() const;

private:
    std::vector<DdsWriterParameter> mWriter;
};

class DdsSubscriberParameter : public DdsQosParameterHolder
{
public:
    DdsSubscriberParameter();
    ~DdsSubscriberParameter();

    void AppendReader(const DdsReaderParameter& reader);
    void SetReader(const std::vector<DdsReaderParameter>& reader);
    const std::vector<DdsReaderParameter>& GetReader() const;

private:
    std::vector<DdsReaderParameter> mReader;
};

class DdsParameter
{
public:
    DdsParameter();
    DdsParameter(const JsonMap& param);

    ~DdsParameter();

    void Init(const JsonMap& param);

    const DdsParticipantParameter& GetParticipant();

    void AppendTopic(const DdsTopicParameter& topic);
    const std::map<std::string,DdsTopicParameter>& GetTopic() const;

    void AppendPublisher(const DdsPublisherParameter& publisher);
    void SetPublisher(const std::vector<DdsPublisherParameter>& publisher);
    const std::vector<DdsPublisherParameter>& GetPublisher() const;

    void AppendSubscriber(const DdsSubscriberParameter& subscriber);
    void SetSubscriber(const std::vector<DdsSubscriberParameter>& subscriber);
    const std::vector<DdsSubscriberParameter>& GetSubscriber() const;

    bool Valid();

private:
    DdsParticipantParameter mParticipant;
    std::map<std::string,DdsTopicParameter> mTopic;
    std::vector<DdsPublisherParameter> mPublisher;
    std::vector<DdsSubscriberParameter> mSubscriber;
};

}
}
#endif//__UT_DDS_PARAMETER_HPP__
