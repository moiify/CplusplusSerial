#ifndef __UT_DDS_QOS_POLICY_HPP__
#define __UT_DDS_QOS_POLICY_HPP__

#include <dds/dds.hpp>
#include "dds_native.hpp"

namespace unitree
{
namespace common
{
#define UT_DDS_LENGTH(length) \
    ((length) >= 0 ? (length) : ::dds::core::LENGTH_UNLIMITED)

class DdsDuration : public DdsNative<::dds::core::Duration>
{
public:
    DdsDuration(int64_t nanoSecond)
    {
        if (nanoSecond < 0)
        {
            mNative = ::dds::core::Duration::infinite();
        }
        else if (nanoSecond == 0)
        {
            mNative = ::dds::core::Duration::zero();
        }
        else
        {
            mNative = decltype(mNative)(nanoSecond/1000000000, nanoSecond%1000000000);
        }
    }
};

class DdsQosDeadlinePolicy : public DdsNative<::dds::core::policy::Deadline>
{
public:
    DdsQosDeadlinePolicy()
    {}

    DdsQosDeadlinePolicy(int64_t period)
    {
        mNative = decltype(mNative)(DdsDuration(period).GetNative());
    }

    void SetPeriod(int64_t& period)
    {
        mNative.period(DdsDuration(period).GetNative());
    }
};

class DdsQosDestinationOrderPolicy : public DdsNative<::dds::core::policy::DestinationOrder>
{
public:
    typedef ::dds::core::policy::DestinationOrderKind::Type KIND_TYPE;

    DdsQosDestinationOrderPolicy()
    {}

    DdsQosDestinationOrderPolicy(int32_t kind)
    {
        mNative = decltype(mNative)((KIND_TYPE)kind);
    }

public:
    void SetKind(int32_t& kind)
    {
        mNative.kind((KIND_TYPE)kind);
    }
};

class DdsQosDurabilityPolicy : public DdsNative<::dds::core::policy::Durability>
{
public:
    typedef ::dds::core::policy::DurabilityKind::Type KIND_TYPE;

    DdsQosDurabilityPolicy()
    {}

    DdsQosDurabilityPolicy(int32_t kind)
    {
        mNative = decltype(mNative)((KIND_TYPE)kind);
    }

public:
    void SetKind(int32_t kind)
    {
        mNative.kind((KIND_TYPE)kind);
    }
};

class DdsQosDurabilityServicePolicy : public DdsNative<::dds::core::policy::DurabilityService>
{
public:
    typedef ::dds::core::policy::HistoryKind::Type KIND_TYPE;

    DdsQosDurabilityServicePolicy()
    {}

    DdsQosDurabilityServicePolicy(int64_t cleanupDelay, int32_t historyKind, int32_t historyDepth,
        int32_t maxSamples, int32_t maxInstances, int32_t maxSamplesPerInstance)
    {
        mNative = decltype(mNative)(DdsDuration(cleanupDelay).GetNative(), (KIND_TYPE)historyKind,
            historyDepth, maxSamples, maxInstances, maxSamplesPerInstance);
    }

public:
    void SetCleanupDelay(int64_t cleanupDelay)
    {
        mNative.service_cleanup_delay(DdsDuration(cleanupDelay).GetNative());
    }

    void SetHistoryKind(int32_t historyKind)
    {
        mNative.history_kind((KIND_TYPE)historyKind);
    }

    void SetHistoryDepth(int32_t historyDepth)
    {
        mNative.history_depth(historyDepth);
    }

    void SetMaxSamples(int32_t maxSamples)
    {
        mNative.max_samples(maxSamples);
    }

    void SetMaxInstances(int32_t maxInstances)
    {
        mNative.max_instances(maxInstances);
    }

    void SetMaxSamplesPerInstance(int32_t maxSamplesPerInstance)
    {
        mNative.max_samples_per_instance(maxSamplesPerInstance);
    }
};

class DdsQosEntityFactoryPolicy : public DdsNative<::dds::core::policy::EntityFactory>
{
public:
    DdsQosEntityFactoryPolicy()
    {}

    DdsQosEntityFactoryPolicy(bool autoEnable)
    {
        mNative = decltype(mNative)(autoEnable);
    }

public:
    void SetAutoEnbale(bool autoEnable)
    {
        mNative.autoenable_created_entities(autoEnable);
    }
};

class DdsQosGroupDataPolicy : public DdsNative<::dds::core::policy::GroupData>
{
public:
    DdsQosGroupDataPolicy()
    {}

    DdsQosGroupDataPolicy(const std::vector<uint8_t>& value)
    {
        mNative = decltype(mNative)(value);
    }

public:
    void SetValue(const std::vector<uint8_t>& value)
    {
        mNative.value(value);
    }
};

class DdsQosHistoryPolicy : public DdsNative<::dds::core::policy::History>
{
public:
    typedef ::dds::core::policy::HistoryKind::Type KIND_TYPE;

    DdsQosHistoryPolicy()
    {}

    DdsQosHistoryPolicy(int32_t kind, int32_t depth)
    {
        mNative = decltype(mNative)((KIND_TYPE)kind, depth);
    }

public:
    void SetKind(int32_t kind)
    {
        mNative.kind((KIND_TYPE)kind);
    }

    void SetDepth(int32_t depth)
    {
        mNative.depth(depth);
    }
};

class DdsQosLatencyBudgetPolicy : public DdsNative<::dds::core::policy::LatencyBudget>
{
public:
    DdsQosLatencyBudgetPolicy()
    {}

    DdsQosLatencyBudgetPolicy(int64_t duration)
    {
        mNative = decltype(mNative)(DdsDuration(duration).GetNative());
    }

public:
    void SetDuration(int64_t duration)
    {
        mNative.duration(DdsDuration(duration).GetNative());
    }
};

class DdsQosLifespanPolicy : public DdsNative<::dds::core::policy::Lifespan>
{
public:
    DdsQosLifespanPolicy()
    {}

    DdsQosLifespanPolicy(int64_t duration)
    {
        mNative = decltype(mNative)(DdsDuration(duration).GetNative());
    }

public:
    void SetDuration(int64_t duration)
    {
        mNative.duration(DdsDuration(duration).GetNative());
    }
};

class DdsQosLivelinessPolicy : public DdsNative<::dds::core::policy::Liveliness>
{
public:
    typedef ::dds::core::policy::LivelinessKind::Type KIND_TYPE;

    DdsQosLivelinessPolicy()
    {}

    DdsQosLivelinessPolicy(int32_t kind, int64_t leaseDuration)
    {
        mNative = decltype(mNative)((KIND_TYPE)kind, DdsDuration(leaseDuration).GetNative());
    }

public:
    void SetKind(int32_t kind)
    {
        mNative.kind((KIND_TYPE)kind);
    }

    void SetLeaseDuration(int64_t leaseDuration)
    {
        mNative.lease_duration(DdsDuration(leaseDuration).GetNative());
    }
};

class DdsQosOwnershipPolicy : public DdsNative<::dds::core::policy::Ownership>
{
public:
    typedef ::dds::core::policy::OwnershipKind::Type KIND_TYPE;

    DdsQosOwnershipPolicy()
    {}

    DdsQosOwnershipPolicy(int32_t kind)
    {
        mNative = decltype(mNative)((KIND_TYPE)kind);
    }

public:
    void SetKind(int32_t kind)
    {
        mNative.kind((KIND_TYPE)kind);
    }
};

class DdsQosOwnershipStrengthPolicy : public DdsNative<::dds::core::policy::OwnershipStrength>
{
public:
    DdsQosOwnershipStrengthPolicy()
    {}

    DdsQosOwnershipStrengthPolicy(int32_t strength)
    {
        mNative = decltype(mNative)(strength);
    }

public:
    void SetValue(int32_t value)
    {
        mNative.value(value);
    }
};

class DdsQosPartitionPolicy : public DdsNative<::dds::core::policy::Partition>
{
public:
    DdsQosPartitionPolicy()
    {}

    DdsQosPartitionPolicy(const std::string& name)
    {
        mNative = decltype(mNative)(name);
    }

public:
    void SetName(const std::string& name)
    {
        mNative.name(name);
    }
};

class DdsQosPresentationPolicy : public DdsNative<::dds::core::policy::Presentation>
{
public:
    typedef ::dds::core::policy::PresentationAccessScopeKind::Type KIND_TYPE;

    DdsQosPresentationPolicy()
    {}

    DdsQosPresentationPolicy(int32_t accessScopeKind, bool coherentAccess, bool orderedAccess)
    {
        mNative = decltype(mNative)((KIND_TYPE)accessScopeKind, coherentAccess, orderedAccess);
    }

public:
    void SetAccessScopeKind(int32_t accessScopeKind)
    {
        mNative.access_scope((KIND_TYPE)accessScopeKind);
    }

    void SetCoherentAccess(bool coherentAccess)
    {
        mNative.coherent_access(coherentAccess);
    }

    void SetOrderedAccess(bool orderedAccess)
    {
        mNative.ordered_access(orderedAccess);
    }
};

class DdsQosReaderDataLifecyclePolicy : public DdsNative<::dds::core::policy::ReaderDataLifecycle>
{
public:
    DdsQosReaderDataLifecyclePolicy()
    {}

    DdsQosReaderDataLifecyclePolicy(int64_t autopurgeNowriterSamplesDelay, int64_t autopurgeDisposedSamplesDelay)
    {
        mNative = decltype(mNative)(DdsDuration(autopurgeNowriterSamplesDelay).GetNative(),
            DdsDuration(autopurgeDisposedSamplesDelay).GetNative());
    }

public:
    void SetAutopurgeNowriterSamplesDelay(int64_t autopurgeNowriterSamplesDelay)
    {
        mNative.autopurge_nowriter_samples_delay(DdsDuration(autopurgeNowriterSamplesDelay).GetNative());
    }

    void SetAutopurgeDisposedSamplesDelay(int64_t autopurgeDisposedSamplesDelay)
    {
        mNative.autopurge_disposed_samples_delay(DdsDuration(autopurgeDisposedSamplesDelay).GetNative());
    }
};

class DdsQosReliabilityPolicy : public DdsNative<::dds::core::policy::Reliability>
{
public:
    typedef ::dds::core::policy::ReliabilityKind::Type KIND_TYPE;

    DdsQosReliabilityPolicy()
    {}

    DdsQosReliabilityPolicy(int32_t kind, int64_t maxBlockingTime)
    {
        mNative = decltype(mNative)((KIND_TYPE)kind, DdsDuration(maxBlockingTime).GetNative());
    }

public:
    void SetKind(int32_t kind)
    {
        mNative.kind((KIND_TYPE)kind);
    }

    void SetBlockingTime(int64_t maxBlockingTime)
    {
        mNative.max_blocking_time(DdsDuration(maxBlockingTime).GetNative());
    }
};

class DdsQosResourceLimitsPolicy : public DdsNative<::dds::core::policy::ResourceLimits>
{
public:
    DdsQosResourceLimitsPolicy()
    {}

    DdsQosResourceLimitsPolicy(int32_t maxSamples, int32_t maxInstances, int32_t maxSamplesPerInstance)
    {
        mNative = decltype(mNative)(UT_DDS_LENGTH(maxSamples), UT_DDS_LENGTH(maxInstances), UT_DDS_LENGTH(maxSamplesPerInstance));
    }

public:
    void SetMaxSamples(int32_t maxSamples)
    {
        mNative.max_samples(maxSamples);
    }

    void SetMaxInstances(int32_t maxInstances)
    {
        mNative.max_instances(maxInstances);
    }

    void SetMaxSamplesPerInstance(int32_t maxSamplesPerInstance)
    {
        mNative.max_samples_per_instance(maxSamplesPerInstance);
    }
};

class DdsQosTimeBasedFilterPolicy : public DdsNative<::dds::core::policy::TimeBasedFilter>
{
public:
    DdsQosTimeBasedFilterPolicy()
    {}

    DdsQosTimeBasedFilterPolicy(int64_t minSep)
    {
        mNative = decltype(mNative)(DdsDuration(minSep).GetNative());
    }

public:
    void SetMinSep(int64_t minSep)
    {
        mNative.minimum_separation(DdsDuration(minSep).GetNative());
    }
};

class DdsQosTopicDataPolicy : public DdsNative<::dds::core::policy::TopicData>
{
public:
    DdsQosTopicDataPolicy()
    {}

    DdsQosTopicDataPolicy(const std::vector<uint8_t>& value)
    {
        mNative = decltype(mNative)(value);
    }

    void SetValue(const std::vector<uint8_t>& value)
    {
        mNative.value(value);
    }
};

class DdsQosTransportPriorityPolicy : public DdsNative<::dds::core::policy::TransportPriority>
{
public:
    DdsQosTransportPriorityPolicy()
    {}

    DdsQosTransportPriorityPolicy(int32_t value)
    {
        mNative = decltype(mNative)(value);
    }

    void SetValue(int32_t value)
    {
        mNative.value(value);
    }
};

class DdsQosWriterDataLifecyclePolicy : public DdsNative<::dds::core::policy::WriterDataLifecycle>
{
public:
    DdsQosWriterDataLifecyclePolicy()
    {}

    DdsQosWriterDataLifecyclePolicy(bool autodisposeUnregisteredInstances)
    {
        mNative = decltype(mNative)(autodisposeUnregisteredInstances);
    }

public:
    void SetAutodisposeUnregisteredInstances(bool autodisposeUnregisteredInstances)
    {
        mNative.autodispose_unregistered_instances(autodisposeUnregisteredInstances);
    }
};

class DdsQosUserDataPolicy : public DdsNative<::dds::core::policy::UserData>
{
public:
    DdsQosUserDataPolicy()
    {}

    DdsQosUserDataPolicy(const std::vector<uint8_t>& value)
    {
        mNative = decltype(mNative)(value);
    }

public:
    void SetValue(const std::vector<uint8_t>& value)
    {
        mNative.value(value);
    }
};

}
}
#endif//__UT_DDS_QOS_POLICY_HPP__
