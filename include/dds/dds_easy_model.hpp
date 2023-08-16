#ifndef __DDS_EASY_MODEL_HPP__
#define __DDS_EASY_MODEL_HPP__

#include "dds_topic_adapter.hpp"
#include "dds_parameter.hpp"
#include "dds_qos_realize.hpp"

#include "common/string_tool.hpp"

#define UT_DDS_PARAMETER_CONFIG_FILENAME "dds_parameter.json"

namespace unitree
{
namespace common
{
class DdsEasyModel
{
public:
    DdsEasyModel() :
        mQuit(false), mParticipant(NULL), mDefaultPublisher(NULL),
        mDefaultSubscriber(NULL)
    {
        mLogger = GetLogger("/unitree/dds/dds_easy_model");
    }

    ~DdsEasyModel()
    {
        mQuit = true;
        MilliSleep(100);

        std::map<std::string, DdsTopicAdapterAbstract*>::iterator iter;
        for (iter=mTopicAdapterMap.begin(); iter!=mTopicAdapterMap.end(); ++iter)
        {
            DdsTopicAdapterAbstract* topicAdapter = iter->second;
            topicAdapter->Release();
        }

        mTopicAdapterMap.clear();

        std::vector<DdsPublisher*>::iterator iterPub;
        for (iterPub=mPublisher.begin(); iterPub!=mPublisher.end(); ++iterPub)
        {
            DdsPublisher* publisher = *iterPub;
            __DDS_SAFE_DEL(publisher);
        }

        mPublisher.clear();

        std::vector<DdsSubscriber*>::iterator iterSub;
        for (iterSub=mSubscriber.begin(); iterSub!=mSubscriber.end(); ++iterPub)
        {
            DdsSubscriber* subscriber = *iterSub;
            __DDS_SAFE_DEL(subscriber);
        }

        mSubscriber.clear();

        __DDS_SAFE_DEL(mDefaultPublisher);
        __DDS_SAFE_DEL(mDefaultSubscriber);
    }

    void Init(const JsonMap& param)
    {
        mDdsParameter.Init(param);

        if (!mDdsParameter.Valid())
        {
            UT_THROW(CommonException, "dds parameter is not valid.");
        }

        //Participant
        const DdsParticipantParameter& partic = mDdsParameter.GetParticipant();

        DdsParticipantQos particQos;
        Realize(partic.GetQos(), particQos);

        std::string configFile = partic.GetConfig();
        if (!configFile.empty() && !StartWith(configFile, UT_PATH_DELIM_STR))
        {
            configFile = OsHelper::Instance()->GetProcessDirectory() + configFile;
        }

        mParticipant = new DdsParticipant(partic.GetDomainId(), &particQos, configFile);

        //Publisher
        const std::vector<DdsPublisherParameter>& vpp = mDdsParameter.GetPublisher();
        size_t i, count = vpp.size();

        for (i=0; i<count; i++)
        {
            const DdsPublisherParameter& pub = vpp[i];

            DdsPublisherQos pubQos;
            Realize(pub.GetQos(), pubQos);
            DdsPublisher* publisher = new DdsPublisher(mParticipant, &pubQos);

            mPublisher.push_back(publisher);
        }

        //Subscribe
        const std::vector<DdsSubscriberParameter>& vsp = mDdsParameter.GetSubscriber();
        count = vsp.size();

        for (i=0; i<count; i++)
        {
            const DdsSubscriberParameter& sub = vsp[i];

            DdsSubscriberQos subQos;
            Realize(sub.GetQos(), subQos);
            DdsSubscriber* subscriber = new DdsSubscriber(mParticipant, &subQos);

            mSubscriber.push_back(subscriber);
        }

        LOG_INFO(mLogger, "Dds model inited from dds parameter");
    }

    void Init(uint32_t domainId)
    {
        mParticipant = new DdsParticipant(domainId);
        LOG_INFO(mLogger, "Dds model inited default. domainId:", domainId);
    }

    void Init(const std::string& ddsParameterFileName = "")
    {
        std::string fileName = ddsParameterFileName;
        if (fileName.empty())
        {
            fileName = OsHelper::Instance()->GetProcessDirectory() + UT_DDS_PARAMETER_CONFIG_FILENAME;
        }

        Any parameter = ParseJsonString(LoadFile(fileName));
        if (parameter.Empty())
        {
            UT_THROW(CommonException, std::string("load dds_parameter config file error. filename:") + fileName);
        }

        Init(AnyCast<JsonMap>(parameter));
    }

    template<typename MSG>
    void SetTopic(const std::string& topic, const DdsReaderCallback& rcb)
    {
        DdsReaderQos readerQos;
        const DdsSubscriber* subscriber = GetReaderDependence(topic, readerQos);

        std::map<std::string,DdsTopicAdapterAbstract*>::iterator iter = mTopicAdapterMap.find(topic);

        if (iter == mTopicAdapterMap.end())
        {
            DdsTopicAdapter<MSG>* topicAdapter = new DdsTopicAdapter<MSG>();

            DdsTopicQos topicQos;
            GetTopicDependence(topic, topicQos);

            topicAdapter->SetTopic(mParticipant, topic, &topicQos);
            topicAdapter->SetReader(subscriber, &readerQos, &rcb);

            mTopicAdapterMap[topic] = (DdsTopicAdapterAbstract*)topicAdapter;
        }
        else
        {
            DdsTopicAdapter<MSG>* topicAdapter = (DdsTopicAdapter<MSG>*)iter->second;

            if (topicAdapter->GetReader())
            {
                UT_THROW(CommonException, std::string("topic reader is alreay exist. topic:") + topic);
            }

            topicAdapter->SetReader(subscriber, &readerQos, &rcb);
        }
    }

    template<typename MSG>
    void SetTopic(const std::string& topic, const DdsMessageHandler& handler)
    {
        DdsReaderCallback cb(handler);
        SetTopic<MSG>(topic, cb);
    }

    template<typename MSG>
    void SetTopic(const std::string& topic)
    {
        DdsWriterQos writerQos;
        const DdsPublisher* publisher = GetWriterDependence(topic, writerQos);

        std::map<std::string,DdsTopicAdapterAbstract*>::iterator iter = mTopicAdapterMap.find(topic);

        if (iter == mTopicAdapterMap.end())
        {
            DdsTopicAdapter<MSG>* topicAdapter = new DdsTopicAdapter<MSG>();

            DdsTopicQos topicQos;
            GetTopicDependence(topic, topicQos);

            topicAdapter->SetTopic(mParticipant, topic, &topicQos);
            topicAdapter->SetWriter(publisher, &writerQos);

            mTopicAdapterMap[topic] = (DdsTopicAdapterAbstract*)topicAdapter;
        }
        else
        {
            DdsTopicAdapter<MSG>* topicAdapter = (DdsTopicAdapter<MSG>*)iter->second;

            if (topicAdapter->GetWriter())
            {
                UT_THROW(CommonException, std::string("topic writer is already exist. topic:") + topic);
            }

            topicAdapter->SetWriter(publisher, &writerQos);
        }
    }

    template<typename MSG>
    bool WriteMessage(const std::string topic, const MSG& message, bool matchReader = true)
    {
        if (mQuit)
        {
            LOG_ERROR(mLogger, "WriteMessage error because QUIT. topic:", topic);
            return false;
        }

        std::map<std::string,DdsTopicAdapterAbstract*>::iterator iter = mTopicAdapterMap.find(topic);
        if (iter == mTopicAdapterMap.end())
        {
            return false;
        }

        DdsTopicAdapter<MSG>* topicAdapter = (DdsTopicAdapter<MSG>*)iter->second;
        if (topicAdapter == NULL)
        {
            return false;
        }

        DdsWriter<MSG>* writer = topicAdapter->GetWriter();
        if (writer == NULL)
        {
            return false;
        }

        return writer->Write(message, matchReader);
    }

private:
    void GetTopicDependence(const std::string& topic, DdsTopicQos& qos)
    {
        const std::map<std::string,DdsTopicParameter>& topics = mDdsParameter.GetTopic();
        std::map<std::string,DdsTopicParameter>::const_iterator iter = topics.find(topic);

        if (iter != topics.end())
        {
            const DdsTopicParameter& param = iter->second;
            Realize(param.GetQos(), qos);
        }
    }

    const DdsSubscriber* GetReaderDependence(const std::string& topic, DdsReaderQos& qos)
    {
        const std::vector<DdsSubscriberParameter>& vsp = mDdsParameter.GetSubscriber();
        size_t i, icount = vsp.size();

        for (i=0; i<icount; i++)
        {
            const std::vector<DdsReaderParameter>& vrp = vsp[i].GetReader();
            size_t j, jcount = vrp.size();

            for (j=0; j<jcount; j++)
            {
                const DdsReaderParameter& reader = vrp[j];

                if (reader.GetTopicName() == topic)
                {
                    Realize(reader.GetQos(), qos);
                    return mSubscriber[i];
                }
            }
        }

        return GetDefaultSubscriber();
    }

    const DdsPublisher* GetWriterDependence(const std::string& topic, DdsWriterQos& qos)
    {
        const std::vector<DdsPublisherParameter>& vpp = mDdsParameter.GetPublisher();
        size_t i, count = vpp.size();

        for (i=0; i<count; i++)
        {
            const std::vector<DdsWriterParameter>& vwp = vpp[i].GetWriter();
            size_t j, jcount = vwp.size();

            for (j=0; j<jcount; j++)
            {
                const DdsWriterParameter& writer = vwp[j];
                if (writer.GetTopicName() == topic)
                {
                    Realize(writer.GetQos(), qos);
                    return mPublisher[i];
                }
            }
        }

        return GetDefaultPublisher();;
    }

    const DdsPublisher* GetDefaultPublisher()
    {
        //std::cout << "use default publisher" << std::endl;
        if (mDefaultPublisher == NULL)
        {
            mDefaultPublisher = new DdsPublisher(mParticipant);
        }

        return mDefaultPublisher;
    }

    const DdsSubscriber* GetDefaultSubscriber()
    {
        //std::cout << "use default subscriber" << std::endl;
        if (mDefaultSubscriber == NULL)
        {
            mDefaultSubscriber = new DdsSubscriber(mParticipant);
        }

        return mDefaultSubscriber;
    }

private:
    bool mQuit;

    DdsParameter mDdsParameter;

    DdsParticipant* mParticipant;
    DdsPublisher* mDefaultPublisher;
    DdsSubscriber* mDefaultSubscriber;

    std::vector<DdsPublisher*> mPublisher;
    std::vector<DdsSubscriber*> mSubscriber;

    std::map<std::string,DdsTopicAdapterAbstract*> mTopicAdapterMap;

    Logger *mLogger;
};

typedef std::shared_ptr<DdsEasyModel> DdsEasyModelPtr;

}
}

#endif//__DDS_EASY_MODEL_HPP__
