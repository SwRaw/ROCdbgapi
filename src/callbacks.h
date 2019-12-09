/* Copyright (c) 2019 Advanced Micro Devices, Inc.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE. */

#ifndef _AMD_DBGAPI_CALLBACKS_H
#define _AMD_DBGAPI_CALLBACKS_H 1

#include "defs.h"

#include "debug.h"
#include "handle_object.h"

#include <functional>
#include <string>

namespace amd
{
namespace dbgapi
{

class process_t;

class shared_library_t
    : public detail::handle_object<amd_dbgapi_shared_library_id_t>
{
  using notify_callback_t = std::function<void (const shared_library_t *)>;

public:
  shared_library_t (amd_dbgapi_shared_library_id_t library_id,
                    process_t *process, std::string name,
                    notify_callback_t on_load, notify_callback_t on_unload);
  ~shared_library_t ();

  bool is_valid () const { return m_is_valid; }

  notify_callback_t callback (amd_dbgapi_shared_library_state_t state) const
  {
    dbgapi_assert (state == AMD_DBGAPI_SHARED_LIBRARY_STATE_LOADED
                   || state == AMD_DBGAPI_SHARED_LIBRARY_STATE_UNLOADED);

    return state == AMD_DBGAPI_SHARED_LIBRARY_STATE_LOADED ? m_on_load
                                                           : m_on_unload;
  }

  process_t *process () const { return m_process; }

private:
  bool m_is_valid = false;

  notify_callback_t const m_on_load;
  notify_callback_t const m_on_unload;

  process_t *const m_process;
};

class breakpoint_t : public detail::handle_object<amd_dbgapi_breakpoint_id_t>
{
  using action_callback_t = std::function<amd_dbgapi_status_t (
      breakpoint_t *, amd_dbgapi_client_thread_id_t,
      amd_dbgapi_breakpoint_action_t *)>;

  void set_state (amd_dbgapi_breakpoint_state_t state);

public:
  breakpoint_t (amd_dbgapi_breakpoint_id_t breakpoint_id,
                const shared_library_t *shared_library,
                amd_dbgapi_global_address_t address, action_callback_t action);

  ~breakpoint_t ();

  bool is_valid () const { return m_inserted; }

  amd_dbgapi_global_address_t address () const { return m_address; }
  action_callback_t action () const { return m_action; }

  void enable () { set_state (AMD_DBGAPI_BREAKPOINT_STATE_ENABLE); }
  void disable () { set_state (AMD_DBGAPI_BREAKPOINT_STATE_DISABLE); }

  const shared_library_t *shared_library () const { return m_shared_library; }
  process_t *process () const { return shared_library ()->process (); }

private:
  bool m_inserted{ false };

  amd_dbgapi_global_address_t const m_address;
  action_callback_t const m_action;

  const shared_library_t *const m_shared_library;
};

} /* namespace dbgapi */
} /* namespace amd */

#endif /* _AMD_DBGAPI_CALLBACKS_H */
