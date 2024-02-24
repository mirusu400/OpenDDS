/*
 *
 *
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#include "DCPS/DdsDcps_pch.h" //Only the _pch include should start with DCPS/
#include "TopicDescriptionImpl.h"
#include "DomainParticipantImpl.h"
#include "TopicImpl.h"
#include "Service_Participant.h"

OPENDDS_BEGIN_VERSIONED_NAMESPACE_DECL

namespace OpenDDS {
namespace DCPS {

TopicDescriptionImpl::TopicDescriptionImpl(const char*            topic_name,
                                           const char*            type_name,
                                           TypeSupport_ptr        type_support,
                                           DomainParticipantImpl* participant)
  : topic_name_(topic_name),
    type_name_(type_name),
    participant_(participant),
    type_support_(OpenDDS::DCPS::TypeSupport::_duplicate(type_support)),
    entity_refs_(0)
{
}

TopicDescriptionImpl::~TopicDescriptionImpl()
{
}

char *
TopicDescriptionImpl::get_type_name()
{
  FILE *fp = fopen("/tmp/opendds-debug", "a+");
  fprintf(fp, "TopicDescriptionImpl::get_type_name\t%s\n", CORBA::string_dup(type_name_.c_str()));
  fclose(fp);

  return CORBA::string_dup(type_name_.c_str());
}

char *
TopicDescriptionImpl::get_name()
{
  FILE *fp = fopen("/tmp/opendds-debug", "a+");
  fprintf(fp, "TopicDescriptionImpl::get_name\t%s\n", CORBA::string_dup(topic_name_.c_str()));
  fclose(fp);
  return CORBA::string_dup(topic_name_.c_str());
}

DDS::DomainParticipant_ptr
TopicDescriptionImpl::get_participant()
{
  DDS::DomainParticipant_ptr participant = DDS::DomainParticipant::_duplicate(participant_);
  FILE *fp = fopen("/tmp/opendds-debug", "a+");
  fprintf(fp, "TopicDescriptionImpl::get_participant\t%p\n", participant);
  fclose(fp);
  return participant;
}

OpenDDS::DCPS::TypeSupport_ptr
TopicDescriptionImpl::get_type_support()
{
  return type_support_;
}

} // namespace DCPS
} // namespace OpenDDS

OPENDDS_END_VERSIONED_NAMESPACE_DECL
