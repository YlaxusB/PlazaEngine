using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Plaza
{
    public class InternalCalls
    {

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void CppFunction();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Vector3Log(Vector3 vector);
    }
}
