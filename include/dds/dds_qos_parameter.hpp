#ifndef __UT_DDS_QOS_PARAMETER_HPP__
#define __UT_DDS_QOS_PARAMETER_HPP__

#include "dds_qos_policy_parameter.hpp"

namespace unitree
{
namespace common
{
class DdsQosParameter
{
public:
    DdsQosParameter() :
        mDefault(true)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        int32_t count = 0;
        JsonMap::const_iterator iter;

#define __INIT_POLICY(NAME, POLICY)                     \
        iter = data.find(NAME);                         \
        if (iter != data.end())                         \
        {                                               \
            const Any& a = iter->second;                \
            if (!a.Empty())                             \
            {                                           \
                const JsonMap& m = AnyCast<JsonMap>(a); \
                if (!m.empty())                         \
                {                                       \
                    m##POLICY.Init(m);                  \
                    count++;                            \
                }                                       \
            }                                           \
        }

        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_DEADLINE, Deadline)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_DESTINATION_ORDER, DestinationOrder)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_DURABILITY, Durability)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_DURABILITY_SERVICE, DurabilityService)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_ENTITY_FACTORY, EntityFactory)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_GROUP_DATA, GroupData)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_HISTORY, History)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_LATENCY_BUDGET, LatencyBudget)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_LIFESPAN, Lifespan)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_LIVELINESS, Liveliness)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_OWNERSHIP, Ownership)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_OWNERSHIP_STRENGTH, OwnershipStrength)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_PARTITION, Partition)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_PRESENTATION, Presentation)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_READER_DATA_LIFECYCLE, ReaderDataLifecycle)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_RELIABILITY, Reliability)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_RESOURCE_LIMITS, ResourceLimits)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_TIME_BASED_FILTER, TimeBasedFilter)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_TOPIC_DATA, TopicData)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_TRANSPORT_PRIORITY, TransportPriority)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_WRITER_DATA_LIFECYCLE, WriterDataLifecycle)
        __INIT_POLICY(UT_DDS_QOS_POLICY_NAME_USER_DATA, UserData)

#undef __INIT_POLICY

        if (count > 0)
        {
            mDefault = false;
        }
    }

#define __GET_POLICY(POLICY)                                        \
    const DdsQos##POLICY##PolicyParameter& Get##POLICY() const  \
    {                                                           \
        return m##POLICY;                                       \
    }

    __GET_POLICY(Deadline)
    __GET_POLICY(DestinationOrder)
    __GET_POLICY(Durability)
    __GET_POLICY(DurabilityService)
    __GET_POLICY(EntityFactory)
    __GET_POLICY(GroupData)
    __GET_POLICY(History)
    __GET_POLICY(LatencyBudget)
    __GET_POLICY(Lifespan)
    __GET_POLICY(Liveliness)
    __GET_POLICY(Ownership)
    __GET_POLICY(OwnershipStrength)
    __GET_POLICY(Partition)
    __GET_POLICY(Presentation)
    __GET_POLICY(ReaderDataLifecycle)
    __GET_POLICY(Reliability)
    __GET_POLICY(ResourceLimits)
    __GET_POLICY(TimeBasedFilter)
    __GET_POLICY(TopicData)
    __GET_POLICY(TransportPriority)
    __GET_POLICY(WriterDataLifecycle)
    __GET_POLICY(UserData)

#undef __GET_POLICY

    bool Default() const
    {
        return mDefault;
    }

private:
    bool mDefault;

    DdsQosDeadlinePolicyParameter mDeadline;
    DdsQosDestinationOrderPolicyParameter mDestinationOrder;
    DdsQosDurabilityPolicyParameter mDurability;
    DdsQosDurabilityServicePolicyParameter mDurabilityService;
    DdsQosEntityFactoryPolicyParameter mEntityFactory;
    DdsQosGroupDataPolicyParameter mGroupData;
    DdsQosHistoryPolicyParameter mHistory;
    DdsQosLatencyBudgetPolicyParameter mLatencyBudget;
    DdsQosLifespanPolicyParameter mLifespan;
    DdsQosLivelinessPolicyParameter mLiveliness;
    DdsQosOwnershipPolicyParameter mOwnership;
    DdsQosOwnershipStrengthPolicyParameter mOwnershipStrength;
    DdsQosPartitionPolicyParameter mPartition;
    DdsQosPresentationPolicyParameter mPresentation;
    DdsQosReaderDataLifecyclePolicyParameter mReaderDataLifecycle;
    DdsQosReliabilityPolicyParameter mReliability;
    DdsQosResourceLimitsPolicyParameter mResourceLimits;
    DdsQosTimeBasedFilterPolicyParameter mTimeBasedFilter;
    DdsQosTopicDataPolicyParameter mTopicData;
    DdsQosTransportPriorityPolicyParameter mTransportPriority;
    DdsQosWriterDataLifecyclePolicyParameter mWriterDataLifecycle;
    DdsQosUserDataPolicyParameter mUserData;
};

}
}

#endif//__UT_DDS_QOS_PARAMETER_HPP__
