#ifndef __UT_DDS_QOS_POLICY_PARAMETER_HPP__
#define __UT_DDS_QOS_POLICY_PARAMETER_HPP__

#include <dds/dds.hpp>
#include "common/json/json.hpp"

namespace unitree
{
namespace common
{
/*
 *----------------------------
 * Supported Qos policy:
 * 1. Deadline
 *      IDL:
 *      struct DeadlineQosPolicy {
 *           Duration_t period;
 *      };
 *
 * 2. DestinationOrder
 *      IDL:
 *      enum DestinationOrderQosPolicyKind {
 *          BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS,
 *          BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS
 *      };
 *      struct DestinationOrderQosPolicy {
 *          DestinationOrderQosPolicyKind kind;
 *      };
 *
 * 3. Durability
 *      IDL:
 *      enum DurabilityQosPolicyKind {
 *          VOLATILE_DURABILITY_QOS, // Least Durability
 *          TRANSIENT_LOCAL_DURABILITY_QOS,
 *          TRANSIENT_DURABILITY_QOS,
 *          PERSISTENT_DURABILITY_QOS // Greatest Durability
 *      };
 *      struct DurabilityQosPolicy {
 *          DurabilityQosPolicyKind kind;
 *      };
 *
 * 4. DurabilityService
 *      IDL:
 *      struct DurabilityServiceQosPolicy {
 *          Duration_t service_cleanup_delay;
 *          HistoryQosPolicyKind history_kind;
 *          long history_depth;
 *          long max_samples;
 *          long max_instances;
 *          long max_samples_per_instance;
 *      };
 *
 * 5. EntityFactory
 *      IDL:
 *      struct EntityFactoryQosPolicy {
 *          boolean autoenable_created_entities;
 *      };
 *
 * 6. GroupData
 *      IDL:
 *      struct GroupDataQosPolicy {
 *          sequence<octet> value;
 *      };
 *
 * 7. History
 *      IDL:
 *      enum HistoryQosPolicyKind {
 *          KEEP_LAST_HISTORY_QOS,
 *          KEEP_ALL_HISTORY_QOS
 *      };
 *      struct HistoryQosPolicy {
 *          HistoryQosPolicyKind kind;
 *          long depth;
 *      };
 * 8. LatencyBudget
 *      IDL:
 *      struct LatencyBudgetQosPolicy {
 *          Duration_t duration;
 *      };
 *
 * 9. Lifespan
 *      IDL:
 *      struct LifespanQosPolicy {
 *          Duration_t duration;
 *      }
 *
 * 10.Liveliness
 *      IDL:
 *      enum LivelinessQosPolicyKind {
 *          AUTOMATIC_LIVELINESS_QOS,
 *          MANUAL_BY_PARTICIPANT_LIVELINESS_QOS,
 *          MANUAL_BY_TOPIC_LIVELINESS_QOS
 *      };
 *      struct LivelinessQosPolicy {
 *          LivelinessQosPolicyKind kind;
 *          Duration_t lease_duration;
 *      };
 *
 * 11.Ownership
 *      IDL:
 *      enum OwnershipQosPolicyKind {
 *          SHARED_OWNERSHIP_QOS,
 *          EXCLUSIVE_OWNERSHIP_QOS
 *      };
 *      struct OwnershipQosPolicy {
 *          OwnershipQosPolicyKind kind;
 *      };
 *
 * 12.OwnershipStrength
 *      IDL:
 *      struct OwnershipStrengthQosPolicy {
 *          long value;
 *      };
 *
 * 13.Partition
 *      IDL:
 *      struct PartitionQosPolicy {
 *          StringSeq name;
 *      };
 *
 * 14.Presentation
 *      IDL:
 *      enum PresentationQosPolicyAccessScopeKind {
 *          INSTANCE_PRESENTATION_QOS,
 *          TOPIC_PRESENTATION_QOS,
 *          GROUP_PRESENTATION_QOS
 *      };
 *      struct PresentationQosPolicy {
 *          PresentationQosPolicyAccessScopeKind access_scope;
 *          boolean coherent_access;
 *          boolean ordered_access;
 *      };
 *
 * 15.ReaderDataLifecycle
 *      IDL:
 *      struct ReaderDataLifecycleQosPolicy {
 *          Duration_t autopurge_nowriter_samples_delay;
 *          Duration_t autopurge_disposed_samples_delay;
 *      };
 *
 * 16.Reliability
 *      IDL:
 *      enum ReliabilityQosPolicyKind {
 *          BEST_EFFORT_RELIABILITY_QOS,
 *          RELIABLE_RELIABILITY_QOS
 *      };
 *      struct ReliabilityQosPolicy {
 *          ReliabilityQosPolicyKind kind;
 *          Duration_t max_blocking_time;
 *      };
 * 17.ResourceLimits
 *      IDL:
 *      struct ResourceLimitsQosPolicy {
 *          long max_samples;
 *          long max_instances;
 *          long max_samples_per_instance;
 *      };
 *
 * 18.TimeBasedFilter
 *      IDL:
 *      struct TimeBasedFilterQosPolicy {
 *          Duration_t minimum_separation;
 *      };
 *
 * 19.TopicData
 *      IDL:
 *      struct TopicDataQosPolicy {
 *          sequence<octet> value;
 *      };
 *
 * 20.TransportPriority
 *      IDL:
 *      struct TransportPriorityQosPolicy {
 *          long value;
 *      };
 *
 * 21.UserData
 *      IDL:
 *      struct UserDataQosPolicy {
 *          sequence<octet> value;
 *      };
 *
 * 22.WriterDataLifecycle
 *      IDL:
 *      struct WriterDataLifecycleQosPolicy {
 *          boolean autodispose_unregistered_instances;
 *      };
 *
 *-------------------------------------------------------------------------------------------------
 * QoS policis corresponding to entity: 
 * DomainParticipant: [2]  { EntityFactory, UserData }
 * Topic:             [13] { Deadline, DestinationOrder, Durability, DurabilityService, History,
 *                           LatencyBudget, LifeSpan, Liveliness, Ownership, Reliability,
 *                           ResourceLimits, TopicData, TransportPriority }
 * Publisher:         [4]  { EntityFactory, GroupData, Partition, Presentation }
 * Subscriber:        [4]  { EntityFactory, GroupData, Partition, Presentation }
 * DataWriter:        [14] { Deadline, DestinationOrder, Durability, History, LatencyBudget, 
 *                           LifeSpan, Liveliness, Ownership, OwnershipStrength, Reliability,
 *                           ResourceLimits, TransportPriority, UserData, WriterDataLifecycle }
 * DataReader:        [12] { Deadline, DestinationOrder, Durability, History, LatencyBudget
 *                           Liveliness, Ownership, Reliability, ReaderDataLifecycle, ResourceLimits,
 *                           TimeBasedFilter, UserData }
 *-------------------------------------------------------------------------------------------------
 */

#define UT_DDS_QOS_POLICY_NAME_DEADLINE                 "Deadline"
#define UT_DDS_QOS_POLICY_NAME_DESTINATION_ORDER        "DestinationOrder"
#define UT_DDS_QOS_POLICY_NAME_DURABILITY               "Durability"
#define UT_DDS_QOS_POLICY_NAME_DURABILITY_SERVICE       "DurabilityService"
#define UT_DDS_QOS_POLICY_NAME_ENTITY_FACTORY           "EntityFactory"
#define UT_DDS_QOS_POLICY_NAME_GROUP_DATA               "GroupData"
#define UT_DDS_QOS_POLICY_NAME_HISTORY                  "History"
#define UT_DDS_QOS_POLICY_NAME_LATENCY_BUDGET           "LatencyBudget"
#define UT_DDS_QOS_POLICY_NAME_LIFESPAN                 "LifeSpan"
#define UT_DDS_QOS_POLICY_NAME_LIVELINESS               "Liveliness"
#define UT_DDS_QOS_POLICY_NAME_OWNERSHIP                "Ownership"
#define UT_DDS_QOS_POLICY_NAME_OWNERSHIP_STRENGTH       "OwnershipStrength"
#define UT_DDS_QOS_POLICY_NAME_PARTITION                "Partition"
#define UT_DDS_QOS_POLICY_NAME_PRESENTATION             "Presentation"
#define UT_DDS_QOS_POLICY_NAME_READER_DATA_LIFECYCLE    "ReaderDataLifecycle"
#define UT_DDS_QOS_POLICY_NAME_RELIABILITY              "Reliability"
#define UT_DDS_QOS_POLICY_NAME_RESOURCE_LIMITS          "ResourceLimits"
#define UT_DDS_QOS_POLICY_NAME_TIME_BASED_FILTER        "TimeBasedFilter"
#define UT_DDS_QOS_POLICY_NAME_TOPIC_DATA               "TopicData"
#define UT_DDS_QOS_POLICY_NAME_TRANSPORT_PRIORITY       "TransportPriority"
#define UT_DDS_QOS_POLICY_NAME_WRITER_DATA_LIFECYCLE    "WriterDataLifecycle"
#define UT_DDS_QOS_POLICY_NAME_USER_DATA                "UserData"

#define UT_DDS_QOS_POLICY_MEMBER_NAME_KIND              "kind"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_PEROID            "peroid"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_CLEANUP_DELAY     "service_cleanup_delay"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_HISTORY_KIND      "history_kind"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_HISTORY_DEPTH     "history_depth"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_MAX_SAMPLES       "max_samples"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_MAX_INSTANCES     "max_instances"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_MAX_SAMPLES_PER_INSTANCE  "max_samples_per_instance"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_AUTO_ENABLE       "autoenable_created_entities"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_VALUE             "value"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_DEPTH             "depth"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_DURATION          "duration"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_LEASE_DURATION    "lease_duration"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_NAME              "name"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_ACCESS_SCOPE      "access_scope"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_COHERENT_ACCESS   "coherent_access"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_ORDERED_ACCESS    "ordered_access"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_MAX_BLOCKING_TIME "max_blocking_time"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_AUTODISPOSE_UNREGISETED_INSTANCES "autodispose_unregistered_instances"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_AUTOPURGE_NOWRITER_SAMPLES_DELAY  "autopurge_nowriter_samples_delay"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_AUTOPURGE_DISPOSED_SAMPLES_DELAY  "autopurge_disposed_samples_delay"
#define UT_DDS_QOS_POLICY_MEMBER_NAME_MIN_SEP           "minimum_separation"

#define UT_DDS_QOS_LENGTH_INF -1
#define UT_DDS_QOS_DURATION_INF -1

class DdsQosPolicyParameter
{
public:
    DdsQosPolicyParameter()
        : mDefault(true)
    {}

    virtual ~DdsQosPolicyParameter()
    {}

    virtual void Init(const JsonMap& data) = 0;

    void Update()
    {
        mDefault = false;
    }

    bool Default() const
    {
        return mDefault;
    }

protected:
    bool mDefault;
};

class DdsQosDeadlinePolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosDeadlinePolicyParameter() :
        mPeriod(UT_DDS_QOS_DURATION_INF)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_PEROID);
        if (iter != data.end())
        {
            mPeriod = AnyNumberCast<int64_t>(iter->second);
            Update();
        }
    }

    int64_t GetPeriod() const
    {
        return mPeriod;
    }

private:
    int64_t mPeriod;
};

class DdsQosDestinationOrderPolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosDestinationOrderPolicyParameter() :
        mKind(0)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_KIND);
        if (iter != data.end())
        {
            const std::string& kind = AnyCast<std::string>(iter->second);

            if (kind == "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS")
            {
                mKind = ::dds::core::policy::DestinationOrderKind::BY_RECEPTION_TIMESTAMP;
            }
            else if (kind == "BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS")
            {
                mKind = ::dds::core::policy::DestinationOrderKind::BY_SOURCE_TIMESTAMP;
            }

            Update();
        }
    }

    int32_t GetKind() const
    {
        return mKind;
    }

private:
    int32_t mKind;
};

class DdsQosDurabilityPolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosDurabilityPolicyParameter() :
        mKind(0)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_KIND);
        if (iter != data.end())
        {
            const std::string& kind = AnyCast<std::string>(iter->second);

            if (kind == "VOLATILE_DURABILITY_QOS")
            {
                mKind = ::dds::core::policy::DurabilityKind::VOLATILE;
            }
            else if (kind == "TRANSIENT_LOCAL_DURABILITY_QOS")
            {
                mKind = ::dds::core::policy::DurabilityKind::TRANSIENT_LOCAL;
            }
            else if (kind == "TRANSIENT_DURABILITY_QOS")
            {
                mKind = ::dds::core::policy::DurabilityKind::TRANSIENT;
            }
            else if (kind == "PERSISTENT_DURABILITY_QOS")
            {
                mKind = ::dds::core::policy::DurabilityKind::PERSISTENT;
            }

            Update();
        }
    }

    int32_t GetKind() const
    {
        return mKind;
    }

private:
    int32_t mKind;
};

class DdsQosDurabilityServicePolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosDurabilityServicePolicyParameter() :
        mCleanupDelay(0), mHistoryKind(0), mHistoryDepth(1), mMaxSamples(UT_DDS_QOS_LENGTH_INF),
        mMaxInstances(UT_DDS_QOS_LENGTH_INF), mMaxSamplesPerInstance(UT_DDS_QOS_LENGTH_INF)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_CLEANUP_DELAY);
        if (iter != data.end())
        {
            mCleanupDelay = AnyNumberCast<int64_t>(iter->second);
            Update();
        }

        iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_HISTORY_KIND);
        if (iter != data.end())
        {
            const std::string& kind = AnyCast<std::string>(iter->second);
            if (kind == "KEEP_LAST_HISTORY_QOS")
            {
                mHistoryKind = ::dds::core::policy::HistoryKind::KEEP_LAST;
            }
            else if (kind == "KEEP_ALL_HISTORY_QOS")
            {
                mHistoryKind = ::dds::core::policy::HistoryKind::KEEP_ALL;
            }
            Update();
        }

        iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_HISTORY_DEPTH);
        if (iter != data.end())
        {
            mHistoryDepth = AnyNumberCast<int32_t>(iter->second);
            Update();
        }

        iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_MAX_SAMPLES);
        if (iter != data.end())
        {
            mMaxSamples = AnyNumberCast<int32_t>(iter->second);
            Update();
        }

        iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_MAX_INSTANCES);
        if (iter != data.end())
        {
            mMaxInstances = AnyNumberCast<int32_t>(iter->second);
            Update();
        }

        iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_MAX_SAMPLES_PER_INSTANCE);
        if (iter != data.end())
        {
            mMaxSamplesPerInstance = AnyNumberCast<int32_t>(iter->second);        
            Update();
        }
    }

    int64_t GetCleanupDelay() const
    {
        return mCleanupDelay;
    }

    int32_t GetHistoryKind() const
    {
        return mHistoryKind;
    }

    int32_t GetHistoryDepth() const
    {
        return mHistoryDepth;
    }

    int32_t GetMaxSamples() const
    {
        return mMaxSamples;
    }

    int32_t GetMaxInstances() const
    {
        return mMaxInstances;
    }

    int32_t GetMaxSamplesPerInstance() const
    {
        return mMaxSamplesPerInstance;
    }

private:
    int64_t mCleanupDelay;
    int32_t mHistoryKind;
    int32_t mHistoryDepth;
    int32_t mMaxSamples;
    int32_t mMaxInstances;
    int32_t mMaxSamplesPerInstance;
};

class DdsQosEntityFactoryPolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosEntityFactoryPolicyParameter() :
        mAutoEnable(true)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_AUTO_ENABLE);
        if (iter != data.end())
        {
            mAutoEnable = (bool)AnyNumberCast<int32_t>(iter->second);
            Update();
        }
    }

    int64_t GetAutoEnable() const
    {
        return mAutoEnable;
    }

private:
    bool mAutoEnable;
};

class DdsQosGroupDataPolicyParameter: public DdsQosPolicyParameter
{
public:
    DdsQosGroupDataPolicyParameter()
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_VALUE);
        if (iter != data.end())
        {
            const JsonArray& arr = AnyCast<JsonArray>(iter->second);
            if (arr.empty())
            {
                return;
            }

            size_t i, n = arr.size();
            for (i=0; i<n; i++)
            {
                mValue.push_back(AnyNumberCast<uint8_t>(arr[i]));
            }

            Update();
        }
    }

    const std::vector<uint8_t>& GetValue() const
    {
        return mValue;
    }

private:
    std::vector<uint8_t> mValue;
};

class DdsQosHistoryPolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosHistoryPolicyParameter() :
        mKind(0), mDepth(1)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_KIND);
        if (iter != data.end())
        {
            const std::string& kind = AnyCast<std::string>(iter->second);
            if (kind == "KEEP_LAST_HISTORY_QOS")
            {
                mKind = ::dds::core::policy::HistoryKind::KEEP_LAST;
            }
            else if (kind == "KEEP_ALL_HISTORY_QOS")
            {
                mKind = ::dds::core::policy::HistoryKind::KEEP_ALL;
            }

            Update();
        }

        iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_DEPTH);
        if (iter != data.end())
        {
            mDepth = AnyNumberCast<int32_t>(iter->second);
            Update();
        }
    }

    int32_t GetKind() const
    {
        return mKind;
    }

    int32_t GetDepth() const
    {
        return mDepth;
    }
private:
    int32_t mKind;
    int32_t mDepth;
};

class DdsQosLatencyBudgetPolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosLatencyBudgetPolicyParameter() :
        mDuration(0)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_DURATION);
        if (iter != data.end())
        {
            mDuration = AnyNumberCast<int64_t>(iter->second);
            Update();
        }
    }

    int64_t GetDuration() const
    {
        return mDuration;
    }

private:
    int64_t mDuration;
};

class DdsQosLifespanPolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosLifespanPolicyParameter() :
        mDuration(UT_DDS_QOS_DURATION_INF)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_DURATION);
        if (iter != data.end())
        {
            mDuration = AnyNumberCast<int64_t>(iter->second);
            Update();
        }
    }

    int64_t GetDuration() const
    {
        return mDuration;
    }

private:
    int64_t mDuration;
};

class DdsQosLivelinessPolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosLivelinessPolicyParameter() :
        mKind(0), mLeaseDuration(UT_DDS_QOS_DURATION_INF)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_KIND);
        if (iter != data.end())
        {
            const std::string& kind = AnyCast<std::string>(iter->second);

            if (kind == "AUTOMATIC_LIVELINESS_QOS")
            {
                mKind = ::dds::core::policy::LivelinessKind::AUTOMATIC;
            }
            else if (kind == "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS")
            {
                mKind = ::dds::core::policy::LivelinessKind::MANUAL_BY_PARTICIPANT;
            }
            else if (kind == "MANUAL_BY_TOPIC_LIVELINESS_QOS")
            {
                mKind = ::dds::core::policy::LivelinessKind::MANUAL_BY_TOPIC;
            }

            Update();
        }

        iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_LEASE_DURATION);
        if (iter != data.end())
        {
            mLeaseDuration = AnyNumberCast<int64_t>(iter->second);
            Update();
        }
    }

    int32_t GetKind() const
    {
        return mKind;
    }

    int64_t GetLeaseDuration() const
    {
        return mLeaseDuration;
    }

private:
    int32_t mKind;
    int64_t mLeaseDuration;
};

class DdsQosOwnershipPolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosOwnershipPolicyParameter() :
        mKind(0)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_KIND);
        if (iter != data.end())
        {
            const std::string& kind = AnyCast<std::string>(iter->second);
            if (kind == "SHARED_OWNERSHIP_QOS")
            {
                mKind = ::dds::core::policy::OwnershipKind::SHARED;
            }
            else if (kind == "EXCLUSIVE_OWNERSHIP_QOS")
            {
                mKind = ::dds::core::policy::OwnershipKind::EXCLUSIVE;
            }

            Update();
        }
    }

    int32_t GetKind() const
    {
        return mKind;
    }

private:
    int32_t mKind;
};

class DdsQosOwnershipStrengthPolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosOwnershipStrengthPolicyParameter() :
        mValue(0)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_VALUE);
        if (iter != data.end())
        {
            mValue = AnyNumberCast<int32_t>(iter->second);
            Update();
        }
    }

    int32_t GetValue() const
    {
        return mValue;
    }

private:
    int32_t mValue;
};

class DdsQosPartitionPolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosPartitionPolicyParameter()
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_NAME);
        if (iter != data.end())
        {
            mName = AnyCast<std::string>(iter->second);
            Update();
        }
    }

    const std::string& GetName() const
    {
        return mName;
    }

private:
    std::string mName;
};

class DdsQosPresentationPolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosPresentationPolicyParameter() :
        mAccessScopeKind(0), mCoherentAccess(false), mOrderedAccess(false)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_ACCESS_SCOPE);
        if (iter != data.end())
        {
            const std::string& kind = AnyCast<std::string>(iter->second);
            if (kind == "INSTANCE_PRESENTATION_QOS")
            {
                mAccessScopeKind = ::dds::core::policy::PresentationAccessScopeKind::INSTANCE;
            }
            else if (kind == "TOPIC_PRESENTATION_QOS")
            {
                mAccessScopeKind = ::dds::core::policy::PresentationAccessScopeKind::TOPIC;
            }
            else if (kind == "GROUP_PRESENTATION_QOS")
            {
                mAccessScopeKind = ::dds::core::policy::PresentationAccessScopeKind::GROUP;
            }

            Update();
        }

        iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_COHERENT_ACCESS);
        if (iter != data.end())
        {
            mCoherentAccess = (bool)AnyNumberCast<int32_t>(iter->second);
            Update();
        }

        iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_ORDERED_ACCESS);
        if (iter != data.end())
        {
            mOrderedAccess = (bool)AnyNumberCast<int32_t>(iter->second);
            Update();
        }
    }

    int32_t GetAccessScopeKind() const
    {
        return mAccessScopeKind;
    }

    bool GetCoherentAccess() const
    {
        return mCoherentAccess;
    }

    bool GetOrderedAccess() const
    {
        return mOrderedAccess;
    }

private:
    int32_t mAccessScopeKind;
    bool mCoherentAccess;
    bool mOrderedAccess;
};

class DdsQosReaderDataLifecyclePolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosReaderDataLifecyclePolicyParameter() :
        mAutopurgeNowriterSamplesDelay(UT_DDS_QOS_DURATION_INF),
        mAutopurgeDisposedSamplesDelay(UT_DDS_QOS_DURATION_INF)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_AUTOPURGE_NOWRITER_SAMPLES_DELAY);
        if (iter != data.end())
        {
            mAutopurgeNowriterSamplesDelay = AnyNumberCast<int64_t>(iter->second);
            Update();
        }

        iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_AUTOPURGE_DISPOSED_SAMPLES_DELAY);
        if (iter != data.end())
        {
            mAutopurgeDisposedSamplesDelay = AnyNumberCast<int64_t>(iter->second);
            Update();
        }
    }

    int64_t GetAutopurgeNowriterSamplesDelay() const
    {
        return mAutopurgeNowriterSamplesDelay;
    }

    int64_t GetAutopurgeDisposedSamplesDelay() const
    {
        return mAutopurgeDisposedSamplesDelay;
    }

private:
    int64_t mAutopurgeNowriterSamplesDelay;
    int64_t mAutopurgeDisposedSamplesDelay;
};

class DdsQosReliabilityPolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosReliabilityPolicyParameter() :
        mKind(0), mMaxBlockingTime(UT_DDS_QOS_DURATION_INF)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_KIND);
        if (iter != data.end())
        {
            const std::string& kind = AnyCast<std::string>(iter->second);
            if (kind == "BEST_EFFORT_RELIABILITY_QOS")
            {
                mKind = ::dds::core::policy::ReliabilityKind::BEST_EFFORT;
            }
            else if (kind == "RELIABLE_RELIABILITY_QOS")
            {
                mKind = ::dds::core::policy::ReliabilityKind::RELIABLE;
            }

            Update();
        }

        iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_MAX_BLOCKING_TIME);
        if (iter != data.end())
        {
            mMaxBlockingTime = AnyNumberCast<int64_t>(iter->second);
            Update();
        }
    }

    int32_t GetKind() const
    {
        return mKind;
    }

    int64_t GetMaxBlockingTime() const
    {
        return mMaxBlockingTime;
    }

private:
    int32_t mKind;
    int64_t mMaxBlockingTime;
};

class DdsQosResourceLimitsPolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosResourceLimitsPolicyParameter() :
        mMaxSamples(UT_DDS_QOS_LENGTH_INF), mMaxInstances(UT_DDS_QOS_LENGTH_INF),
        mMaxSamplesPerInstance(UT_DDS_QOS_LENGTH_INF)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_MAX_SAMPLES);
        if (iter != data.end())
        {
            mMaxSamples = AnyNumberCast<int32_t>(iter->second);
            Update();
        }

        iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_MAX_INSTANCES);
        if (iter != data.end())
        {
            mMaxInstances = AnyNumberCast<int32_t>(iter->second);
            Update();
        }

        iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_MAX_SAMPLES_PER_INSTANCE);
        if (iter != data.end())
        {
            mMaxSamplesPerInstance = AnyNumberCast<int32_t>(iter->second);
            Update();
        }
    }

    int32_t GetMaxSamples() const
    {
        return mMaxSamples;
    }

    int32_t GetMaxInstances() const
    {
        return mMaxInstances;
    }

    int32_t GetMaxSamplesPerInstance() const
    {
        return mMaxSamplesPerInstance;
    }

private:
    int32_t mMaxSamples;
    int32_t mMaxInstances;
    int32_t mMaxSamplesPerInstance;
};

class DdsQosTimeBasedFilterPolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosTimeBasedFilterPolicyParameter() :
        mMinSep(0)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_MIN_SEP);
        if (iter != data.end())
        {
            mMinSep = AnyNumberCast<int64_t>(iter->second);
            Update();
        }
    }

    int64_t GetMinSep() const
    {
        return mMinSep;
    }

private:
    int64_t mMinSep;
};

class DdsQosTopicDataPolicyParameter: public DdsQosPolicyParameter
{
public:
    DdsQosTopicDataPolicyParameter()
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_VALUE);
        if (iter != data.end())
        {
            const JsonArray& arr = AnyCast<JsonArray>(iter->second);
            if (arr.empty())
            {
                return;
            }

            size_t i, n = arr.size();
            for (i=0; i<n; i++)
            {
                mValue.push_back(AnyNumberCast<uint8_t>(arr[i]));
            }

            Update();
        }
    }

    const std::vector<uint8_t>& GetValue() const
    {
        return mValue;
    }

private:
    std::vector<uint8_t> mValue;
};

class DdsQosTransportPriorityPolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosTransportPriorityPolicyParameter() :
        mValue(0)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_VALUE);
        if (iter != data.end())
        {
            mValue = AnyNumberCast<int32_t>(iter->second);
            Update();
        }
    }

    int32_t GetValue() const
    {
        return mValue;
    }

private:
    int32_t mValue;
};

class DdsQosWriterDataLifecyclePolicyParameter : public DdsQosPolicyParameter
{
public:
    DdsQosWriterDataLifecyclePolicyParameter() :
        mAutodisposeUnregisteredInstances(true)
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_AUTODISPOSE_UNREGISETED_INSTANCES);
        if (iter != data.end())
        {
            mAutodisposeUnregisteredInstances = (bool)AnyNumberCast<int32_t>(iter->second);
            Update();
        }
    }

    bool GetAutodisposeUnregisteredInstances() const
    {
        return mAutodisposeUnregisteredInstances;
    }

private:
    bool mAutodisposeUnregisteredInstances;
};

class DdsQosUserDataPolicyParameter: public DdsQosPolicyParameter
{
public:
    DdsQosUserDataPolicyParameter()
    {}

    void Init(const JsonMap& data)
    {
        if (data.empty())
        {
            return;
        }

        JsonMap::const_iterator iter = data.find(UT_DDS_QOS_POLICY_MEMBER_NAME_VALUE);
        if (iter != data.end())
        {
            const JsonArray& arr = AnyCast<JsonArray>(iter->second);
            if (arr.empty())
            {
                return;
            }

            size_t i, n = arr.size();
            for (i=0; i<n; i++)
            {
                mValue.push_back(AnyNumberCast<uint8_t>(arr[i]));
            }

            Update();
        }
    }

    const std::vector<uint8_t>& GetValue() const
    {
        return mValue;
    }

private:
    std::vector<uint8_t> mValue;
};

}
}

#endif//__UT_DDS_QOS_POLICY_PARAMETER_HPP__
