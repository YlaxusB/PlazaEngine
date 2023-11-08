using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Plaza
{
    public static class Physics
    {
        public struct RaycastHit {
            public UInt64 hitUuid;
            public Vector3 point;
        }

        public static RaycastHit Raycast(Vector3 origin, Vector3 direction, float maxDistance)
        {
            InternalCalls.Physics_Raycast(origin, direction, maxDistance, out RaycastHit hit);
            return hit;
        }
    }
}
