#include "timing.hpp"
#include "nodes.hpp"

#include <emscripten/emscripten.h>

extern alia::millisecond_count the_millisecond_tick_count;

void
refresh();

static bool refresh_requested = false;

void
refresh(void*)
{
    refresh_requested = false;
    refresh();
}

namespace alia {

void
request_animation_refresh(dataless_context ctx)
{
    if (!refresh_requested)
    {
        emscripten_async_call(refresh, 0, -1);
        refresh_requested = true;
        // invoke virtual method on system interface
        // (Also set a flag indicating that a refresh is needed.)
    }
}

millisecond_count
get_raw_animation_tick_count(dataless_context ctx)
{
    if (is_refresh_event(ctx))
        request_animation_refresh(ctx);
    return the_millisecond_tick_count;
}

millisecond_count
get_raw_animation_ticks_left(dataless_context ctx, millisecond_count end_time)
{
    int ticks_remaining = int(end_time - the_millisecond_tick_count);
    if (ticks_remaining > 0)
    {
        if (is_refresh_event(ctx))
            request_animation_refresh(ctx);
        return millisecond_count(ticks_remaining);
    }
    return 0;
}

} // namespace alia
