/*
 *
 *
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#include "DCPS/DdsDcps_pch.h" //Only the _pch include should start with DCPS/
#include "ReadConditionImpl.h"
#include "DataReaderImpl.h"

OPENDDS_BEGIN_VERSIONED_NAMESPACE_DECL

namespace OpenDDS {
namespace DCPS {

CORBA::Boolean ReadConditionImpl::get_trigger_value()
{
  return parent_->contains_sample(sample_states_,
                                  view_states_, instance_states_);
}

DDS::SampleStateMask ReadConditionImpl::get_sample_state_mask()
{
  FILE *fp = fopen("/tmp/opendds-debug", "a+");
  fprintf(fp, "ReadConditionImpl::get_sample_state_mask() %d\n", sample_states_);
  fclose(fp);
  return sample_states_;
}

DDS::ViewStateMask ReadConditionImpl::get_view_state_mask()
{
  FILE *fp = fopen("/tmp/opendds-debug", "a+");
  fprintf(fp, "ReadConditionImpl::get_view_state_mask() %d\n", view_states_);
  fclose(fp);
  return view_states_;
}

DDS::InstanceStateMask ReadConditionImpl::get_instance_state_mask()
{
  FILE *fp = fopen("/tmp/opendds-debug", "a+");
  fprintf(fp, "ReadConditionImpl::get_instance_state_mask() %d\n", instance_states_);
  fclose(fp);
  return instance_states_;
}

DDS::DataReader_ptr ReadConditionImpl::get_datareader()
{
  DDS::DataReader_ptr ptr = DDS::DataReader::_duplicate(parent_);
  FILE *fp = fopen("/tmp/opendds-debug", "a+");
  fprintf(fp, "ReadConditionImpl::get_datareader() %p\n", ptr);
  fclose(fp);
  return ptr;
}

} // namespace DCPS
} // namespace OpenDDS

OPENDDS_END_VERSIONED_NAMESPACE_DECL
