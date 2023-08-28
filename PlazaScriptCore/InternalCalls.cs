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
        public static Vector3 GetPosition()
        {
            GetPositionCall(out Vector3 vector);
            return vector;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SetPosition(ref Vector3 vector);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void GetPositionCall(out Vector3 vector);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool InputIsKeyDown(KeyCode keyCode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void MoveTowards(Vector3 vector3);
    }
}
