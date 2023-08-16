#ifndef __UT_DDS_QOS_REALIZE_HPP__
#define __UT_DDS_QOS_REALIZE_HPP__

#include "dds_qos_parameter.hpp"
#include "dds_qos.hpp"

namespace unitree
{
namespace common
{
/*
 * QoS policis corresponding to entity: 
 * DomainParticipant: [2]  { EntityFactory, UserData }
 * Topic:             [13] { Deadline, DestinationOrder, Durability, DurabilityService, History,
 *                           LatencyBudget, LifeSpan, Liveliness, Ownership, Reliability,
 *                           ResourceLimits, TopicData, TransportPriority }
 * Publisher:         [4]  { EntityFactory, GroupData, Partition, Presentation }
 * Subscriber:        [4]  { EntityFactory, GroupData, Partition, Presentation }
 * DataWriter:        [14] { Deadline, DestinationOrder, Durability, History, LatencyBudget, LifeSpan,
 *                           Liveliness, Ownership, OwnershipStrength, Reliability, ResourceLimits,
 *                           TransportPriority, UserData, WriterDataLifecycle }
 * DataReader:        [12] { Deadline, DestinationOrder, Durability, History, LatencyBudget
 *                           Liveliness, Ownership, Reliability, ReaderDataLifecycle, ResourceLimits,
 *                           TimeBasedFilter, UserData }
 */
static inline void Realize(const DdsQosParameter& parameter, DdsParticipantQos& qos)
{
    //1
    const DdsQosEntityFactoryPolicyParameter& entityFactory = parameter.GetEntityFactory();
    if (!entityFactory.Default())
    {
        DdsQosEntityFactoryPolicy policy(entityFactory.GetAutoEnable());
        qos.SetPolicy<DdsQosEntityFactoryPolicy>(policy);
    }

    //2
    const DdsQosUserDataPolicyParameter& userData = parameter.GetUserData();
    if (!userData.Default())
    {
        DdsQosUserDataPolicy policy(userData.GetValue());
        qos.SetPolicy<DdsQosUserDataPolicy>(policy);
    }
}

static inline void Realize(const DdsQosParameter& parameter, DdsTopicQos& qos)
{
    //1
    const DdsQosDeadlinePolicyParameter& deadline = parameter.GetDeadline();
    if (!deadline.Default())
    {
        DdsQosDeadlinePolicy policy(deadline.GetPeriod());
        qos.SetPolicy<DdsQosDeadlinePolicy>(policy);
    }

    //2
    const DdsQosDestinationOrderPolicyParameter& destinationOrder = parameter.GetDestinationOrder();
    if (!destinationOrder.Default())
    {
        DdsQosDestinationOrderPolicy policy(destinationOrder.GetKind());
        qos.SetPolicy<DdsQosDestinationOrderPolicy>(policy);
    }

    //3
    const DdsQosDurabilityPolicyParameter& durability = parameter.GetDurability();
    if (!durability.Default())
    {
        DdsQosDurabilityPolicy policy(durability.GetKind());
        qos.SetPolicy<DdsQosDurabilityPolicy>(policy);
    }

    //4
    const DdsQosDurabilityServicePolicyParameter& durabilityService = parameter.GetDurabilityService();
    if (!durabilityService.Default())
    {
        DdsQosDurabilityServicePolicy policy(durabilityService.GetCleanupDelay(),
            durabilityService.GetHistoryKind(), durabilityService.GetHistoryDepth(),
            durabilityService.GetMaxSamples(), durabilityService.GetMaxInstances(),
            durabilityService.GetMaxSamplesPerInstance());
        qos.SetPolicy<DdsQosDurabilityServicePolicy>(policy);
    }

    //5
    const DdsQosHistoryPolicyParameter& history = parameter.GetHistory();
    if (!history.Default())
    {
        DdsQosHistoryPolicy policy(history.GetKind(), history.GetDepth());
        qos.SetPolicy<DdsQosHistoryPolicy>(policy);
    }

    //6
    const DdsQosLatencyBudgetPolicyParameter& latencyBudget = parameter.GetLatencyBudget();
    if (!latencyBudget.Default())
    {
        DdsQosLatencyBudgetPolicy policy(latencyBudget.GetDuration());
        qos.SetPolicy<DdsQosLatencyBudgetPolicy>(policy);
    }

    //7
    const DdsQosLifespanPolicyParameter& lifespan = parameter.GetLifespan();
    if (!lifespan.Default())
    {
        DdsQosLifespanPolicy policy(lifespan.GetDuration());
        qos.SetPolicy<DdsQosLifespanPolicy>(policy);
    }

    //8
    const DdsQosLivelinessPolicyParameter& liveliness = parameter.GetLiveliness();
    if (!liveliness.Default())
    {
        DdsQosLivelinessPolicy policy(liveliness.GetKind(), liveliness.GetLeaseDuration());
        qos.SetPolicy<DdsQosLivelinessPolicy>(policy);
    }

    //9
    const DdsQosOwnershipPolicyParameter& ownership = parameter.GetOwnership();
    if (!ownership.Default())
    {
        DdsQosOwnershipPolicy policy(ownership.GetKind());
        qos.SetPolicy<DdsQosOwnershipPolicy>(policy);
    }

    //10
    const DdsQosReliabilityPolicyParameter& reliability = parameter.GetReliability();
    if (!reliability.Default())
    {
        DdsQosReliabilityPolicy policy(reliability.GetKind(), reliability.GetMaxBlockingTime());
        qos.SetPolicy<DdsQosReliabilityPolicy>(policy);
    }

    //11
    const DdsQosResourceLimitsPolicyParameter& resourceLimites = parameter.GetResourceLimits();
    if (!resourceLimites.Default())
    {
        DdsQosResourceLimitsPolicy policy(resourceLimites.GetMaxSamples(), resourceLimites.GetMaxInstances(),
            resourceLimites.GetMaxSamplesPerInstance());
        qos.SetPolicy<DdsQosResourceLimitsPolicy>(policy);
    }

    //12
    const DdsQosTopicDataPolicyParameter& topicData = parameter.GetTopicData();
    if (!topicData.Default())
    {
        DdsQosTopicDataPolicy policy(topicData.GetValue());
        qos.SetPolicy<DdsQosTopicDataPolicy>(policy);
    }

    //13
    const DdsQosTransportPriorityPolicyParameter& transportPriority = parameter.GetTransportPriority();
    if (!transportPriority.Default())
    {
        DdsQosTransportPriorityPolicy policy(transportPriority.GetValue());
        qos.SetPolicy<DdsQosTransportPriorityPolicy>(policy);
    }
}

static inline void Realize(const DdsQosParameter& parameter, DdsPublisherQos& qos)
{
    //1
    const DdsQosEntityFactoryPolicyParameter& entityFactory = parameter.GetEntityFactory();
    if (!entityFactory.Default())
    {
        DdsQosEntityFactoryPolicy policy(entityFactory.GetAutoEnable());
        qos.SetPolicy<DdsQosEntityFactoryPolicy>(policy);
    }

    //2
    const DdsQosGroupDataPolicyParameter& groupData = parameter.GetGroupData();
    if (!groupData.Default())
    {
        DdsQosGroupDataPolicy policy(groupData.GetValue());
        qos.SetPolicy<DdsQosGroupDataPolicy>(policy);
    }

    //3
    const DdsQosPartitionPolicyParameter& partition = parameter.GetPartition();
    if (!partition.Default())
    {
        DdsQosPartitionPolicy policy(partition.GetName());
        qos.SetPolicy<DdsQosPartitionPolicy>(policy);
    }

    //4
    const DdsQosPresentationPolicyParameter& presentation = parameter.GetPresentation();
    if (!presentation.Default())
    {
        DdsQosPresentationPolicy policy(presentation.GetAccessScopeKind(),
            presentation.GetCoherentAccess(), presentation.GetOrderedAccess());
        qos.SetPolicy<DdsQosPresentationPolicy>(policy);
    }
}

static inline void Realize(const DdsQosParameter& parameter, DdsSubscriberQos& qos)
{
    //1
    const DdsQosEntityFactoryPolicyParameter& entityFactory = parameter.GetEntityFactory();
    if (!entityFactory.Default())
    {
        DdsQosEntityFactoryPolicy policy(entityFactory.GetAutoEnable());
        qos.SetPolicy<DdsQosEntityFactoryPolicy>(policy);
    }

    //2
    const DdsQosGroupDataPolicyParameter& groupData = parameter.GetGroupData();
    if (!groupData.Default())
    {
        DdsQosGroupDataPolicy policy(groupData.GetValue());
        qos.SetPolicy<DdsQosGroupDataPolicy>(policy);
    }

    //3
    const DdsQosPartitionPolicyParameter& partition = parameter.GetPartition();
    if (!partition.Default())
    {
        DdsQosPartitionPolicy policy(partition.GetName());
        qos.SetPolicy<DdsQosPartitionPolicy>(policy);
    }

    //4
    const DdsQosPresentationPolicyParameter& presentation = parameter.GetPresentation();
    if (!presentation.Default())
    {
        DdsQosPresentationPolicy policy(presentation.GetAccessScopeKind(),
            presentation.GetCoherentAccess(), presentation.GetOrderedAccess());
        qos.SetPolicy<DdsQosPresentationPolicy>(policy);
    }
}

static inline void Realize(const DdsQosParameter& parameter, DdsWriterQos& qos)
{
    //1
    const DdsQosDeadlinePolicyParameter& deadline = parameter.GetDeadline();
    if (!deadline.Default())
    {
        DdsQosDeadlinePolicy policy(deadline.GetPeriod());
        qos.SetPolicy<DdsQosDeadlinePolicy>(policy);
    }

    //2
    const DdsQosDestinationOrderPolicyParameter& destinationOrder = parameter.GetDestinationOrder();
    if (!destinationOrder.Default())
    {
        DdsQosDestinationOrderPolicy policy(destinationOrder.GetKind());
        qos.SetPolicy<DdsQosDestinationOrderPolicy>(policy);
    }

    //3
    const DdsQosDurabilityPolicyParameter& durability = parameter.GetDurability();
    if (!durability.Default())
    {
        DdsQosDurabilityPolicy policy(durability.GetKind());
        qos.SetPolicy<DdsQosDurabilityPolicy>(policy);
    }

    //4
    const DdsQosHistoryPolicyParameter& history = parameter.GetHistory();
    if (!history.Default())
    {
        DdsQosHistoryPolicy policy(history.GetKind(), history.GetDepth());
        qos.SetPolicy<DdsQosHistoryPolicy>(policy);
    }

    //5
    const DdsQosLatencyBudgetPolicyParameter& latencyBudget = parameter.GetLatencyBudget();
    if (!latencyBudget.Default())
    {
        DdsQosLatencyBudgetPolicy policy(latencyBudget.GetDuration());
        qos.SetPolicy<DdsQosLatencyBudgetPolicy>(policy);
    }

    //6
    const DdsQosLifespanPolicyParameter& lifespan = parameter.GetLifespan();
    if (!lifespan.Default())
    {
        DdsQosLifespanPolicy policy(lifespan.GetDuration());
        qos.SetPolicy<DdsQosLifespanPolicy>(policy);
    }

    //7
    const DdsQosLivelinessPolicyParameter& liveliness = parameter.GetLiveliness();
    if (!liveliness.Default())
    {
        DdsQosLivelinessPolicy policy(liveliness.GetKind(), liveliness.GetLeaseDuration());
        qos.SetPolicy<DdsQosLivelinessPolicy>(policy);
    }

    //8
    const DdsQosOwnershipPolicyParameter& ownership = parameter.GetOwnership();
    if (!ownership.Default())
    {
        DdsQosOwnershipPolicy policy(ownership.GetKind());
        qos.SetPolicy<DdsQosOwnershipPolicy>(policy);
    }

    //9
    const DdsQosOwnershipStrengthPolicyParameter& ownershipStrength = parameter.GetOwnershipStrength();
    if (!ownershipStrength.Default())
    {
        DdsQosOwnershipStrengthPolicy policy(ownershipStrength.GetValue());
        qos.SetPolicy<DdsQosOwnershipStrengthPolicy>(policy);
    }

    //10
    const DdsQosReliabilityPolicyParameter& reliability = parameter.GetReliability();
    if (!reliability.Default())
    {
        DdsQosReliabilityPolicy policy(reliability.GetKind(), reliability.GetMaxBlockingTime());
        qos.SetPolicy<DdsQosReliabilityPolicy>(policy);
    }

    //11
    const DdsQosResourceLimitsPolicyParameter& resourceLimites = parameter.GetResourceLimits();
    if (!resourceLimites.Default())
    {
        DdsQosResourceLimitsPolicy policy(resourceLimites.GetMaxSamples(), resourceLimites.GetMaxInstances(),
            resourceLimites.GetMaxSamplesPerInstance());
        qos.SetPolicy<DdsQosResourceLimitsPolicy>(policy);
    }

    //12
    const DdsQosTransportPriorityPolicyParameter& transportPriority = parameter.GetTransportPriority();
    if (!transportPriority.Default())
    {
        DdsQosTransportPriorityPolicy policy(transportPriority.GetValue());
        qos.SetPolicy<DdsQosTransportPriorityPolicy>(policy);
    }

    //13
    const DdsQosUserDataPolicyParameter& userData = parameter.GetUserData();
    if (!userData.Default())
    {
        DdsQosUserDataPolicy policy(userData.GetValue());
        qos.SetPolicy<DdsQosUserDataPolicy>(policy);
    }

    //14
    const DdsQosWriterDataLifecyclePolicyParameter& writerDataLifecycle = parameter.GetWriterDataLifecycle();
    if (!writerDataLifecycle.Default())
    {
        DdsQosWriterDataLifecyclePolicy policy(writerDataLifecycle.GetAutodisposeUnregisteredInstances());
        qos.SetPolicy<DdsQosWriterDataLifecyclePolicy>(policy);
    }
}

static inline void Realize(const DdsQosParameter& parameter, DdsReaderQos& qos)
{
    //1
    const DdsQosDeadlinePolicyParameter& deadline = parameter.GetDeadline();
    if (!deadline.Default())
    {
        DdsQosDeadlinePolicy policy(deadline.GetPeriod());
        qos.SetPolicy<DdsQosDeadlinePolicy>(policy);
    }

    //2
    const DdsQosDestinationOrderPolicyParameter& destinationOrder = parameter.GetDestinationOrder();
    if (!destinationOrder.Default())
    {
        DdsQosDestinationOrderPolicy policy(destinationOrder.GetKind());
        qos.SetPolicy<DdsQosDestinationOrderPolicy>(policy);
    }

    //3
    const DdsQosDurabilityPolicyParameter& durability = parameter.GetDurability();
    if (!durability.Default())
    {
        DdsQosDurabilityPolicy policy(durability.GetKind());
        qos.SetPolicy<DdsQosDurabilityPolicy>(policy);
    }

    //4
    const DdsQosHistoryPolicyParameter& history = parameter.GetHistory();
    if (!history.Default())
    {
        DdsQosHistoryPolicy policy(history.GetKind(), history.GetDepth());
        qos.SetPolicy<DdsQosHistoryPolicy>(policy);
    }

    //5
    const DdsQosLatencyBudgetPolicyParameter& latencyBudget = parameter.GetLatencyBudget();
    if (!latencyBudget.Default())
    {
        DdsQosLatencyBudgetPolicy policy(latencyBudget.GetDuration());
        qos.SetPolicy<DdsQosLatencyBudgetPolicy>(policy);
    }

    //6
    const DdsQosLivelinessPolicyParameter& liveliness = parameter.GetLiveliness();
    if (!liveliness.Default())
    {
        DdsQosLivelinessPolicy policy(liveliness.GetKind(), liveliness.GetLeaseDuration());
        qos.SetPolicy<DdsQosLivelinessPolicy>(policy);
    }

    //7
    const DdsQosOwnershipPolicyParameter& ownership = parameter.GetOwnership();
    if (!ownership.Default())
    {
        DdsQosOwnershipPolicy policy(ownership.GetKind());
        qos.SetPolicy<DdsQosOwnershipPolicy>(policy);
    }

    //8
    const DdsQosReliabilityPolicyParameter& reliability = parameter.GetReliability();
    if (!reliability.Default())
    {
        DdsQosReliabilityPolicy policy(reliability.GetKind(), reliability.GetMaxBlockingTime());
        qos.SetPolicy<DdsQosReliabilityPolicy>(policy);
    }

    //9
    const DdsQosReaderDataLifecyclePolicyParameter& readerDataLifecycle = parameter.GetReaderDataLifecycle();
    if (readerDataLifecycle.Default())
    {
        DdsQosReaderDataLifecyclePolicy policy(readerDataLifecycle.GetAutopurgeNowriterSamplesDelay(),
            readerDataLifecycle.GetAutopurgeDisposedSamplesDelay());
        qos.SetPolicy<DdsQosReaderDataLifecyclePolicy>(policy);
    }

    //10
    const DdsQosResourceLimitsPolicyParameter& resourceLimites = parameter.GetResourceLimits();
    if (!resourceLimites.Default())
    {
        DdsQosResourceLimitsPolicy policy(resourceLimites.GetMaxSamples(), resourceLimites.GetMaxInstances(),
            resourceLimites.GetMaxSamplesPerInstance());
        qos.SetPolicy<DdsQosResourceLimitsPolicy>(policy);
    }

    //11
    const DdsQosTimeBasedFilterPolicyParameter& timeBasedFilter = parameter.GetTimeBasedFilter();
    if (!timeBasedFilter.Default())
    {
        DdsQosTimeBasedFilterPolicy policy(timeBasedFilter.GetMinSep());
        qos.SetPolicy<DdsQosTimeBasedFilterPolicy>(policy);
    }

    //12
    const DdsQosUserDataPolicyParameter& userData = parameter.GetUserData();
    if (!userData.Default())
    {
        DdsQosUserDataPolicy policy(userData.GetValue());
        qos.SetPolicy<DdsQosUserDataPolicy>(policy);
    }
}

}
}

#endif//__UT_DDS_QOS_REALIZE_HPP__
