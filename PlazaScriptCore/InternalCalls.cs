using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Plaza
{
    public static class InternalCalls
    {
        public static Vector3 GetPosition(UInt64 uuid)
        {
            GetPositionCall(uuid, out Vector3 vector);
            return vector;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool HasComponent(UInt64 uuid, Type componentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SetPosition(UInt64 uuid, ref Vector3 vector);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void GetPositionCall(UInt64 uuid, out Vector3 vector);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool InputIsKeyDown(KeyCode keyCode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void MoveTowards(UInt64 uuid, Vector3 vector3);
    }
}
