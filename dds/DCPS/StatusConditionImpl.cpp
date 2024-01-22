/*
 *
 *
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#include "DCPS/DdsDcps_pch.h" //Only the _pch include should start with DCPS/
#include "StatusConditionImpl.h"
#include "EntityImpl.h"

OPENDDS_BEGIN_VERSIONED_NAMESPACE_DECL

namespace OpenDDS {
namespace DCPS {

CORBA::Boolean StatusConditionImpl::get_trigger_value()
{
  ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, g, lock_, false);

  if (DCPS_debug_level > 9) {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("(%P|%t) StatusConditionImpl::get_trigger_value() - ")
               ACE_TEXT("mask==0x%x, changes==0x%x.\n"),
               this->mask_,
               this->parent_->get_status_changes()));
  }

  return (parent_->get_status_changes() & mask_) > 0;
}

DDS::StatusMask StatusConditionImpl::get_enabled_statuses()
{
  ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, g, lock_, 0);
  FILE *fp = fopen("/tmp/opendds-debug", "a+");
  fprintf(fp, "StatusConditionImpl::get_enabled_statuses\t%d\n", mask_);
  fclose(fp);
  return mask_;
}

DDS::ReturnCode_t
StatusConditionImpl::set_enabled_statuses(DDS::StatusMask mask)
{
  {
    ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, g, lock_,
                     DDS::RETCODE_OUT_OF_RESOURCES);
    mask_ = mask;
  }
  signal_all();
  FILE *fp = fopen("/tmp/opendds-debug", "a+");
  fprintf(fp, "StatusConditionImpl::set_enabled_statuses\t%d\n", DDS::RETCODE_OK);
  fclose(fp);
  return DDS::RETCODE_OK;
}

DDS::Entity_ptr StatusConditionImpl::get_entity()
{
  // NOTE::: Return Stack-based object reference?
  DDS::Entity_ptr entity = DDS::Entity::_duplicate(parent_);
  FILE *fp = fopen("/tmp/opendds-debug", "a+");
  fprintf(fp, "StatusConditionImpl::get_entity\t%d\n", entity);
  fclose(fp);
  return entity;
}

} // namespace DCPS
} // namespace OpenDDS

OPENDDS_END_VERSIONED_NAMESPACE_DECL
