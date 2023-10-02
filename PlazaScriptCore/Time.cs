using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Plaza
{
    public static class Time
    {
        public static float deltaTime { get { return InternalCalls.Time_GetDeltaTime(); } }
    }
}
